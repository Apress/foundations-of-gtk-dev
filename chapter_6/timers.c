#include <gtk/gtk.h>

static void button_clicked (GtkButton*, GTimer*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *button;
  GTimer *timer;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Timers");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 150, 75);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  /* Initialize the timer. */
  timer = g_timer_new ();
  button = gtk_button_new_with_label ("Start Timer");

  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (button_clicked),
                    (gpointer) timer);

  gtk_container_add (GTK_CONTAINER (window), button);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

/* Count the amount of elapsed time between two button clicks. */
static void 
button_clicked (GtkButton *button, 
                GTimer *timer)
{
  static gdouble start_time = 0.0;
  static gdouble end_time = 0.0;
  static gboolean running = FALSE;

  if (!running)
  {
    start_time = g_timer_elapsed (timer, NULL);
    gtk_button_set_label (button, "Stop Timer");
  }
  else
  {
    end_time = g_timer_elapsed (timer, NULL);
    gtk_button_set_label (button, "Start Timer");
    g_print ("Elapsed Time: %.2f\n", end_time - start_time);
  }
  
  running = !running;
}
