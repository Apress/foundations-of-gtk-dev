#include <gtk/gtk.h>
#include <glade/glade.h>

void new_clicked (GtkToolButton*);
void open_clicked (GtkToolButton*);
void save_clicked (GtkToolButton*);
void cut_clicked (GtkToolButton*);
void copy_clicked (GtkToolButton*);
void paste_clicked (GtkToolButton*);
void find_clicked (GtkButton*);

static GladeXML *xml = NULL;

int main (int argc, 
          char *argv[])
{
  GtkWidget *window;

  gtk_init (&argc, &argv);

  xml = glade_xml_new ("exercise10-1.glade", NULL, NULL);
  window = glade_xml_get_widget (xml, "window");
  gtk_window_set_title (GTK_WINDOW (window), "Untitled");
  glade_xml_signal_autoconnect (xml);

  gtk_widget_show_all (window);
  gtk_main ();

  return 0;
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
  gint result;
  GtkTextBuffer *buffer;
  gchar *content, *file, *title;
  GtkTextIter start, end;
  
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
    switch (result)
    {
      case (GTK_RESPONSE_APPLY):
      {
        buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
        file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        gtk_text_buffer_get_bounds (buffer, &start, &end);
        content = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);
      
        g_file_set_contents (file, content, -1, NULL);
        gtk_window_set_title (GTK_WINDOW (window), file);
        g_free (content);
      }
      default:
        gtk_widget_destroy (dialog);
    }
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
