#include <gtk/gtk.h>
#include <glade/glade.h>
#include <math.h>
#include <string.h>

static void begin_print (GtkPrintOperation*, GtkPrintContext*);
static void draw_page (GtkPrintOperation*, GtkPrintContext*, gint);
static void end_print (GtkPrintOperation*, GtkPrintContext*);
static void menu_activated (GtkMenuShell*);

void new_clicked (GtkToolButton*);
void open_clicked (GtkToolButton*);
void save_clicked (GtkToolButton*);
void cut_clicked (GtkToolButton*);
void copy_clicked (GtkToolButton*);
void paste_clicked (GtkToolButton*);
void print_clicked (GtkToolButton*);
void find_clicked (GtkButton*);

#define HEADER_HEIGHT 19.84
#define HEADER_GAP 8.5

typedef struct
{
  gdouble font_size;
  gint lines_per_page;  
  gchar **lines;
  gint total_lines;
  gint total_pages;
} PrintData;

static GtkPrintSettings *settings = NULL;
static GladeXML *xml = NULL;
static PrintData *data = NULL;

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *textview, *open, *menu;
  PangoFontDescription *fd;
  GtkRecentManager *manager;

  gtk_init (&argc, &argv);

  xml = glade_xml_new ("exercise12-1.glade", NULL, NULL);
  window = glade_xml_get_widget (xml, "window");
  textview = glade_xml_get_widget (xml, "textview");
  open = glade_xml_get_widget (xml, "open");
  settings = gtk_print_settings_new ();
  
  fd = pango_font_description_from_string ("Monospace 10");
  gtk_widget_modify_font (textview, fd);
  gtk_window_set_title (GTK_WINDOW (window), "Untitled");
  glade_xml_signal_autoconnect (xml);
  pango_font_description_free (fd);
  
  manager = gtk_recent_manager_get_default ();
  menu = gtk_recent_chooser_menu_new_for_manager (manager);
  gtk_menu_tool_button_set_menu (GTK_MENU_TOOL_BUTTON (open), menu);
  
  gtk_recent_chooser_set_show_not_found (GTK_RECENT_CHOOSER (menu), FALSE);
  gtk_recent_chooser_set_local_only (GTK_RECENT_CHOOSER (menu), TRUE);
  gtk_recent_chooser_set_limit (GTK_RECENT_CHOOSER (menu), 10);
  gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER (menu),
                                    GTK_RECENT_SORT_MRU);
  
  g_signal_connect (G_OBJECT (menu), "selection-done",
                    G_CALLBACK (menu_activated), NULL);

  gtk_widget_show_all (window);
  gtk_main ();

  return 0;
}

/* A menu item was activated. So, retrieve the file URI and open it. */
static void 
menu_activated (GtkMenuShell *menu)
{
  GtkTextBuffer *buffer;
  gchar *filename, *content;
  GtkWidget *window, *textview;
  gsize length;

  window = glade_xml_get_widget (xml, "window");
  textview = glade_xml_get_widget (xml, "textview");
  filename = gtk_recent_chooser_get_current_uri (GTK_RECENT_CHOOSER (menu));
    
  if (filename != NULL)
  {
    /* Remove the "file://" prefix from the beginning of the URI if it exists. */
    if (g_ascii_strncasecmp (filename, "file://", 7) == 0)
    {
      length = strlen (filename);
      g_memmove (filename, filename + 7, length - 7);
      filename[length-7] = '\0';
    }
    
    if (g_file_get_contents (filename, &content, &length, NULL))
    {
      gtk_window_set_title (GTK_WINDOW (window), filename);
      buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
      gtk_text_buffer_set_text (buffer, content, -1);
      g_free (content);
    }
    else
      g_warning ("The file '%s' could not be read!", filename);

    g_free (filename);
  }
}

/* Verify that the user want to create a new document. If so, delete
 * all of the text from the buffer. */
void 
new_clicked (GtkToolButton *new)
{
  GtkWidget *dialog, *textview, *window;
  GtkTextBuffer *buffer;
  gint result;

  textview = glade_xml_get_widget (xml, "textview");
  window = glade_xml_get_widget (xml, "window");
  dialog = gtk_message_dialog_new (GTK_WINDOW (window), GTK_DIALOG_MODAL, 
                       GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, 
                       "All changes will be lost. Do you want to continue?");

  result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_YES)
  {
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    gtk_text_buffer_set_text (buffer, "", -1);
    gtk_window_set_title (GTK_WINDOW (window), "Untitled");
  }

  gtk_widget_destroy (dialog);
}

