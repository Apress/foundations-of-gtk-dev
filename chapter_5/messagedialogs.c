#include <gtk/gtk.h>

static void button_clicked (GtkButton*, GtkWindow*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *button;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Message Dialogs");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  button = gtk_button_new_with_mnemonic ("_Click Me");

  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (button_clicked),
                    (gpointer) window);

  gtk_container_add (GTK_CONTAINER (window), button);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

/* Create a new message dialog that tells the user that the button was clicked. */
static void
button_clicked (GtkButton *button,
                GtkWindow *parent)
{
  GtkWidget *dialog;
  
  dialog = gtk_message_dialog_new (parent, GTK_DIALOG_MODAL,
                                   GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                   "The button was clicked!");
  gtk_window_set_title (GTK_WINDOW (dialog), "Information");

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}
