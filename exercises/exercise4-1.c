#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <unistd.h>

typedef struct
{
  GtkWidget *file, *name, *rnm;
} Widgets;

static void file_changed (GtkFileChooser*, Widgets*);
static void rename_clicked (GtkButton*, Widgets*);

int main (int argc, 
          char *argv[])
{
  Widgets *w = (Widgets*) g_malloc (sizeof (Widgets));
  GtkWidget *window, *vbox, *hbox;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Exercise 4-1");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  w->rnm = gtk_button_new_from_stock (GTK_STOCK_APPLY);
  w->name = gtk_entry_new ();
  gtk_widget_set_sensitive (w->rnm, FALSE);
  gtk_widget_set_sensitive (w->name, FALSE);
  
  w->file = gtk_file_chooser_button_new ("Choose File", 
                                         GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (w->file),
                                       g_get_home_dir());
  
  g_signal_connect (G_OBJECT (w->file), "selection-changed",
                    G_CALLBACK (file_changed), (gpointer) w);
  g_signal_connect (G_OBJECT (w->rnm), "clicked",
                    G_CALLBACK (rename_clicked), (gpointer) w);

  hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), w->name, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), w->rnm, FALSE, TRUE, 0);
  
  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), w->file, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);

  gtk_main ();
  return 0;
}

/* When the selected file is changed, check whether the user has write accesss. */
static void 
file_changed (GtkFileChooser *file, 
              Widgets *w)
{
  gchar *fn = gtk_file_chooser_get_filename (file);
  gint mode = g_access (fn, W_OK);
  
  gtk_widget_set_sensitive (w->rnm, !mode);
  gtk_widget_set_sensitive (w->name, !mode);
}

/* Rename the selected file with g_rename(), a function provided by GLib. */
static void 
rename_clicked (GtkButton *rnm, 
                Widgets *w)
{
  gchar *old, *new, *location;
  
  old = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (w->file));
  location = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (w->file));
  new = g_strconcat (location, "/", gtk_entry_get_text (GTK_ENTRY (w->name)), NULL);
  
  g_rename (old, new);
  gtk_widget_set_sensitive (w->rnm, FALSE);
  gtk_widget_set_sensitive (w->name, FALSE);
}
