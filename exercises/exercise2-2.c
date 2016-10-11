#include <gtk/gtk.h>

#define FIRST_NAME "Andrew"
#define LAST_NAME "Krause"

static void destroy (GtkWidget*, gpointer);
static gboolean key_press (GtkWidget*, GdkEventKey*, GtkLabel*);
static void title_changed (GObject*, GParamSpec*, gpointer);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *label;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL); 
  g_object_set (window, "title", FIRST_NAME, "width-request", 300,
                "height-request", 100, "resizable", FALSE, NULL);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (destroy), NULL);

  label = gtk_label_new (LAST_NAME);
  g_object_set (label, "selectable", TRUE, NULL);

  g_signal_connect (G_OBJECT (window), "key-press-event",
                    G_CALLBACK (key_press), (gpointer) label);
  g_signal_connect (G_OBJECT (window), "notify::title",
                    G_CALLBACK (title_changed), NULL);

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
  gchar *text;
  g_object_get (label, "label", &text, NULL);

  if (g_ascii_strcasecmp (text, FIRST_NAME) == 0)
  {
    g_object_set (window, "title", FIRST_NAME, NULL);
    g_object_set (label, "label", LAST_NAME, NULL);
  }
  else
  {
    g_object_set (window, "title", LAST_NAME, NULL);
    g_object_set (label, "label", FIRST_NAME, NULL);
  }

  return FALSE;
}

/* Notify the user that the window title was changed. */
static void 
title_changed (GObject *window, 
               GParamSpec *property, 
               gpointer data)
{
  gchar *text;
  g_object_get (window, "title", &text, NULL);
  g_message ("The title was changed to '%s'!", text);
}

static void
destroy (GtkWidget *window,
         gpointer data)
{
  gtk_main_quit ();
}
