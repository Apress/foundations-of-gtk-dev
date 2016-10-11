#include <gtk/gtk.h>

static gboolean pulse_progress (GtkProgressBar*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *progress;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Timeouts");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 200, -1);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  progress = gtk_progress_bar_new ();
  gtk_progress_bar_set_pulse_step (GTK_PROGRESS_BAR (progress), 0.1);
  
  g_timeout_add (100, (GSourceFunc) pulse_progress, (gpointer) progress);

  gtk_container_add (GTK_CONTAINER (window), progress);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

/* Pulse the progress bar and return TRUE so the timeout is called again. */
static gboolean 
pulse_progress (GtkProgressBar *progress)
{
  static gint count = 0;

  gtk_progress_bar_pulse (progress);
  i++;

  return (i < 25);
}
