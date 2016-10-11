#include <gtk/gtk.h>

static void destroy (GtkWidget*, gpointer);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *scale_int, *scale_float, *vbox;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Scales");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 250, -1);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (destroy), NULL);

  /* Create a scale that scrolls integers and one that scrolls floating ponit. */
  scale_int = gtk_hscale_new_with_range (0.0, 10.0, 1.0);
  scale_float = gtk_hscale_new_with_range (0.0, 1.0, 0.1);
  
  /* Set the number of decimal places to display for each widget. */
  gtk_scale_set_digits (GTK_SCALE (scale_int), 0);
  gtk_scale_set_digits (GTK_SCALE (scale_float), 1);
  
  /* Set the position of the value with respect to the widget. */
  gtk_scale_set_value_pos (GTK_SCALE (scale_int), GTK_POS_RIGHT);
  gtk_scale_set_value_pos (GTK_SCALE (scale_float), GTK_POS_LEFT);

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), scale_int);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), scale_float);

  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

static void
destroy (GtkWidget *window,
         gpointer data)
{
  gtk_main_quit ();
}
