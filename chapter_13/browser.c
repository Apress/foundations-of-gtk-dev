#include "navigation.h"

void on_delete_clicked (GtkToolButton*);
static void setup_tree_view (GtkWidget*);
static void setup_tree_model (GtkWidget*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *treeview, *statusbar;

  gtk_init (&argc, &argv);

  current_path = NULL;
  history_pos = 0;

  xml = glade_xml_new ("browser.glade", NULL, NULL);
  window = glade_xml_get_widget (xml, "window");
  treeview = glade_xml_get_widget (xml, "treeview");
  statusbar = glade_xml_get_widget (xml, "statusbar");
  
  size_type[0] = "B";
  size_type[1] = "KiB";
  size_type[2] = "MiB";
  size_type[3] = "GiB";

  history = g_ptr_array_new ();
  g_ptr_array_add (history, (gpointer) g_strdup (G_DIR_SEPARATOR_S));
  context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar), "Message");
  
  glade_xml_signal_autoconnect (xml);
  setup_tree_view (treeview);
  setup_tree_model (treeview);

  gtk_widget_show_all (window);
  gtk_main ();
  
  return 0;
}

/* Display an error message to the user using GtkMessageDialog. */
void
file_manager_error (gchar *message)
{
  GtkWidget *dialog, *window;
  
  window = glade_xml_get_widget (xml, "window");
  dialog = gtk_message_dialog_new (GTK_WINDOW (window), GTK_DIALOG_MODAL, 
                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                   "File Manager Error");
  gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), message);
  
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

/* Setup the columns in the tree view. These include a column for the GdkPixbuf,
 * file name, file size, and last modification time/date. */
static void
setup_tree_view (GtkWidget *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  
  /* Create a tree view column with an icon and file name. */
  column = gtk_tree_view_column_new ();
  gtk_tree_view_column_set_title (column, "File Browser");
  
  renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_column_pack_start (column, renderer, FALSE);
  gtk_tree_view_column_set_attributes (column, renderer, "pixbuf", ICON, NULL);
  
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, renderer, TRUE);
  gtk_tree_view_column_set_attributes (column, renderer, "text", FILENAME, NULL);
  
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  /* Insert a second tree view column that displays the file size. */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
                         ("Size", renderer, "text", SIZE, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  /* Insert a third tree view column that displays the last modified time/date. */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
                         ("Last Modified", renderer, "text", MODIFIED, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}

/* Convert the GList path into a character string. */
gchar*
path_to_string ()
{
  gchar *location;
  GList *temp;

  /* If there is no content, use the root directory. */
  if (g_list_length (current_path) == 0)
    location = G_DIR_SEPARATOR_S;
  /* Otherwise, build the path out of the GList content. */
  else
  {  
    temp = current_path;
    location = g_strconcat (G_DIR_SEPARATOR_S, (gchar*) temp->data, NULL);
    temp = temp->next;
  
    while (temp != NULL)
    {
      location = g_strconcat (location, G_DIR_SEPARATOR_S, (gchar*) temp->data, NULL);
      temp = temp->next;
    }
  }
  
  return location;
}

/* Setup the content of the tree model. */
void
populate_tree_model (GtkWidget *treeview)
{
  GtkWidget *entry, *statusbar;
  GdkPixbuf *pixbuf_file, *pixbuf_dir;
  GtkListStore *store;
  GtkTreeIter iter;
  GdkPixbuf *directory;
  struct stat st;
  gchar *location, *file, *message;
  gfloat size, total_size = 0;
  gint i, items = 0;;
  GDir *dir;
  
  store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
  gtk_list_store_clear (store);
  location = path_to_string ();
  
  /* If the current location is not the root directory, add the '..' entry. */
  if (g_list_length (current_path) > 0)
  {
    directory = gdk_pixbuf_new_from_file ("directory.png", NULL);
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, ICON, directory, FILENAME, "..", -1);
  }
  
  /* Return if the path does not exist. */
  if (!g_file_test (location, G_FILE_TEST_IS_DIR))
  {
    file_manager_error ("The path %s does not exist!");
    g_free (location);
    return;
  }
  
  /* Display the new location in the address bar. */
  entry = glade_xml_get_widget (xml, "location");
  gtk_entry_set_text (GTK_ENTRY (entry), location);
  
  /* Add each file to the list along with the file size and modified date. */
  pixbuf_dir = gdk_pixbuf_new_from_file ("directory.png", NULL);
  pixbuf_file = gdk_pixbuf_new_from_file ("file.png", NULL);
  dir = g_dir_open (location, 0, NULL);
  while ((file = (gchar*) g_dir_read_name (dir)))
  {
    gchar *fn, *filesize, *modified;
    
    fn = g_strconcat (location, "/", file, NULL);
    if (g_stat (fn, &st) == 0)
    {
      /* Calculate the file size and order of magnitude. */
      i = 0;
      size = (gfloat) st.st_size;
      total_size += size;
      while (size >= 1024.0)
      {
        size = size / 1024.0;
        i++;
      }
      
      /* Create strings for the file size and last modified date. */
      filesize = g_strdup_printf ("%.1f %s", size, size_type[i]);
      modified = g_strdup (ctime (&st.st_mtime));
      modified[strlen(modified)-1] = '\0';
    }
    
    /* Add the file and its properties as a new tree view row. */
    gtk_list_store_append (store, &iter);
    
    if (g_file_test (fn, G_FILE_TEST_IS_DIR))
      gtk_list_store_set (store, &iter, ICON, pixbuf_dir, FILENAME, file,
                          SIZE, filesize, MODIFIED, modified, -1);
    else
      gtk_list_store_set (store, &iter, ICON, pixbuf_file, FILENAME, file,
                          SIZE, filesize, MODIFIED, modified, -1);
    items++;

    g_free (fn);
  }
  
  /* Calculate the total size of the directory content. */
  i = 0;
  while (total_size >= 1024.0)
  {
    total_size = total_size / 1024.0;
    i++;
  }
  
  /* Add the number of items and the total size of the directory content
   * to the status bar. */
  statusbar = glade_xml_get_widget (xml, "statusbar");
  message = g_strdup_printf ("%d items, Total Size: %.1f %s", items,
                             total_size, size_type[i]);
  gtk_statusbar_pop (GTK_STATUSBAR (statusbar), context_id);
  gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id, message);
  g_object_unref (pixbuf_dir);
  g_object_unref (pixbuf_file);
  g_free (message);
}

