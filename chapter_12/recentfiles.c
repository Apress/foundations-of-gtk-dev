#include <gtk/gtk.h>
#include <string.h>

typedef struct
{
  GtkWidget *window;
  GtkWidget *textview;
  GtkToolItem *recent;
} Widgets;

static void open_file (GtkButton*, Widgets*);
static void save_file (GtkButton*, Widgets*);
static void open_recent_file (GtkButton*, Widgets*);
static void menu_activated (GtkMenuShell*, Widgets*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *vbox, *hbox, *open, *save, *swin, *icon, *menu;
  PangoFontDescription *fd;
  GtkRecentManager *manager;
  Widgets *w;

  gtk_init (&argc, &argv);

  w = g_slice_new (Widgets);
  w->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (w->window), "Recent Files");
  gtk_container_set_border_width (GTK_CONTAINER (w->window), 5);
  gtk_widget_set_size_request (w->window, 600, 400);

  g_signal_connect (G_OBJECT (w->window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  w->textview = gtk_text_view_new ();
  fd = pango_font_description_from_string ("Monospace 10");
  gtk_widget_modify_font (w->textview, fd);
  pango_font_description_free (fd);
  
  swin = gtk_scrolled_window_new (NULL, NULL);
  open = gtk_button_new_from_stock (GTK_STOCK_OPEN);
  save = gtk_button_new_from_stock (GTK_STOCK_SAVE);
  icon = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
  w->recent = gtk_menu_tool_button_new (icon, "Recent Files");
  
  /* Load the default recent chooser menu and create a menu from it. */
  manager = gtk_recent_manager_get_default ();
  menu = gtk_recent_chooser_menu_new_for_manager (manager);
  gtk_menu_tool_button_set_menu (GTK_MENU_TOOL_BUTTON (w->recent), menu);
  
  gtk_recent_chooser_set_show_not_found (GTK_RECENT_CHOOSER (menu), FALSE);
  gtk_recent_chooser_set_local_only (GTK_RECENT_CHOOSER (menu), TRUE);
  gtk_recent_chooser_set_limit (GTK_RECENT_CHOOSER (menu), 10);
  gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER (menu),
                                    GTK_RECENT_SORT_MRU);
  
  g_signal_connect (G_OBJECT (menu), "selection-done",
                    G_CALLBACK (menu_activated), (gpointer) w);
  g_signal_connect (G_OBJECT (open), "clicked",
                    G_CALLBACK (open_file), (gpointer) w);
  g_signal_connect (G_OBJECT (save), "clicked",
                    G_CALLBACK (save_file), (gpointer) w);
  g_signal_connect (G_OBJECT (w->recent), "clicked",
                    G_CALLBACK (open_recent_file), (gpointer) w);
  
  hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), open, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), save, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), GTK_WIDGET (w->recent), FALSE, FALSE, 0);
  
  vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (swin), w->textview);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), swin, TRUE, TRUE, 0);

  gtk_container_add (GTK_CONTAINER (w->window), vbox);
  gtk_widget_show_all (w->window);
  
  gtk_main ();
  return 0;
}

/* Save the changes that the user made to the file to disk. */
static void 
save_file (GtkButton *save,
           Widgets *w)
{
  const gchar *filename;
  gchar *content;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  
  filename = gtk_window_get_title (GTK_WINDOW (w->window));
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w->textview));
  gtk_text_buffer_get_bounds (buffer, &start, &end);
  content = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
  
  if (!g_file_set_contents (filename, content, -1, NULL))
    g_warning ("The file '%s' could not be written!", filename);
  g_free (content);

}

/* A menu item was activated. So, retrieve the file URI and open it. */
static void 
menu_activated (GtkMenuShell *menu, 
                Widgets *w)
{
  GtkTextBuffer *buffer;
  gchar *filename, *content, *fn;
  gsize length;

  filename = gtk_recent_chooser_get_current_uri (GTK_RECENT_CHOOSER (menu));
    
  if (filename != NULL)
  {
    /* Remove the "file://" prefix from the beginning of the URI if it exists. */
    fn = g_filename_from_uri (filename, NULL, NULL);
    
    if (g_file_get_contents (fn, &content, &length, NULL))
    {
      gtk_window_set_title (GTK_WINDOW (w->window), fn);
      buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w->textview));
      gtk_text_buffer_set_text (buffer, content, -1);
      g_free (content);
    }
    else
      g_warning ("The file '%s' could not be read!", filename);

    g_free (filename);
    g_free (fn);
  }
}

