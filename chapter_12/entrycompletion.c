#include <gtk/gtk.h>

#define NUM_ELEMENTS 4

static gchar *widgets[] = { "GtkDialog", "GtkWindow", "GtkContainer", "GtkWidget" };

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *vbox, *label, *entry;
  GtkEntryCompletion *completion;
  GtkListStore *store;
  GtkTreeIter iter;
  unsigned int i;
  
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Automatic Completion");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  label = gtk_label_new ("Enter a widget in the following GtkEntry:");
  entry = gtk_entry_new ();
  
  /* Create a GtkListStore that will hold autocompletion possibilities. */
  store = gtk_list_store_new (1, G_TYPE_STRING);
  for (i = 0; i < NUM_ELEMENTS; i++)
  {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, widgets[i], -1);
  }

  completion = gtk_entry_completion_new ();
  gtk_entry_set_completion (GTK_ENTRY (entry), completion);
  gtk_entry_completion_set_model (completion, GTK_TREE_MODEL (store));
  gtk_entry_completion_set_text_column (completion, 0);
  
  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), entry, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);
  
  g_object_unref (completion);
  g_object_unref (store);
  
  gtk_main ();
  return 0;
}