/* Add the tree store, but the actual content is setup in populate_tree_model(). */
static void
setup_tree_model (GtkWidget *treeview)
{
  GtkListStore *store;
  
  store = gtk_list_store_new (COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING,
                              G_TYPE_STRING, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
  g_object_unref (store);
  
  populate_tree_model (treeview);
}

/* Store the current location as the history stack. */
void
store_history ()
{
  gchar *previous, *current;
  GList *temp;
  gint i;

  /* Build a string that contains the current path. */
  previous = G_DIR_SEPARATOR_S;
  temp = current_path;
  while (temp != NULL)
  {
    previous = g_strconcat (previous, (gchar*) temp->data, "/", NULL);
    temp = temp->next;
  }
  
  /* Remove all strings that occur after history_pos and append the new path. */
  if (strlen (previous) > 1)
    previous[strlen(previous)-1] = '\0';
  current = (gchar*) g_ptr_array_index (history, history_pos);
  if (g_ascii_strcasecmp (previous, current) != 0)
  {
    if (history_pos + 1 != history->len)
    {
      for (i = history_pos + 1; i <= (history->len - history_pos - 1); i++) 
        g_free (g_ptr_array_index (history, i));
      g_ptr_array_remove_range (history, history_pos + 1, 
                                history->len - history_pos - 1);
    }
    
    g_ptr_array_add (history, (gpointer) g_strdup (previous));
    history_pos++;
  }
}

/* Convert the GString into a list of path elements stored in current_path. */
void
parse_location (GString *location)
{
  gchar *find;
  gsize len;

  /* Free the current content stored in current_path. */
  g_list_foreach (current_path, (GFunc) g_free, NULL);
  g_list_free (current_path);
  current_path = NULL;
  
  /* Erase all forward slashes from the end of the string. */
  while (location->str[location->len - 1] == '/')
    g_string_erase (location, location->len - 1, 1);
  
  /* Parse through the string, splitting it into a GList at the '/' character. */
  while (location->str[0] == '/')
  {
    while (location->str[0] == '/')
      g_string_erase (location, 0, 1);
    find = g_strstr_len (location->str, location->len, "/");
    
    if (find == NULL)
      current_path = g_list_append (current_path, g_strdup (location->str));
    else
    {
      len = (gsize) (strlen (location->str) - strlen (find));
      current_path = g_list_append (current_path, g_strndup (location->str, len));
      g_string_erase (location, 0, len);
    }
  }
}

/* Delete the currently selected file or folder. Folders will only be removed if
 * it does not include any content. */
void 
on_delete_clicked (GtkToolButton *item)
{
  GtkWidget *entry, *treeview, *dialog, *window;
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  GString *location;
  GList *temp;
  gchar *file;
  
  entry = glade_xml_get_widget (xml, "location");
  treeview = glade_xml_get_widget (xml, "treeview");
  window = glade_xml_get_widget (xml, "window");
  location = g_string_new ("/");
  temp = current_path;
  
  /* Create the current path out of the content of current_path. */
  while (temp != NULL)
  {
    g_string_append_printf (location, "%s/", (gchar*) temp->data);
    temp = temp->next;
  }

  /* If there is a selected file, delete it if the user approves. */
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    gtk_tree_model_get (model, &iter, FILENAME, &file, -1);
    if (g_ascii_strcasecmp (file, "..") == 0)
    {
      file_manager_error ("You cannot remove the '..' directory!");
      return;
    }

    gtk_entry_set_text (GTK_ENTRY (entry), location->str);
    g_string_append (location, file);
    
    /* Ask the user if it is okay to remove the file or folder. */
    dialog = gtk_message_dialog_new (GTK_WINDOW (window), GTK_DIALOG_MODAL, 
                                     GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                                     "Do you really want to delete %s?", file);
    
    /* If the user approves, remove the file or report if it can't be done. */
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES)
      if (g_remove (location->str) != 0)
        file_manager_error ("The file or folder could not be removed!");
    gtk_widget_destroy (dialog);
    populate_tree_model (NULL);
  }
  
  g_string_free (location, TRUE);
}

