#include <gtk/gtk.h>

#define FIRST_NAME "Andrew"
#define LAST_NAME "Krause"

static void destroy (GtkWidget*, gpointer);
static gboolean key_press (GtkWidget*, GdkEventKey*, GtkLabel*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *label;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL); 
  gtk_window_set_title (GTK_WINDOW (window), FIRST_NAME);
  gtk_widget_set_size_request (window, 300, 100); 
  gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (destroy), NULL);

  label = gtk_label_new (LAST_NAME);
  gtk_label_set_selectable (GTK_LABEL (label), TRUE);

  g_signal_connect (G_OBJECT (window), "key-press-event",
                    G_CALLBACK (key_press), (gpointer) label);

  gtk_container_add (GTK_CONTAINER (window), label);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

/* This callback function is called every time key-press-event is emitted
 * from the main GtkWindow. By placing the third parameter as a GtkLabel,
 * the gpointer is already cast and ready for use. */
static gboolean 
key_press (GtkWidget *window, 
           GdkEventKey *event, 
           GtkLabel *label)
{
  const gchar *text = gtk_label_get_text (GTK_LABEL (label));

  if (g_ascii_strcasecmp (text, FIRST_NAME) == 0)
  {
    gtk_window_set_title (GTK_WINDOW (window), FIRST_NAME);
    gtk_label_set_text (GTK_LABEL (label), LAST_NAME);
  }
  else
  {
    gtk_window_set_title (GTK_WINDOW (window), LAST_NAME);
    gtk_label_set_text (GTK_LABEL (label), FIRST_NAME);
  }

  return FALSE;
}

static void
destroy (GtkWidget *window,
         gpointer data)
{
  gtk_main_quit ();
}