/* Replace the content of the current buffer with the content of a file. */
void 
open_clicked (GtkToolButton *open)
{
  GtkWidget *dialog, *textview, *window;
  gint result;
  GtkTextBuffer *buffer;
  gchar *content, *file;
  
  window = glade_xml_get_widget (xml, "window");
  textview = glade_xml_get_widget (xml, "textview");
  dialog = gtk_file_chooser_dialog_new ("Choose a file ..", GTK_WINDOW (window),
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_APPLY,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        NULL);
  
  result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_APPLY)
  {
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

    g_file_get_contents (file, &content, NULL, NULL);
    gtk_text_buffer_set_text (buffer, content, -1);
    gtk_window_set_title (GTK_WINDOW (window), file);
      
    g_free (content);
    g_free (file);
  }

  gtk_widget_destroy (dialog);
}

/* Save the content of the current buffer to a file. */
void 
save_clicked (GtkToolButton *cut)
{
  GtkWidget *dialog, *textview, *window;
  GtkTextBuffer *buffer;
  gchar *content, *file, *title;
  GtkTextIter start, end;
  gint result;
  
  textview = glade_xml_get_widget (xml, "textview");
  window = glade_xml_get_widget (xml, "window");
  title = (gchar*) gtk_window_get_title (GTK_WINDOW (window));
  
  if (g_ascii_strcasecmp (title, "Untitled") == 0)
  {
    dialog = gtk_file_chooser_dialog_new ("Save the file ..", GTK_WINDOW (window),
                                          GTK_FILE_CHOOSER_ACTION_SAVE,
                                          GTK_STOCK_SAVE, GTK_RESPONSE_APPLY,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                          NULL);
  
    result = gtk_dialog_run (GTK_DIALOG (dialog));
    if (result == GTK_RESPONSE_APPLY)
    {
      buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
      file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      gtk_text_buffer_get_bounds (buffer, &start, &end);
      content = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);
      
      g_file_set_contents (file, content, -1, NULL);
      gtk_window_set_title (GTK_WINDOW (window), file);
      g_free (content);
    }

    gtk_widget_destroy (dialog);
  }
  else
  {
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    gtk_text_buffer_get_bounds (buffer, &start, &end);
    content = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);
    g_file_set_contents (title, content, -1, NULL);
    g_free (content);
  }
}

/* Copy the selection to the clipboard and remove it from the buffer. */
void
cut_clicked (GtkToolButton *cut)
{
  GtkWidget *textview;
  GtkClipboard *clipboard;
  GtkTextBuffer *buffer;
  
  textview = glade_xml_get_widget (xml, "textview");
  clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_text_buffer_cut_clipboard (buffer, clipboard, TRUE);
}

/* Copy the selection to the clipboard. */
void 
copy_clicked (GtkToolButton *copy)
{
  GtkWidget *textview;
  GtkClipboard *clipboard;
  GtkTextBuffer *buffer;
  
  textview = glade_xml_get_widget (xml, "textview");
  clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_text_buffer_copy_clipboard (buffer, clipboard);
}

/* Delete any selected text and insert the clipboard content into
 * the document. */
void
paste_clicked (GtkToolButton *paste)
{
  GtkWidget *textview;
  GtkClipboard *clipboard;
  GtkTextBuffer *buffer;
  
  textview = glade_xml_get_widget (xml, "textview");
  clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_text_buffer_paste_clipboard (buffer, clipboard, NULL, TRUE);
}

/* Search for a text string from the current cursor position if there is no
 * selected text, or one character after the cursor if there is. */
void 
find_clicked (GtkButton *cut)
{
  const gchar *find;
  GtkTextBuffer *buffer; 
  GtkTextIter start, begin, end;
  GtkWidget *search, *textview;
  GtkTextMark *mark;
  GtkWidget *dialog;
  gboolean success;
  
  search = glade_xml_get_widget (xml, "entry");
  textview = glade_xml_get_widget (xml, "textview");
  find = gtk_entry_get_text (GTK_ENTRY (search));
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  
  gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
  
  if (gtk_text_iter_compare (&start, &end) != 0)
    gtk_text_iter_forward_char (&start);

  success = gtk_text_iter_forward_search (&start, find, 0, &begin, &end, NULL);
  
  /* Select the instance on the screen if the string is found. Otherwise, tell
   * the user it has failed. */
  if (success)
  {
    mark = gtk_text_buffer_create_mark (buffer, NULL, &begin, FALSE);
    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (textview), mark);
    gtk_text_buffer_delete_mark (buffer, mark);

    gtk_text_buffer_select_range (buffer, &begin, &end);
  }
  else
  {
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, 
                                     GTK_MESSAGE_INFO, GTK_BUTTONS_OK, 
                                     "The text was not found!");
    
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
  }
}

