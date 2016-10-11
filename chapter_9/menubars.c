#include <gtk/gtk.h>

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *menubar, *file, *edit, *help, *filemenu, *editmenu, *helpmenu;
  GtkWidget *new, *open, *cut, *copy, *paste, *contents, *about;
  GtkAccelGroup *group;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Menu Bars");
  gtk_widget_set_size_request (window, 250, -1);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  group = gtk_accel_group_new ();
  menubar = gtk_menu_bar_new ();
  file = gtk_menu_item_new_with_label ("File");
  edit = gtk_menu_item_new_with_label ("Edit");
  help = gtk_menu_item_new_with_label ("Help");
  filemenu = gtk_menu_new ();
  editmenu = gtk_menu_new ();
  helpmenu = gtk_menu_new ();

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file), filemenu); 
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (edit), editmenu); 
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help), helpmenu);
  
  gtk_menu_shell_append (GTK_MENU_SHELL (menubar), file); 
  gtk_menu_shell_append (GTK_MENU_SHELL (menubar), edit); 
  gtk_menu_shell_append (GTK_MENU_SHELL (menubar), help);
  
  /* Create the File menu content. */
  new = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW, group);
  open = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, group);
  gtk_menu_shell_append (GTK_MENU_SHELL (filemenu), new);
  gtk_menu_shell_append (GTK_MENU_SHELL (filemenu), open);
  
  /* Create the Edit menu content. */
  cut = gtk_image_menu_item_new_from_stock (GTK_STOCK_CUT, group);
  copy = gtk_image_menu_item_new_from_stock (GTK_STOCK_COPY, group);
  paste = gtk_image_menu_item_new_from_stock (GTK_STOCK_PASTE, group);
  gtk_menu_shell_append (GTK_MENU_SHELL (editmenu), cut);
  gtk_menu_shell_append (GTK_MENU_SHELL (editmenu), copy);
  gtk_menu_shell_append (GTK_MENU_SHELL (editmenu), paste);
  
  /* Create the Help menu content. */
  contents = gtk_image_menu_item_new_from_stock (GTK_STOCK_HELP, group);
  about = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, group);
  gtk_menu_shell_append (GTK_MENU_SHELL (helpmenu), contents);
  gtk_menu_shell_append (GTK_MENU_SHELL (helpmenu), about);
  
  gtk_container_add (GTK_CONTAINER (window), menubar);
  gtk_window_add_accel_group (GTK_WINDOW (window), group);

  gtk_widget_show_all (window);
  gtk_main ();
  return 0;
}
