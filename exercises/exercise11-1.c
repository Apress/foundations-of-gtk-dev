#include <gtk/gtk.h>
#include "mymarquee.h"

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *marquee;
  
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Exercise 11-1");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  
  g_signal_connect (G_OBJECT (window), "destroy",
		                G_CALLBACK (gtk_main_quit), NULL);
  
  marquee = my_marquee_new ("Wheeeee!");
  my_marquee_add_message (MY_MARQUEE (marquee), "Wheeeee Again!");
  my_marquee_set_speed (MY_MARQUEE (marquee), 10);
  my_marquee_set_scroll_dir (MY_MARQUEE (marquee), MY_MARQUEE_SCROLL_RIGHT);
  
  g_timeout_add (150, (GSourceFunc) my_marquee_slide, (gpointer) marquee);
  
  gtk_container_add (GTK_CONTAINER (window), marquee);
  gtk_widget_show_all (window);
  
  gtk_main ();
  return 0;
}
