#include <gtk/gtk.h>
#include "myipaddress.h"

static void ip_address_changed (MyIPAddress*);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *ipaddress;
  gint address[4] = { 1, 20, 35, 255 };
  
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "GtkIPAddress");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  ipaddress = my_ip_address_new ();
  my_ip_address_set_address (MY_IP_ADDRESS (ipaddress), address);
  g_signal_connect (G_OBJECT (ipaddress), "ip-changed",
                    G_CALLBACK (ip_address_changed), NULL);
  
  gtk_container_add (GTK_CONTAINER (window), ipaddress);
  gtk_widget_show_all (window);
  
  gtk_main ();
  return 0;
}

/* When the IP address is changed, print the new value to the screen. */
static void 
ip_address_changed (MyIPAddress *ipaddress)
{
  gchar *address = my_ip_address_get_address (ipaddress);
  g_print ("%s\n", address);
  g_free (address);
}