/* Display information about the currently selected file or folder. */
void
on_info_clicked (GtkToolButton *item)
{
  GtkWidget *dialog, *name, *loc, *type, *size, *mod, *access, *treeview;
  gchar *file, *location, *modified, *accessed;
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  struct stat st;
  gfloat file_size;
  gint i;

  treeview = glade_xml_get_widget (xml, "treeview");
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
  
  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    dialog = glade_xml_get_widget (xml, "dialog");
    name = glade_xml_get_widget (xml, "name");
    loc = glade_xml_get_widget (xml, "filelocation");
    type = glade_xml_get_widget (xml, "type");
    size = glade_xml_get_widget (xml, "size");
    mod = glade_xml_get_widget (xml, "modified");
    access = glade_xml_get_widget (xml, "accessed");
    gtk_tree_model_get (model, &iter, FILENAME, &file, -1);
    
    /* Set the file name, location and whether it is a file or directory. */
    location = path_to_string ();
    gtk_label_set_text (GTK_LABEL (name), file);
    gtk_label_set_text (GTK_LABEL (loc), location);
    file = g_strconcat (location, "/", file, NULL);
    if (g_file_test (file, G_FILE_TEST_IS_DIR))
      gtk_label_set_text (GTK_LABEL (type), "Directory");
    else
      gtk_label_set_text (GTK_LABEL (type), "File");
    
    /* Set the file size, last modified date and last accessed date. */
    if (g_stat (file, &st) == 0)
    {
      i = 0;
      file_size = (gfloat) st.st_size;
      while (file_size >= 1024.0)
      {
        file_size = file_size / 1024.0;
        i++;
      }
    
      modified = g_strndup (ctime (&st.st_mtime), strlen (ctime (&st.st_mtime))-1);
      accessed = g_strndup (ctime (&st.st_atime), strlen (ctime (&st.st_atime))-1);
      gtk_label_set_text (GTK_LABEL (mod), modified);
      gtk_label_set_text (GTK_LABEL (access), accessed);
      gtk_label_set_text (GTK_LABEL (size), 
                          g_strdup_printf ("%.1f %s", file_size, size_type[i]));
    }
    
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_hide (dialog);
  }
}
