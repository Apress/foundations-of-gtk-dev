#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

enum 
{
  ACTION = 0,
  MASK,
  VALUE,
  COLUMNS
};

typedef struct
{
  gchar *action;
  GdkModifierType mask;
  guint value;
} Accelerator;

const Accelerator list[] =
{ 
  { "Cut", GDK_CONTROL_MASK, GDK_X },
  { "Copy", GDK_CONTROL_MASK, GDK_C },
  { "Paste", GDK_CONTROL_MASK, GDK_V },
  { "New", GDK_CONTROL_MASK, GDK_N },
  { "Open", GDK_CONTROL_MASK, GDK_O },
  { "Print", GDK_CONTROL_MASK, GDK_P },
  { NULL, 0, 0 }
};

static void setup_tree_view (GtkWidget*);
static void accel_edited (GtkCellRendererAccel*, gchar*, guint,
                          GdkModifierType, guint, GtkTreeView*);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *treeview, *scrolled_win;
  GtkListStore *store;
  GtkTreeIter iter;
  guint i = 0;
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Accelerator Keys");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 250, 250);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  treeview = gtk_tree_view_new ();
  setup_tree_view (treeview);

  /* Create a new tree model with three columns, as string, gint and guint. */
  store = gtk_list_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_UINT);
  
  /* Add all of the products to the GtkListStore. */
  while (list[i].action != NULL)
  {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, ACTION, list[i].action, 
                        MASK, (gint) list[i].mask, VALUE, list[i].value, -1);
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

/* Create a tree view with two columns. The first is an action and the
 * second is a keyboard accelerator. */
static void
setup_tree_view (GtkWidget *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
                         ("Buy", renderer, "text", ACTION, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  renderer = gtk_cell_renderer_accel_new ();
  g_object_set (renderer, "accel-mode", GTK_CELL_RENDERER_ACCEL_MODE_GTK,
                "editable", TRUE, NULL);

  column = gtk_tree_view_column_new_with_attributes ("Buy", renderer, 
                                "accel-mods", MASK, "accel-key", VALUE, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  g_signal_connect (G_OBJECT (renderer), "accel_edited",
                    G_CALLBACK (accel_edited),
                    (gpointer) treeview);
}

/* Apply the new keyboard accelerator key and mask to the cell. */
static void
accel_edited (GtkCellRendererAccel *renderer,
              gchar *path,
              guint accel_key,
              GdkModifierType mask,
              guint hardware_keycode,
              GtkTreeView *treeview)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  
  model = gtk_tree_view_get_model (treeview);
  if (gtk_tree_model_get_iter_from_string (model, &iter, path))
    gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                        MASK, (gint) mask, VALUE, accel_key, -1);
}
