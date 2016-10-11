#include <gtk/gtk.h>
#include <glib/gstdio.h>

static void save_clicked (GtkWindow*);
static void create_folder_clicked (GtkWindow*);
static void open_clicked (GtkWindow*);
static void select_folder_clicked (GtkWindow*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *vbox, *save, *create, *open, *select;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Exercise 5-1");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  save = gtk_button_new_with_label ("Save a File");
  create = gtk_button_new_with_label ("Create a New Folder");
  open = gtk_button_new_with_label ("Open One or More Files");
  select = gtk_button_new_with_label ("Select a Folder");
  
  g_signal_connect_swapped (G_OBJECT (save), "clicked",
                            G_CALLBACK (save_clicked), 
                            (gpointer) window);
  g_signal_connect_swapped  (G_OBJECT (create), "clicked",
                            G_CALLBACK (create_folder_clicked), 
                            (gpointer) window);
  g_signal_connect_swapped  (G_OBJECT (open), "clicked",
                            G_CALLBACK (open_clicked), 
                            (gpointer) window);
  g_signal_connect_swapped  (G_OBJECT (select), "clicked",
                            G_CALLBACK (select_folder_clicked), 
                            (gpointer) window);
  
  vbox = gtk_vbox_new (TRUE, 5);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), save);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), create);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), open);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), select);

  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

static void
save_clicked (GtkWindow *parent)
{
  GtkWidget *dialog, *chooser;
  gint result;
  gchar *filename;
  
  dialog = gtk_dialog_new_with_buttons ("Save File As ...", NULL,
                                        GTK_DIALOG_NO_SEPARATOR,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_SAVE, GTK_RESPONSE_APPLY,
                                        NULL);
  gtk_container_set_border_width (GTK_CONTAINER (dialog), 10);
  gtk_widget_set_size_request (dialog, 600, -1);

  /* Create a file chooser dialog that is used to save a file. */
  chooser = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_SAVE);

  gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), chooser);
  gtk_widget_show_all (dialog);

  result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_APPLY)
  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    g_print ("Saving file as: %s\n", filename);
    g_free (filename);
  }
  
  gtk_widget_destroy (dialog);
}

static void
create_folder_clicked (GtkWindow *parent)
{
  GtkWidget *dialog, *chooser;
  gint result;
  gchar *filename;
  
  dialog = gtk_dialog_new_with_buttons ("Create a Folder ...", NULL,
                                        GTK_DIALOG_NO_SEPARATOR,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OK, GTK_RESPONSE_OK,
                                        NULL);
  gtk_container_set_border_width (GTK_CONTAINER (dialog), 10);
  gtk_widget_set_size_request (dialog, 600, -1);
  
  /* Create a file chooser dialog that is used to create a folder. */
  chooser = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER);

  gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), chooser);
  gtk_widget_show_all (dialog);

  result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_OK)
  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    g_mkdir (filename, 0777);
    g_print ("Creating directory: %s\n", filename);
    g_free (filename);
  }
  
  gtk_widget_destroy (dialog);
}

static void
open_clicked (GtkWindow *parent)
{
  GtkWidget *dialog, *chooser;
  gint result;
  GSList *filenames, *ptr;
  
  dialog = gtk_dialog_new_with_buttons ("Open File(s) ...", NULL,
                                        GTK_DIALOG_NO_SEPARATOR,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                        NULL);
  gtk_container_set_border_width (GTK_CONTAINER (dialog), 10);
  gtk_widget_set_size_request (dialog, 600, 400);
  
  /* Create a file chooser dialog that is used to select multiple files. */
  chooser = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (chooser), TRUE);

  gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), chooser);
  gtk_widget_show_all (dialog);

  result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_ACCEPT)
  {
    filenames = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (chooser));
    
    for (ptr = filenames; ptr != NULL; ptr = ptr->next)
    {
      gchar *file = (gchar*) ptr->data;
      g_print ("%s was selected.\n", file);
    }
    
    g_slist_foreach (filenames, (GFunc) g_free, NULL);
    g_slist_free (filenames);
  }
  
  gtk_widget_destroy (dialog);
}

static void
select_folder_clicked (GtkWindow *parent)
{
  GtkWidget *dialog, *chooser;
  gint result;
  gchar *filename;
  
  dialog = gtk_dialog_new_with_buttons ("Select Folder ...", NULL,
                                        GTK_DIALOG_NO_SEPARATOR,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OK, GTK_RESPONSE_OK,
                                        NULL);
  gtk_container_set_border_width (GTK_CONTAINER (dialog), 10);
  gtk_widget_set_size_request (dialog, 600, 400);
  
  /* Create a file chooser dialog that is used to select a folder. */
  chooser = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);

  gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), chooser);
  gtk_widget_show_all (dialog);

  result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_OK)
  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    g_print ("Selected folder: %s\n", filename);
    g_free (filename);
  }
  
  gtk_widget_destroy (dialog);
}
