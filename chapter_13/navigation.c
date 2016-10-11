#include "navigation.h"

/* When a row is activated, either move to the desired location or show
 * more information if the file is not a directory. */
void 
on_row_activated (GtkTreeView *treeview,
                  GtkTreePath *path,
                  GtkTreeViewColumn *column) 
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *file;

  model = gtk_tree_view_get_model (treeview);
  if (gtk_tree_model_get_iter (model, &iter, path))
  {
    gtk_tree_model_get (model, &iter, FILENAME, &file, -1);
    
    /* Move to the parent directory. */
    if (g_ascii_strcasecmp ("..", file) == 0)
    {
      GList *last = g_list_last (current_path);
      store_history ();
      current_path = g_list_remove_link (current_path, last);
      g_list_free_1 (last);
      populate_tree_model (GTK_WIDGET (treeview));
    }
    /* Move to the chosen directory or show more information about the file. */
    else
    {
      gchar *location = path_to_string ();   

      if (g_file_test (g_strconcat (location, "/", file, NULL), G_FILE_TEST_IS_DIR))
      {
        store_history ();
        current_path = g_list_append (current_path, file);
        populate_tree_model (GTK_WIDGET (treeview));
      }
      else
        on_info_clicked (NULL);  
    }
  }
}

/* Go to the address bar location when the button is clicked. */
void 
on_go_clicked (GtkButton *button) 
{
  GtkWidget *entry, *treeview;
  GString *location;
  
  entry = glade_xml_get_widget (xml, "location");
  treeview = glade_xml_get_widget (xml, "treeview");
  location = g_string_new (gtk_entry_get_text (GTK_ENTRY (entry)));
  
  /* If the directory exists, visit the entered location. */
  if (g_file_test (location->str, G_FILE_TEST_IS_DIR))
  {
    store_history ();
    parse_location (location);
    populate_tree_model (GTK_WIDGET (treeview));
  }
  else
    file_manager_error ("The location does not exist!");
  
  g_string_free (location, TRUE);
}

/* Visit the previous location in the history list. */
void 
on_back_clicked (GtkToolButton *item)
{
  GtkWidget *treeview;

  if (history_pos >= 0)
  {
    /* Store the current location at the end of the list. */
    if (history_pos + 1 == history->len)
      store_history();

    treeview = glade_xml_get_widget (xml, "treeview");
    parse_location (g_string_new ((gchar*) history->pdata[history_pos]));
    populate_tree_model (GTK_WIDGET (treeview));

    if (history_pos > 0)
      history_pos--;
  }
}

/* Visit the next location in the history list. */
void
on_forward_clicked (GtkToolButton *item)
{
  GtkWidget *treeview;
  GString *str;

  if (history_pos + 1 < history->len)
  {
    history_pos++;
    treeview = glade_xml_get_widget (xml, "treeview");
    str = g_string_new ((gchar*) g_ptr_array_index (history, history_pos));
    parse_location (str);
    populate_tree_model (GTK_WIDGET (treeview));
  }
}

/* Go to the address bar location when the GtkEntry is activated. */
void
on_location_activate (GtkEntry *entry) 
{
  on_go_clicked (NULL);
}

/* Visit the parent directory of the current location. */
void 
on_up_clicked (GtkToolButton *item)
{
  GtkWidget *entry;
  GString *location;
  GList *temp;
  
  entry = glade_xml_get_widget (xml, "location");  
  location = g_string_new ("/");
  temp = current_path;

  while (temp->next != NULL)
  {
    g_string_append_printf (location, "%s/", (gchar*) temp->data);
    temp = temp->next;
  }

  gtk_entry_set_text (GTK_ENTRY (entry), location->str);
  g_string_free (location, TRUE);
  on_go_clicked (NULL);
}

/* Visit the current user's home directory. */
void 
on_home_clicked (GtkToolButton *item) 
{
  GtkWidget *entry;
  
  entry = glade_xml_get_widget (xml, "location");
  gtk_entry_set_text (GTK_ENTRY (entry), g_get_home_dir ());
  on_go_clicked (NULL);
}

/* Refresh the content of the current location. */
void 
on_refresh_clicked (GtkToolButton *item)
{
  GtkWidget *treeview = glade_xml_get_widget (xml, "treeview");
  populate_tree_model (treeview);
}
