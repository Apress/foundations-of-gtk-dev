#include <gtk/gtk.h>

typedef struct
{
  GtkWidget *entry;
  GtkWidget *chooser;
} Widgets;

static void save_the_file (GtkButton*, Widgets*);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *vbox, *button;
  Widgets *widgets = g_slice_new (Widgets);
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Exercise 6-1");
  gtk_widget_set_size_request (window, 250, -1);
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  widgets->entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (widgets->entry), 
                      "Enter some text and choose a location");
  
  widgets->chooser = gtk_file_chooser_button_new ("Choose a Location ...",
                                      GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (widgets->chooser),
                                       g_get_home_dir ());
  
  button = gtk_button_new_from_stock (GTK_STOCK_SAVE);
  
  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), widgets->entry, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), widgets->chooser, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);
  
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (save_the_file),
                    (gpointer) widgets);
  
  gtk_main();
  return 0;
}

static void 
save_the_file (GtkButton *button, 
               Widgets *widgets)
{
  gchar *filename;
  const gchar *content;
  GError *error = NULL;
  
  /* Create the path to the file by appending a file name to the folder. */
  filename = g_strconcat (gtk_file_chooser_get_filename (GTK_FILE_CHOOSER 
                          (widgets->chooser)), "/arbitrary_file", NULL);
  
  /* Retrieve the content that will be written to the file. */
  content = gtk_entry_get_text (GTK_ENTRY (widgets->entry));
  
  /* Write the text to the file and give a critical error message if 
   * the action was unsuccessful. */
  g_file_set_contents (filename, content, -1, &error);
  
  if (error != NULL)
    g_warning (error->message);
  g_free (filename);
}
