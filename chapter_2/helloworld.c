#include <gtk/gtk.h>

int main (int argc,
          char *argv[])
{
  GtkWidget *window;

  /* Initialize GTK+ and all of its supporting libraries. */
  gtk_init (&argc, &argv);

  /* Create a new window, give it a title and display it to the user. */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Hello World");
  gtk_widget_show (window);

  /* Hand control over to the main loop. */
  gtk_main ();
  return 0;
}
