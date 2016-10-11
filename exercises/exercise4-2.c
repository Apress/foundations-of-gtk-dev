#include <gtk/gtk.h>

typedef struct
{
  GtkWidget *spin, *scale, *check;
} Widgets;

static void value_changed (GtkWidget*, Widgets*);

int main (int argc, 
          char *argv[])
{
  Widgets *w = (Widgets*) g_malloc (sizeof (Widgets));
  GtkWidget *window, *vbox;
  GtkAdjustment *adj1, *adj2;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Exercise 4-2");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  adj1 = GTK_ADJUSTMENT (gtk_adjustment_new (0.5, 0.0, 1.0, 0.01, 0.02, 0.02));
  adj2 = GTK_ADJUSTMENT (gtk_adjustment_new (0.5, 0.0, 1.02, 0.01, 0.02, 0.02));
  
  w->spin = gtk_spin_button_new (adj1, 0.01, 2);
  w->scale = gtk_hscale_new (adj2);
  w->check = gtk_check_button_new_with_label ("Synchronize Spin and Scale");
  
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w->check), TRUE);
  gtk_scale_set_digits (GTK_SCALE (w->scale), 2);
  gtk_scale_set_value_pos (GTK_SCALE (w->scale), GTK_POS_RIGHT);
  
  g_signal_connect (G_OBJECT (w->spin), "value_changed",
                    G_CALLBACK (value_changed), (gpointer) w);
  g_signal_connect (G_OBJECT (w->scale), "value_changed",
                    G_CALLBACK (value_changed), (gpointer) w);
  
  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), w->spin, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), w->scale, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), w->check, FALSE, TRUE, 0);

  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

static void
value_changed (GtkWidget *widget,
               Widgets *w)
{
  gdouble val1, val2;
  
  val1 = gtk_spin_button_get_value (GTK_SPIN_BUTTON (w->spin));
  val2 = gtk_range_get_value (GTK_RANGE (w->scale));

  /* Synchronize the widget's value based upon the type of "widget". */
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w->check)) && val1 != val2)
  {
    if (GTK_IS_SPIN_BUTTON (widget))
      gtk_range_set_value (GTK_RANGE (w->scale), val1);
    else
      gtk_spin_button_set_value (GTK_SPIN_BUTTON (w->spin), val2);
  }
}
