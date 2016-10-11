#include <gtk/gtk.h>

enum 
{ 
  BUY_IT = 0,
  QUANTITY,
  PRODUCT,
  COLUMNS
};

enum
{
  PRODUCT_CATEGORY,
  PRODUCT_CHILD
};

typedef struct
{
  gint product_type;
  gboolean buy;
  gint quantity;
  gchar *product;
} GroceryItem;

GroceryItem list[] = 
{ 
  { PRODUCT_CATEGORY, TRUE, 0, "Cleaning Supplies" },
  { PRODUCT_CHILD, TRUE, 1, "Paper Towels" }, 
  { PRODUCT_CHILD, TRUE, 3, "Toilet Paper" },
  { PRODUCT_CATEGORY, TRUE, 0, "Food" },
  { PRODUCT_CHILD, TRUE, 2, "Bread" },
  { PRODUCT_CHILD, FALSE, 1, "Butter" },
  { PRODUCT_CHILD, TRUE, 1, "Milk" },
  { PRODUCT_CHILD, FALSE, 3, "Chips" },
  { PRODUCT_CHILD, TRUE, 4, "Soda" }, 
  { PRODUCT_CATEGORY, FALSE, 0, NULL }
};

static void setup_tree_view (GtkWidget*);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *treeview, *scrolled_win;
  GtkTreeStore *store;
  GtkTreeIter iter, child;
  guint i = 0, j;
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Grocery List");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 275, 300);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  treeview = gtk_tree_view_new ();  
  setup_tree_view (treeview);
  
  store = gtk_tree_store_new (COLUMNS, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_STRING);
  
  while (list[i].product != NULL)
  {
    /* If the product type is a category, count the quantity of all of the products
     * in the category that are going to be boughty. */
    if (list[i].product_type == PRODUCT_CATEGORY)
    {
      j = i + 1;
      
      /* Calculate how many products will be bought in the category. */
      while (list[j].product != NULL && list[j].product_type != PRODUCT_CATEGORY)
      {
        if (list[j].buy)
          list[i].quantity += list[j].quantity;
        j++;
      }
      
      /* Add the category as a new root element. */
      gtk_tree_store_append (store, &iter, NULL);
      gtk_tree_store_set (store, &iter, BUY_IT, list[i].buy, 
                          QUANTITY, list[i].quantity, PRODUCT, list[i].product, -1);
    }
    /* Otherwise, add the product as a child of the category. */
    else
    {
      gtk_tree_store_append (store, &child, &iter);
      gtk_tree_store_set (store, &child, BUY_IT, list[i].buy, 
                          QUANTITY, list[i].quantity, PRODUCT, list[i].product, -1);
    }
    
    i++;
  }
  
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
  gtk_tree_view_expand_all (GTK_TREE_VIEW (treeview));
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
