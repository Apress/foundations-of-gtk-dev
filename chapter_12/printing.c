#include <gtk/gtk.h>
#include <math.h>

#define HEADER_HEIGHT 20.0
#define HEADER_GAP 8.5

/* A structure that will hold information about the current print job. */
typedef struct
{
  gchar *filename;
  gdouble font_size;
  gint lines_per_page;  
  gchar **lines;
  gint total_lines;
  gint total_pages;
} PrintData;

typedef struct
{
  GtkWidget *window, *chooser;
  PrintData *data;
} Widgets;

GtkPrintSettings *settings;

static void print_file (GtkButton*, Widgets*);
static void begin_print (GtkPrintOperation*, GtkPrintContext*, Widgets*);
static void draw_page (GtkPrintOperation*, GtkPrintContext*, gint, Widgets*);
static void end_print (GtkPrintOperation*, GtkPrintContext*, Widgets*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *hbox, *print;
  Widgets *w;

  gtk_init (&argc, &argv);

  w = g_slice_new (Widgets);
  w->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (w->window), "Printing");
  gtk_container_set_border_width (GTK_CONTAINER (w->window), 10);
  gtk_widget_set_size_request (w->window, 200, -1);

  g_signal_connect (G_OBJECT (w->window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  w->chooser = gtk_file_chooser_button_new ("Select a File",
                                            GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (w->chooser),
                                       g_get_home_dir ());
  
  print = gtk_button_new_from_stock (GTK_STOCK_PRINT);
  
  g_signal_connect (G_OBJECT (print), "clicked",
                    G_CALLBACK (print_file), (gpointer) w);
  
  hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), w->chooser, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), print, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (w->window), hbox);
  gtk_widget_show_all (w->window);
  
  gtk_main ();
  return 0;
}

/* Print the selected file with a font of "Monospace 10". */
static void 
print_file (GtkButton *button,
            Widgets *w)
{
  GtkPrintOperation *operation;
  GtkWidget *dialog;
  GError *error = NULL;
  gchar *filename;
  gint res;

  /* Return if a file has not been selected because there is nothing to print. */
  filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (w->chooser));
  if (filename == NULL)
    return;

  /* Create a new print operation, applying saved print settings if they exist. */
  operation = gtk_print_operation_new ();
  if (settings != NULL)
    gtk_print_operation_set_print_settings (operation, settings);
  
  w->data = g_slice_new (PrintData);
  w->data->filename = g_strdup (filename);
  w->data->font_size = 10.0;

  g_signal_connect (G_OBJECT (operation), "begin_print", 
		                G_CALLBACK (begin_print), (gpointer) w);
  g_signal_connect (G_OBJECT (operation), "draw_page", 
		                G_CALLBACK (draw_page), (gpointer) w);
  g_signal_connect (G_OBJECT (operation), "end_print", 
		                G_CALLBACK (end_print), (gpointer) w);

  /* Run the default print operation that will print the selected file. */
  res = gtk_print_operation_run (operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, 
                                 GTK_WINDOW (w->window), &error);

  /* If the print operation was accepted, save the new print settings. */
  if (res == GTK_PRINT_OPERATION_RESULT_APPLY)
  {
    if (settings != NULL)
      g_object_unref (settings);
    settings = g_object_ref (gtk_print_operation_get_print_settings (operation));
  }
  /* Otherwise, report that the print operation has failed. */
  else if (error)
  {
    dialog = gtk_message_dialog_new (GTK_WINDOW (w->window), 
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
				                             error->message);
    
    g_error_free (error);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);     
  }
  
  g_object_unref (operation);
  g_free (filename);
}

/* Begin the printing by retrieving the contents of the selected files and
 * spliting it into single lines of text. */
static void
begin_print (GtkPrintOperation *operation, 
             GtkPrintContext *context,
             Widgets *w)
{
  gchar *contents;
  gdouble height;
  gsize length;

  /* Retrieve the file contents and split it into lines of text. */
  g_file_get_contents (w->data->filename, &contents, &length, NULL);
  w->data->lines = g_strsplit (contents, "\n", 0);

  /* Count the total number of lines in the file. */
  w->data->total_lines = 0;
  while (w->data->lines[w->data->total_lines] != NULL)
    w->data->total_lines++;
  
  /* Based on the height of the page and font size, calculate how many lines can be 
   * rendered on a single page. A padding of 3 is placed between lines as well. */
  height = gtk_print_context_get_height (context) - HEADER_HEIGHT - HEADER_GAP;
  w->data->lines_per_page = floor (height / (w->data->font_size + 3));
  w->data->total_pages = (w->data->total_lines - 1) / w->data->lines_per_page + 1;
  gtk_print_operation_set_n_pages (operation, w->data->total_pages);
  g_free (contents);
}

/* Draw the page, which includes a header with the file name and page number along
 * with one page of text with a font of "Monospace 10". */
static void
draw_page (GtkPrintOperation *operation,
           GtkPrintContext *context,
           gint page_nr,
           Widgets *w)
{
  cairo_t *cr;
  PangoLayout *layout;
  gdouble width, text_height;
  gint line, i, text_width, layout_height;
  PangoFontDescription *desc;
  gchar *page_str;

  cr = gtk_print_context_get_cairo_context (context);
  width = gtk_print_context_get_width (context);
  layout = gtk_print_context_create_pango_layout (context);
  desc = pango_font_description_from_string ("Monospace");
  pango_font_description_set_size (desc, w->data->font_size * PANGO_SCALE);

  /* Render the page header with the filename and page number. */
  pango_layout_set_font_description (layout, desc);
  pango_layout_set_text (layout, w->data->filename, -1);
  pango_layout_set_width (layout, -1);
  pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
  pango_layout_get_size (layout, NULL, &layout_height);
  text_height = (gdouble) layout_height / PANGO_SCALE;

  cairo_move_to (cr, 0, (HEADER_HEIGHT - text_height) / 2);
  pango_cairo_show_layout (cr, layout);

  page_str = g_strdup_printf ("%d of %d", page_nr + 1, w->data->total_pages);
  pango_layout_set_text (layout, page_str, -1);
  pango_layout_get_size (layout, &text_width, NULL);
  pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);

  cairo_move_to (cr, width - (text_width / PANGO_SCALE), 
                 (HEADER_HEIGHT - text_height) / 2);
  pango_cairo_show_layout (cr, layout);
  
  /* Render the page text with the specified font and size. */  
  cairo_move_to (cr, 0, HEADER_HEIGHT + HEADER_GAP);
  line = page_nr * w->data->lines_per_page;
  for (i = 0; i < w->data->lines_per_page && line < w->data->total_lines; i++) 
  {
    pango_layout_set_text (layout, w->data->lines[line], -1);
    pango_cairo_show_layout (cr, layout);
    cairo_rel_move_to (cr, 0, w->data->font_size + 3);
    line++;
  }

  g_free (page_str);
  g_object_unref (layout);
  pango_font_description_free (desc);
}

/* Clean up after the printing operation since it is done. */
static void
end_print (GtkPrintOperation *operation, 
           GtkPrintContext *context,
           Widgets *w)
{
  g_strfreev (w->data->lines);
  g_slice_free1 (sizeof (PrintData), w->data);
  w->data = NULL;
}
