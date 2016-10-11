#include <gtk/gtk.h>

static void previous_tab (GtkButton*, GtkNotebook*);
static void next_tab (GtkButton*, GtkNotebook*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *notebook, *paned, *hbox, *previous, *close;
  guint i;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Exercise 3-2");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 250, 200);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  notebook = gtk_notebook_new ();
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);
  
  previous = gtk_button_new_with_mnemonic ("_Previous Tab");
  g_signal_connect (G_OBJECT (previous), "clicked",
                    G_CALLBACK (previous_tab),
                    (gpointer) notebook);

  close = gtk_button_new_with_mnemonic ("_Close");
  g_signal_connect_swapped (G_OBJECT (close), "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            (gpointer) window);
  
  /* Create four GtkNotebook pages that will move to the next tab when the
   * child GtkButton is pressed. */
  for (i = 0; i < 4; i++)
  {
    GtkWidget *label, *button, *expander;
    
    label = gtk_label_new ("Tab");
    button = gtk_button_new_with_mnemonic ("_Next Tab");
    
    expander = gtk_expander_new (g_strdup_printf ("You Are Viewing Tab %i", i+1));
    gtk_expander_set_expanded (GTK_EXPANDER (expander), TRUE);
    gtk_container_add (GTK_CONTAINER (expander), button);
    
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), expander, label);
    gtk_container_set_border_width (GTK_CONTAINER (expander), 10);
    
    g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (next_tab),
                      (gpointer) notebook);
  }
  
  hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_end (GTK_BOX (hbox), close, FALSE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX (hbox), previous, FALSE, FALSE, 0);
  
  paned = gtk_vpaned_new ();
  gtk_paned_pack1 (GTK_PANED (paned), notebook, TRUE, FALSE);
  gtk_paned_pack1 (GTK_PANED (paned), hbox, TRUE, FALSE);

  gtk_container_add (GTK_CONTAINER (window), paned);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

/* Move to the previous tab in the GtkNotebook. */
static void 
previous_tab (GtkButton *button, 
              GtkNotebook *notebook)
{
  if (gtk_notebook_get_current_page (notebook) == 0)
    gtk_notebook_set_current_page (notebook, 3);
  else
    gtk_notebook_prev_page (notebook);
}

/* Move to the next tab in the GtkNotebook. */
static void 
next_tab (GtkButton *button, 
          GtkNotebook *notebook)
{
  if (gtk_notebook_get_current_page (notebook) == 3)
    gtk_notebook_set_current_page (notebook, 0);
  else
    gtk_notebook_next_page (notebook);
}
