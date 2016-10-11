#include <gtk/gtk.h>

static void button_clicked (GtkButton*, GtkWindow*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *button;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Open Files");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  button = gtk_button_new_with_label ("Open Files");

  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (button_clicked), 
                    (gpointer) window);

  gtk_container_add (GTK_CONTAINER (window), button);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

/* Show a file chooser dialog and allow the user to choose multiple files. */
static void
button_clicked (GtkButton *button,
                GtkWindow *window)
{
  GtkWidget *dialog;
  GSList *filenames;
  
  dialog = gtk_file_chooser_dialog_new ("Open File(s) ...", window,
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                        NULL);

  /* Allow the user to choose more than one file at a time. */
  gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), TRUE);

  gint result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_ACCEPT)
  {
    filenames = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (dialog));
    
    while (filenames != NULL)
    {
      gchar *file = (gchar*) filenames->data;
      g_print ("%s was selected.\n", file);
      filenames = filenames->next;
    }
  }

  gtk_widget_destroy (dialog);
}
