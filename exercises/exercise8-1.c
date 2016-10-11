#include <gtk/gtk.h>

enum 
{ 
  ICON = 0,
  FILENAME,
  COLUMNS
};

GList *current_path;

static void setup_tree_view (GtkWidget*);
static void setup_tree_model (GtkWidget*);
static void populate_tree_model (GtkWidget*);
static void row_activated (GtkTreeView*, GtkTreePath*, 
                            GtkTreeViewColumn*, gpointer);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *treeview, *scrolled_win;
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Exercise 8-1");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 250, 500);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  treeview = gtk_tree_view_new ();
  
  g_signal_connect (G_OBJECT (treeview), "row-activated",
                    G_CALLBACK (row_activated), NULL);
  
  setup_tree_view (treeview);
  setup_tree_model (treeview);
  
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
  gtk_container_add (GTK_CONTAINER (scrolled_win), treeview);
  gtk_container_add (GTK_CONTAINER (window), scrolled_win);
  gtk_widget_show_all (window);
  
  gtk_main ();
  return 0;
}

/* Create a tree view with only one column header, but two columns. */
static void
setup_tree_view (GtkWidget *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  
  column = gtk_tree_view_column_new ();
  gtk_tree_view_column_set_title (column, "File Browser");
  
  renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_column_pack_start (column, renderer, FALSE);
  gtk_tree_view_column_set_attributes (column, renderer, "pixbuf", ICON, NULL);
  
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, renderer, TRUE);
  gtk_tree_view_column_set_attributes (column, renderer, "text", FILENAME, NULL);
  
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}

/* Setup the content of the tree model. */
static void
populate_tree_model (GtkWidget *treeview)
{
  GtkListStore *store;
  GtkTreeIter iter;
  GdkPixbuf *directory;
  gchar *location, *file;
  GList *temp;
  GDir *dir;
  
  store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
  gtk_list_store_clear (store);
  
  /* Build the tree path out of current_path. */
  if (current_path == NULL)
    location = g_strdup ("/");
  else
  {
    temp = current_path;
    location = g_strconcat ("/", (gchar*) temp->data, NULL);
    temp = temp->next;

    while (temp != NULL)
    {
      location = g_strconcat (location, "/", (gchar*) temp->data, NULL);
      temp = temp->next;
    }
    
    directory = gdk_pixbuf_new_from_file ("directory.png", NULL);
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, ICON, directory, FILENAME, "..", -1);
  }

  /* Parse through the directory, adding all of its contents to the model. */
  dir = g_dir_open (location, 0, NULL);
  while ((file = (gchar*) g_dir_read_name (dir)))
  {
    GdkPixbuf *pixbuf;
    gchar *temp = g_strconcat (location, "/", file, NULL);
    
    if (g_file_test (temp, G_FILE_TEST_IS_DIR))
      pixbuf = gdk_pixbuf_new_from_file ("directory.png", NULL);
    else
      pixbuf = gdk_pixbuf_new_from_file ("file.png", NULL);
    
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, ICON, pixbuf, FILENAME, file, -1);
    
    g_object_unref (pixbuf);
    g_free (temp);
  }
  
  g_free (location);
}

/* Add the tree store, but the actual content is setup in populate_tree_model(). */
static void
setup_tree_model (GtkWidget *treeview)
{
  GtkListStore *store;
  
  store = gtk_list_store_new (COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
  g_object_unref (store);
  
  populate_tree_model (treeview);
}

static void
row_activated (GtkTreeView *treeview,
               GtkTreePath *path,
               GtkTreeViewColumn *column,
               gpointer data)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *file;

  model = gtk_tree_view_get_model (treeview);
  if (gtk_tree_model_get_iter (model, &iter, path))
  {
    gtk_tree_model_get (model, &iter, FILENAME, &file, -1);
    
    if (g_ascii_strcasecmp ("..", file) == 0)
    {
      GList *last = g_list_last (current_path);
      current_path = g_list_remove_link (current_path, last);
      g_list_free_1 (last);
      populate_tree_model (GTK_WIDGET (treeview));
    }
    else
    {
      gchar *location;
      GList *temp;

      if (current_path == NULL)
        location = "/";
      else
      {
        temp = current_path;
        location = g_strconcat ("/", (gchar*) temp->data, NULL);
        temp = temp->next;
      
        while (temp != NULL)
        {
          location = g_strconcat (location, "/", (gchar*) temp->data, NULL);
          temp = temp->next;
        }
        
        location = g_strconcat (location, "/", NULL);
      }

      location = g_strconcat (location, file, NULL);
      if (g_file_test (location, G_FILE_TEST_IS_DIR))
      {
        current_path = g_list_append (current_path, (gpointer) file);
        populate_tree_model (GTK_WIDGET (treeview));
      }    
      
      g_free (location);    
    }
    
    g_free (file);
  }
}
