#ifndef BROWSER_H
#define BROWSER_H

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib/gstdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

enum 
{ 
  ICON = 0,
  FILENAME,
  SIZE,
  MODIFIED,
  COLUMNS
};

const gchar *size_type[4];
GList *current_path;
GPtrArray *history;
GladeXML *xml;
gint history_pos;
guint context_id;

void file_manager_error (gchar*);
void populate_tree_model (GtkWidget*);
void on_info_clicked (GtkToolButton*);
void store_history ();
void parse_location (GString*);
void populate_tree_model (GtkWidget*);
gchar* path_to_string ();

#endif
