#include <gtk/gtk.h>

static void destroy (GtkWidget*, gpointer);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *vbox, *hbox, *question, *label, *pass;
  gchar *str;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Password?");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (destroy), NULL);

  str = g_strconcat ("What is the password for ", g_get_user_name(), "?", NULL);
  question = gtk_label_new (str);
  label = gtk_label_new ("Password:");

  /* Create a new GtkEntry widget and hide its content from view. */
  pass = gtk_entry_new ();
  gtk_entry_set_visibility (GTK_ENTRY (pass), FALSE);
  gtk_entry_set_invisible_char (GTK_ENTRY (pass), '*');

  hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start_defaults (GTK_BOX (hbox), label);
  gtk_box_pack_start_defaults (GTK_BOX (hbox), pass);

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), question);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox);

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
