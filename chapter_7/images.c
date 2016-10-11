#include <gtk/gtk.h>

#define IMAGE_UNDO "/path/to/undo.png"
#define IMAGE_REDO "/path/to/redo.png"

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *scrolled_win, *textview;
  GdkPixbuf *undo, *redo;
  GtkTextIter line;
  GtkTextBuffer *buffer;
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Pixbufs");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 200, 150);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  textview = gtk_text_view_new ();
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_text_buffer_set_text (buffer, " Undo\n Redo", -1);

  /* Create two images and insert them into the text buffer. */
  undo = gdk_pixbuf_new_from_file (IMAGE_UNDO, NULL);
  gtk_text_buffer_get_iter_at_line (buffer, &line, 0);
  gtk_text_buffer_insert_pixbuf (buffer, &line, undo);

  redo = gdk_pixbuf_new_from_file (IMAGE_REDO, NULL);
  gtk_text_buffer_get_iter_at_line (buffer, &line, 1);
  gtk_text_buffer_insert_pixbuf (buffer, &line, redo);
  
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (scrolled_win), textview);
  gtk_container_add (GTK_CONTAINER (window), scrolled_win);
  gtk_widget_show_all (window);
  
  gtk_main();
  return 0;
}
