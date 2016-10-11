#include <gtk/gtk.h>

static void destroy (GtkWidget*, gpointer);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *scrolled_win, *textview;
  GtkTextBuffer *buffer;
  PangoFontDescription *font;
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Text Views Properties");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 250, 150);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (destroy), NULL);
  
  font = pango_font_description_from_string ("Monospace Bold 10");
  textview = gtk_text_view_new ();
  gtk_widget_modify_font (textview, font);
  
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_WORD);
  gtk_text_view_set_justification (GTK_TEXT_VIEW (textview), GTK_JUSTIFY_RIGHT);

  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), TRUE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), TRUE);
  
  gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW (textview), 5);
  gtk_text_view_set_pixels_below_lines (GTK_TEXT_VIEW (textview), 5);
  gtk_text_view_set_pixels_inside_wrap (GTK_TEXT_VIEW (textview), 5);
  
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), 10);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (textview), 10);
  
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_text_buffer_set_text (buffer, "This is some text!\nChange me!\nPlease!", -1);
  
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  
  gtk_container_add (GTK_CONTAINER (scrolled_win), textview);
  gtk_container_add (GTK_CONTAINER (window), scrolled_win);
  gtk_widget_show_all (window);
  
  gtk_main();
  return 0;
}

static void
destroy (GtkWidget *window,
         gpointer data)
{
  gtk_main_quit ();
}
