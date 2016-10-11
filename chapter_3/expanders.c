#include <gtk/gtk.h>

static void destroy (GtkWidget*, gpointer);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *expander, *label;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Expander");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 200, 100);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (destroy), NULL);
  
  expander = gtk_expander_new_with_mnemonic ("Click _Me For More!");
  label = gtk_label_new ("Hide me or show me,\nthat is your choice.");
  
  gtk_container_add (GTK_CONTAINER (expander), label);
  gtk_expander_set_expanded (GTK_EXPANDER (expander), TRUE);
  gtk_container_add (GTK_CONTAINER (window), expander);

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