/* Open a file selected by the user and add it as a new recent file. */
static void
open_file (GtkButton *open,
           Widgets *w)
{
  GtkWidget *dialog;
  GtkRecentManager *manager;
  GtkRecentData *data;
  GtkTextBuffer *buffer;
  gchar *filename, *content, *uri;
  gsize length;
  
  static gchar *groups[2] = {
    "testapp",
    NULL
  };
  
  dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW (w->window),
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_OK,
                                        NULL);
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    
    if (g_file_get_contents (filename, &content, &length, NULL))
    {
      /* Create a new recently used resource. */
      data = g_slice_new (GtkRecentData);
      data->display_name = NULL;
      data->description = NULL;
      data->mime_type = "text/plain";
      data->app_name = (gchar*) g_get_application_name ();
      data->app_exec = g_strjoin (" ", g_get_prgname (), "%u", NULL);
      data->groups = groups;
      data->is_private = FALSE;
      uri = g_filename_to_uri (filename, NULL, NULL);
      
      /* Add the recently used resource to the default recent manager. */
      manager = gtk_recent_manager_get_default ();
      gtk_recent_manager_add_full (manager, uri, data);
      
      /* Load the file and set the filename as the title of the window. */
      gtk_window_set_title (GTK_WINDOW (w->window), filename);
      buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w->textview));
      gtk_text_buffer_set_text (buffer, content, -1);
      
      g_free (content);
      g_free (uri);
      g_free (data->app_exec);
      g_slice_free (GtkRecentData, data);
    }
    else
      g_warning ("The file '%s' could not be read!", filename);
    
    g_free (filename);
  }
  
  gtk_widget_destroy (dialog);
}

/* Allow the user to choose a recent file from the list in the dialog. */
static void 
open_recent_file (GtkButton *recent,
                  Widgets *w)
{
  GtkWidget *dialog;
  GtkRecentManager *manager;
  GtkTextBuffer *buffer;
  GtkRecentFilter *filter;
  gchar *filename, *content, *fn;
  gsize length;
  
  manager = gtk_recent_manager_get_default ();
  dialog = gtk_recent_chooser_dialog_new_for_manager ("Open Recent File",
                                             GTK_WINDOW (w->window), manager,
                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                             GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);
  
  /* Add a filter that will display all of the files in the dialog. */
  filter = gtk_recent_filter_new ();
  gtk_recent_filter_set_name (filter, "All Files");
  gtk_recent_filter_add_pattern (filter, "*");
  gtk_recent_chooser_add_filter (GTK_RECENT_CHOOSER (dialog), filter);
  
  /* Add another filter that will only display plain text files. */
  filter = gtk_recent_filter_new ();
  gtk_recent_filter_set_name (filter, "Plain Text");
  gtk_recent_filter_add_mime_type (filter, "text/plain");
  gtk_recent_chooser_add_filter (GTK_RECENT_CHOOSER (dialog), filter);
  
  gtk_recent_chooser_set_show_not_found (GTK_RECENT_CHOOSER (dialog), FALSE);
  gtk_recent_chooser_set_local_only (GTK_RECENT_CHOOSER (dialog), TRUE);
  gtk_recent_chooser_set_limit (GTK_RECENT_CHOOSER (dialog), 10);
  gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER (dialog),
                                    GTK_RECENT_SORT_MRU);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
  {
    filename = gtk_recent_chooser_get_current_uri (GTK_RECENT_CHOOSER (dialog));
    
    if (filename != NULL)
    {
      /* Remove the "file://" prefix from the beginning of the URI if it exists. */
      fn = g_filename_from_uri (filename, NULL, NULL);
    
      if (g_file_get_contents (fn, &content, &length, NULL))
      {
        gtk_window_set_title (GTK_WINDOW (w->window), fn);
        buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w->textview));
        gtk_text_buffer_set_text (buffer, content, -1);
        g_free (content);
      }
      else
        g_warning ("The file '%s' could not be read!", filename);

      g_free (filename);
      g_free (fn);
    }
  }

  gtk_widget_destroy (dialog);
}