void 
print_clicked (GtkToolButton *button)
{
  GtkPrintOperation *operation;
  GtkWidget *dialog, *window;
  GError *error = NULL;
  gint res;

  operation = gtk_print_operation_new ();
  if (settings != NULL)
    gtk_print_operation_set_print_settings (operation, settings);
  
  data = g_slice_new (PrintData);
  data->font_size = 10.0;

  g_signal_connect (G_OBJECT (operation), "begin_print", 
		                G_CALLBACK (begin_print), NULL);
  g_signal_connect (G_OBJECT (operation), "draw_page", 
		                G_CALLBACK (draw_page), NULL);
  g_signal_connect (G_OBJECT (operation), "end_print", 
		                G_CALLBACK (end_print), NULL);

  window = glade_xml_get_widget (xml, "window");
  res = gtk_print_operation_run (operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, 
                                 GTK_WINDOW (window), &error);

  if (res == GTK_PRINT_OPERATION_RESULT_APPLY)
  {
    if (settings != NULL)
      g_object_unref (settings);
    settings = g_object_ref (gtk_print_operation_get_print_settings (operation));
  }
  else if (error)
  {
    dialog = gtk_message_dialog_new (GTK_WINDOW (window), 
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
				                             error->message);
    
    g_error_free (error);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);     
  }
  
  g_object_unref (operation);
}

/* Begin the printing by retrieving the contents of the selected files and
 * spliting it into single lines of text. */
static void
begin_print (GtkPrintOperation *operation, 
             GtkPrintContext *context)
{
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  GtkWidget *textview;
  gchar *contents;
  gdouble height;

  textview = glade_xml_get_widget (xml, "textview");
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_text_buffer_get_bounds (buffer, &start, &end);
  contents = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
  data->lines = g_strsplit (contents, "\n", 0);

  data->total_lines = 0;
  while (data->lines[data->total_lines] != NULL)
    data->total_lines++;
  
  height = gtk_print_context_get_height (context) - HEADER_HEIGHT - HEADER_GAP;
  data->lines_per_page = floor (height / (data->font_size + 3));
  data->total_pages = (data->total_lines - 1) / data->lines_per_page + 1;
  gtk_print_operation_set_n_pages (operation, data->total_pages);
  g_free (contents);
}

/* Draw the page, which includes a header with the file name and page number along
 * with one page of text with a font of "Monospace 10". */
static void
draw_page (GtkPrintOperation *operation,
           GtkPrintContext *context,
           gint page_nr)
{
  cairo_t *cr;
  PangoLayout *layout;
  gdouble width, text_height;
  gint line, i, text_width, layout_height;
  PangoFontDescription *desc;
  gchar *page_str;
  GtkWidget *window;

  cr = gtk_print_context_get_cairo_context (context);
  width = gtk_print_context_get_width (context);
  layout = gtk_print_context_create_pango_layout (context);
  desc = pango_font_description_from_string ("Monospace");
  pango_font_description_set_size (desc, data->font_size * PANGO_SCALE);

  /* Render the page header with the filename and page number. */
  window = glade_xml_get_widget (xml, "window");
  pango_layout_set_font_description (layout, desc);
  pango_layout_set_text (layout, gtk_window_get_title (GTK_WINDOW (window)), -1);
  pango_layout_set_width (layout, -1);
  pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
  pango_layout_get_size (layout, NULL, &layout_height);
  text_height = (gdouble) layout_height / PANGO_SCALE;

  cairo_move_to (cr, 0, (HEADER_HEIGHT - text_height) / 2);
  pango_cairo_show_layout (cr, layout);

  page_str = g_strdup_printf ("%d of %d", page_nr + 1, data->total_pages);
  pango_layout_set_text (layout, page_str, -1);
  pango_layout_get_size (layout, &text_width, NULL);
  pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);

  cairo_move_to (cr, width - (text_width / PANGO_SCALE), 
                 (HEADER_HEIGHT - text_height) / 2);
  pango_cairo_show_layout (cr, layout);
  
  /* Render the page text with the specified font and size. */  
  cairo_move_to (cr, 0, HEADER_HEIGHT + HEADER_GAP);
  line = page_nr * data->lines_per_page;
  for (i = 0; i < data->lines_per_page && line < data->total_lines; i++) 
  {
    pango_layout_set_text (layout, data->lines[line], -1);
    pango_cairo_show_layout (cr, layout);
    cairo_rel_move_to (cr, 0, data->font_size + 3);
    line++;
  }

  pango_font_description_free (desc);
  g_object_unref (layout);
  g_free (page_str);
}

/* Clean up after the printing operation since it is done. */
static void
end_print (GtkPrintOperation *operation, 
           GtkPrintContext *context)
{
  g_strfreev (data->lines);
  g_slice_free1 (sizeof (PrintData),data);
  data = NULL;
}
