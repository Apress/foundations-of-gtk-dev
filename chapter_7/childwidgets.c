#include <gtk/gtk.h>

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *scrolled_win, *textview, *button;
  GtkTextChildAnchor *anchor;
  GtkTextIter iter;
  GtkTextBuffer *buffer;
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Child Widgets");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 250, 100);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  textview = gtk_text_view_new ();
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_text_buffer_set_text (buffer, "\n Click  to exit!", -1);

  /* Create a new child widget anchor at the specified iterator. */
  gtk_text_buffer_get_iter_at_offset (buffer, &iter, 8);
  anchor = gtk_text_buffer_create_child_anchor (buffer, &iter);

  /* Insert a GtkButton widget at the child anchor. */
  button = gtk_button_new_with_label ("the button");
  gtk_text_view_add_child_at_anchor (GTK_TEXT_VIEW (textview), button, anchor);

  g_signal_connect_swapped (G_OBJECT (button), "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            (gpointer) window);
  
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (scrolled_win), textview);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  
  gtk_container_add (GTK_CONTAINER (window), scrolled_win);
  gtk_widget_show_all (window);
  
  gtk_main();
  return 0;
}
