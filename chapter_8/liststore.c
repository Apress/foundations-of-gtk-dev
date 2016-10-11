#include <gtk/gtk.h>

enum
{ 
  BUY_IT = 0,
  QUANTITY,
  PRODUCT,
  COLUMNS
};

typedef struct
{
  gboolean buy;
  gint quantity;
  gchar *product;
} GroceryItem;

const GroceryItem list[] = 
{ 
  { TRUE, 1, "Paper Towels" }, 
  { TRUE, 2, "Bread" },
  { FALSE, 1, "Butter" },
  { TRUE, 1, "Milk" },
  { FALSE, 3, "Chips" },
  { TRUE, 4, "Soda" }, 
  { FALSE, 0, NULL }
};

static void setup_tree_view (GtkWidget*);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *treeview, *scrolled_win;
  GtkListStore *store;
  GtkTreeIter iter;
  guint i = 0;
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Grocery List");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 250, 175);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  treeview = gtk_tree_view_new ();
  setup_tree_view (treeview);
  
  /* Create a new tree model with three columns, as string, gint and guint. */
  store = gtk_list_store_new (COLUMNS, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_STRING);
  
  /* Add all of the products to the GtkListStore. */
  while (list[i].product != NULL)
  {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, BUY_IT, list[i].buy, 
                        QUANTITY, list[i].quantity, PRODUCT, list[i].product, -1);
    i++;
  }
  
  /* Add the tree model to the tree view and unreference it so that the model will
   * be destroyed along with the tree view. */
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
  g_object_unref (store);
  
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
  gtk_container_add (GTK_CONTAINER (scrolled_win), treeview);
  gtk_container_add (GTK_CONTAINER (window), scrolled_win);
  gtk_widget_show_all (window);
  
  gtk_main ();
  return 0;
}

/* Add three columns to the GtkTreeView. All three of the columns will be
 * displayed as text, although one is a gboolean value and another is
 * an integer. */
static void
setup_tree_view (GtkWidget *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  
  /* Create a new GtkCellRendererText, add it to the tree view column and
   * append the column to the tree view. */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
                         ("Buy", renderer, "text", BUY_IT, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
                         ("Count", renderer, "text", QUANTITY, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
                         ("Product", renderer, "text", PRODUCT, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}
