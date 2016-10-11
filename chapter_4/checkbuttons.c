#include <gtk/gtk.h>

static void destroy (GtkWidget*, gpointer);
static void check_toggled (GtkToggleButton*, GtkWidget*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *vbox, *check1, *check2, *close;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Check Buttons");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (destroy), NULL);

  check1 = gtk_check_button_new_with_label ("I am the main option.");
  check2 = gtk_check_button_new_with_label ("I rely on the other guy.");
  
  /* Only enable the second check button when the first is enabled. */
  gtk_widget_set_sensitive (check2, FALSE);
  g_signal_connect (G_OBJECT (check1), "toggled",
                    G_CALLBACK (check_toggled),
                    (gpointer) check2);
  
  close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  g_signal_connect_swapped (G_OBJECT (close), "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            (gpointer) window);
  
  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), check1, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), check2, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), close, FALSE, TRUE, 0);
  
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

/* If the main check button is active, enable the other. Otherwise, disable
 * the supplementary check button. */
static void
check_toggled (GtkToggleButton *check1,
               GtkWidget *check2)
{
  if (gtk_toggle_button_get_active (check1))
    gtk_widget_set_sensitive (check2, TRUE);
  else
    gtk_widget_set_sensitive (check2, FALSE);
}

static void
destroy (GtkWidget *window,
         gpointer data)
{
  gtk_main_quit ();
}
