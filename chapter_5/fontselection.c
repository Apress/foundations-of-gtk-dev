#include <gtk/gtk.h>

static void font_dialog_response (GtkFontSelectionDialog*, gint, gpointer);

int main (int argc, 
          char *argv[])
{
  GtkWidget *dialog;

  gtk_init (&argc, &argv);

  /* Use the font selection dialog as the toplevel widget. */
  dialog = gtk_font_selection_dialog_new ("Choose a Font");
  gtk_font_selection_dialog_set_font_name (GTK_FONT_SELECTION_DIALOG (dialog),
                                           "Sans Bold Italic 12");
  gtk_font_selection_dialog_set_preview_text (GTK_FONT_SELECTION_DIALOG (dialog),
                                              "Foundations of GTK+ Development");

  g_signal_connect (G_OBJECT (dialog), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (G_OBJECT (dialog), "response",
                    G_CALLBACK (font_dialog_response), NULL);

  gtk_widget_show_all (dialog);

  gtk_main ();
  return 0;
}

/* If the user presses "Apply", display the font, but do not destroy the dialog. If
 * "OK" is pressed, display the font and destroy the dialog. Otherwise, just destroy
 * the dialog. */
static void
font_dialog_response (GtkFontSelectionDialog *dialog,
                      gint response,
                      gpointer data)
{
  gchar *font;
  GtkWidget *message;
  
  switch (response)
  {
  case (GTK_RESPONSE_APPLY):
  case (GTK_RESPONSE_OK):
    font = gtk_font_selection_dialog_get_font_name (dialog);
    message = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_INFO, GTK_BUTTONS_OK, font);
    gtk_window_set_title (GTK_WINDOW (message), "Selected Font");

    gtk_dialog_run (GTK_DIALOG (message));
    gtk_widget_destroy (message);
    g_free (font);
    break;
  default:
    gtk_widget_destroy (GTK_WIDGET (dialog));
  }
  
  if (response == GTK_RESPONSE_OK)
    gtk_widget_destroy (GTK_WIDGET (dialog));
}
