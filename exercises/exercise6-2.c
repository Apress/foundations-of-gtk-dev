#include <gtk/gtk.h>

typedef struct
{
  gint i;
  GtkWidget *label;
} Counting;

static gboolean timeout_function (Counting*);
static void button_clicked (GtkButton*, Counting*);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *button, *hbox;
  Counting *widget = g_slice_new (Counting);
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Exercise 6-2");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  widget->i = 0;
  widget->label = gtk_label_new ("0 Seconds");
  
  button = gtk_button_new_from_stock (GTK_STOCK_CLEAR);
  
  hbox = gtk_hbox_new (FALSE, 10);
  gtk_box_pack_start_defaults (GTK_BOX (hbox), widget->label);
  gtk_box_pack_start_defaults (GTK_BOX (hbox), button);
  
  gtk_container_add (GTK_CONTAINER (window), hbox);
  gtk_widget_show_all (window);
  
  /* Reset the counter when the button is clicked. */
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (button_clicked),
                    (gpointer) widget);
  
  /* Create a new timeout function that will count every second. */
  g_timeout_add_full (G_PRIORITY_DEFAULT,
                      1000,
                      (GSourceFunc) timeout_function,
                      (gpointer) widget,
                      NULL);
  
  gtk_main();
  return 0;
}

static gboolean 
timeout_function (Counting *widget)
{
  gchar *count;

  /* Create the count string based upon the current count. */
  if (widget->i == 0)
    count = "1 Second";
  else
    count = g_strdup_printf ("%i Seconds", widget->i + 1);
  
  /* Increment to the next second. */
  widget->i++;
  
  /* Set the new label text and return TRUE so the timout function will
   * continue to be called. */
  gtk_label_set_text (GTK_LABEL (widget->label), count);
  
  return TRUE;
}

static void 
button_clicked (GtkButton *button, 
                Counting *widget)
{
  /* Reset the count and the label. */
  widget->i = 0;
  gtk_label_set_text (GTK_LABEL (widget->label), "0 Seconds");
}
