#include <gtk/gtk.h>
#include "mymarquee.h"

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *marquee;
  PangoFontDescription *fd;
  
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "GtkMarquee");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  
  g_signal_connect (G_OBJECT (window), "destroy",
		                G_CALLBACK (gtk_main_quit), NULL);
  
  fd = pango_font_description_from_string ("Monospace 30");
  marquee = my_marquee_new ();
  gtk_widget_modify_font (marquee, fd);
  my_marquee_set_message (MY_MARQUEE (marquee), "Wheeeee!");
  my_marquee_set_speed (MY_MARQUEE (marquee), 10);
  pango_font_description_free (fd);
  
  g_timeout_add (150, (GSourceFunc) my_marquee_slide, (gpointer) marquee);
  
  gtk_container_add (GTK_CONTAINER (window), marquee);
  gtk_widget_show_all (window);
  
  gtk_main ();
  return 0;
}
