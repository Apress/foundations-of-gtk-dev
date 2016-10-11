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
static void add_product (GtkButton*, GtkTreeView*);
static void remove_products (GtkButton*, GtkTreeView*);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *treeview, *scrolled_win, *vbox, *hbox, *add, *remove;
  GtkTreeStore *store;
  GtkTreeIter iter, child;
  GtkTreeSelection *selection;
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
  
  /* Allow multiple rows to be selected at the same time. */
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_MULTIPLE);
  
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
  add = gtk_button_new_from_stock (GTK_STOCK_ADD);
  remove = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
  
  g_signal_connect (G_OBJECT (add), "clicked",
                    G_CALLBACK (add_product), (gpointer) treeview);
  g_signal_connect (G_OBJECT (remove), "clicked",
                    G_CALLBACK (remove_products), (gpointer) treeview);
  
  hbox = gtk_hbox_new (TRUE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), add, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), remove, FALSE, TRUE, 0);
  
  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
  
  gtk_container_add (GTK_CONTAINER (scrolled_win), treeview);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);
  
  gtk_main ();
  return 0;
}

static void
add_product (GtkButton *add,
             GtkTreeView *treeview)
{
  GtkWidget *dialog, *table, *combobox, *entry, *spin, *check;
  GtkTreeIter iter, child;
  GtkTreePath *path;
  GtkTreeModel *model;
  const gchar *product;
  gchar *category, *name;
  gint quantity, i = 0;
  gboolean buy;
  
  /* Create a dialog that will be used to create a new product. */
  dialog = gtk_dialog_new_with_buttons ("Add a Product", NULL,
                                        GTK_DIALOG_MODAL,
                                        GTK_STOCK_ADD, GTK_RESPONSE_OK,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        NULL);

  /* Create widgets that will be packed into the dialog. */
  combobox = gtk_combo_box_new_text ();
  entry = gtk_entry_new ();
  spin = gtk_spin_button_new_with_range (0, 100, 1);
  check = gtk_check_button_new_with_mnemonic ("_Buy the Product");
  gtk_spin_button_set_digits (GTK_SPIN_BUTTON (spin), 0);
  
  /* Add all of the categories to the combo box. */
  while (list[i].product != NULL)
  {
    if (list[i].product_type == PRODUCT_CATEGORY)
      gtk_combo_box_append_text (GTK_COMBO_BOX (combobox), list[i].product);
    i++;
  }
  
  table = gtk_table_new (4, 2, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);
  gtk_container_set_border_width (GTK_CONTAINER (table), 5);
  
  /* Pack the table that will hold the dialog widgets. */
  gtk_table_attach (GTK_TABLE (table), gtk_label_new ("Category:"), 0, 1, 0, 1, 
                    GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (table), combobox, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 
                    GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (table), gtk_label_new ("Product:"), 0, 1, 1, 2, 
                    GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, 
                    GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (table), gtk_label_new ("Quantity:"), 0, 1, 2, 3, 
                    GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (table), spin, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, 
                    GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (table), check, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, 
                    GTK_SHRINK | GTK_FILL, 0, 0);
  
  gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), table);
  gtk_widget_show_all (dialog);

  /* If the user presses OK, verify the entries and add the product. */
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
  {
    quantity = (gint) gtk_spin_button_get_value (GTK_SPIN_BUTTON (spin));
    product = gtk_entry_get_text (GTK_ENTRY (entry));
    category = gtk_combo_box_get_active_text (GTK_COMBO_BOX (combobox));
    buy = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check));
    
    if (g_ascii_strcasecmp (product, "") == 0 || category == NULL)
    {
      g_warning ("All of the fields were not correctly filled out!");
      gtk_widget_destroy (dialog);
      
      if (category != NULL)
        g_free (category);
      return;
    }
    
    model = gtk_tree_view_get_model (treeview);
    gtk_tree_model_get_iter_from_string (model, &iter, "0");
    
    /* Retrieve an iterator pointing to the selected category. */
    do
    {
      gtk_tree_model_get (model, &iter, PRODUCT, &name, -1);
      
      if (g_ascii_strcasecmp (name, category) == 0)
      {
        g_free (name);
        break;
      }
      
      g_free (name);
    } while (gtk_tree_model_iter_next (model, &iter));
    
    /* Convert the category iterator to a path so that it will not become invalid
     * and add the new product as a child of the category. */
    path = gtk_tree_model_get_path (model, &iter);
    gtk_tree_store_append (GTK_TREE_STORE (model), &child, &iter);
    gtk_tree_store_set (GTK_TREE_STORE (model), &child, BUY_IT, buy, 
                        QUANTITY, quantity, PRODUCT, product, -1);
    
    /* Add the quantity to the running total if it is to be purchased. */
    if (buy)
    {
      gtk_tree_model_get_iter (model, &iter, path);
      gtk_tree_model_get (model, &iter, QUANTITY, &i, -1);
      i += quantity;
      gtk_tree_store_set (GTK_TREE_STORE (model), &iter, QUANTITY, i, -1);
    }
    
    gtk_tree_path_free (path);
    g_free (category);
  }

  gtk_widget_destroy (dialog);
}

static void
remove_row (GtkTreeRowReference *ref,
            GtkTreeModel *model)
{
  GtkTreeIter parent, iter;
  GtkTreePath *path;
  gboolean buy;
  gint quantity, pnum;
  
  path = gtk_tree_row_reference_get_path (ref);
  gtk_tree_model_get_iter (model, &iter, path);

  /* Only remove the row if it is not a root row. */
  if (gtk_tree_model_iter_parent (model, &parent, &iter))
  {
    gtk_tree_model_get (model, &iter, BUY_IT, &buy, QUANTITY, &quantity, -1);
    gtk_tree_model_get (model, &parent, QUANTITY, &pnum, -1);
    
    if (buy)
    {
      pnum -= quantity;
      gtk_tree_store_set (GTK_TREE_STORE (model), &parent, QUANTITY, pnum, -1);
    }
    
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_store_remove (GTK_TREE_STORE (model), &iter);    
  }
}

static void 
remove_products (GtkButton *remove,
                 GtkTreeView *treeview)
{
  GtkTreeSelection *selection;
  GtkTreeRowReference *ref;
  GtkTreeModel *model;
  GList *rows, *ptr, *references = NULL;
  
  selection = gtk_tree_view_get_selection (treeview);
  model = gtk_tree_view_get_model (treeview);
  rows = gtk_tree_selection_get_selected_rows (selection, &model);
  
  /* Create tree row references to all of the selected rows. */
  ptr = rows;
  while (ptr != NULL)
  {
    ref = gtk_tree_row_reference_new (model, (GtkTreePath*) ptr->data);
    references = g_list_prepend (references, gtk_tree_row_reference_copy (ref));
    gtk_tree_row_reference_free (ref);
    ptr = ptr->next;
  }
  
  /* Remove each of the selected rows pointed to by the row reference. */
  g_list_foreach (references, (GFunc) remove_row, model);

  /* Free the tree paths, tree row references and lists. */
  g_list_foreach (references, (GFunc) gtk_tree_row_reference_free, NULL);
  g_list_foreach (rows, (GFunc) gtk_tree_path_free, NULL);
  g_list_free (references);
  g_list_free (rows);
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
