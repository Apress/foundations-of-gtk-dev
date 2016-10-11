#include <gtk/gtk.h>
#include <glade/glade.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* A structure used to hold a single event. */
typedef struct
{
  gchar *name;
  gchar *location;
  gchar *start_time;
  gchar *end_time;
} Event;

enum
{
  NAME = 0,
  LOCATION,
  TIME,
  COLUMNS
};

static void setup_tree_view (GtkWidget*);

GladeXML *xml = NULL;
GNode *cal = NULL;
gchar *parsed[7] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *treeview;

  gtk_init (&argc, &argv);

  xml = glade_xml_new ("calendar.glade", NULL, NULL);
  window = glade_xml_get_widget (xml, "window");
  treeview = glade_xml_get_widget (xml, "treeview");
  
  setup_tree_view (treeview);
  glade_xml_signal_autoconnect (xml);
  gtk_widget_show_all (window);
  gtk_main ();
  
  return 0;
}

/* Populate the GtkTreeView with three columns to display the name of the event,
 * its location, and its start and end times. */
static void
setup_tree_view (GtkWidget *treeview)
{
  GtkCellRenderer *rend;
  GtkTreeViewColumn *column;
  
  rend = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Event Name", rend, 
                                                     "text", NAME, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  rend = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Location", rend, 
                                                     "text", LOCATION, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  rend = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Event Time", rend, "text", 
                                                     TIME, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}

/* Loop through all of the nodes of the given base, returning the correct node. */
static GNode*
add_or_retrieve_element (GNode *base,
                         guint num,
                         gboolean ret_null)
{
  GNode *node = NULL, *temp = NULL;
  
  /* If there are no children, either return NULL or create a new child. */
  if (base->children == NULL)
  {
    if (ret_null)
      return NULL;
    
    node = g_node_new (GINT_TO_POINTER (num));
    return g_node_prepend (base, node);
  }
  
  /* Loop through all of the children, returning the match if found. */
  for (temp = base->children; temp != NULL; temp = temp->next)
    if (GPOINTER_TO_INT (temp->data) == num)
      return temp;
  
  /* Return NULL if no child should be created if there is no match. */
  if (ret_null)
    return NULL;
  
  /* Otherwise, create a new child and return it. */
  node = g_node_new (GINT_TO_POINTER (num));
  return g_node_prepend (base, node);
}

/* Display all of the events for the selected day in the GtkTreeView. */
static void
populate_current_date ()
{
  GNode *node_year = NULL, *node_month = NULL, *node_day = NULL, *event = NULL;
  GtkWidget *treeview, *calendar;
  guint day, month, year;
  GtkListStore *store;
  GtkTreeIter iter;
  
  treeview = glade_xml_get_widget (xml, "treeview");
  calendar = glade_xml_get_widget (xml, "calendar");
  
  store = gtk_list_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  gtk_calendar_get_date (GTK_CALENDAR (calendar), &year, &month, &day);

  /* Find a node corresponding to the selected year. */
  node_year = add_or_retrieve_element (cal, year, TRUE);
  
  /* Find the month and day nodes if the previous is not NULL. */
  if (node_year != NULL)
  {
    node_month = add_or_retrieve_element (node_year, month, TRUE);
    if (node_month != NULL)
    {
      node_day = add_or_retrieve_element (node_month, day, TRUE);
    }
  }

  /* If the day was found, loop add all of its events to the GtkTreeView. */
  if (node_day != NULL)
  {
    for (event = node_day->children; event != NULL; event = event->next)
    {
      gchar *time, *start, *end;
     
      /* Retrieve the start and end times of the current event. */
      start = (gchar*) ((Event*) event->data)->start_time;
      end = (gchar*) ((Event*) event->data)->end_time;
      
      /* If the start time is "All Day", do no display an end time. */
      if (g_ascii_strcasecmp (start, "All Day") == 0)
        time = g_strdup ("All Day");
      else
        time = g_strconcat (start, " to ", end, NULL);
    
      /* Add the event as a new row in the GtkTreeView. */
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, NAME, (gchar*) ((Event*) event->data)->name,
                          LOCATION, (gchar*) ((Event*) event->data)->location, 
                          TIME, time, -1);
      g_free (time);
    }
  }
  
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
  g_object_unref (store);
}

/* Enable the Add, Remove and Clear menu items when a calendar is open. */
static void
enable_menu_items ()
{
  gtk_widget_set_sensitive (glade_xml_get_widget (xml, "add"), TRUE);
  gtk_widget_set_sensitive (glade_xml_get_widget (xml, "remove"), TRUE);
  gtk_widget_set_sensitive (glade_xml_get_widget (xml, "clear"), TRUE);
}

/* Create a new event based upon information entered by the user. */
static Event*
get_event_information ()
{
  GtkWidget *dialog, *name, *location, *start, *end;
  Event *event = NULL;
  
  dialog = glade_xml_get_widget (xml, "dialog");
  name = glade_xml_get_widget (xml, "event_name");
  location = glade_xml_get_widget (xml, "location");
  start = glade_xml_get_widget (xml, "start_time");
  end = glade_xml_get_widget (xml, "end_time");
  
  /* Reset the event name and location since the dialog is reused. */
  gtk_entry_set_text (GTK_ENTRY (name), "");
  gtk_entry_set_text (GTK_ENTRY (location), "");
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_APPLY)
  {
    /* Create a new event with the entered name, location and start time. */
    event = g_slice_new (Event);
    event->name = g_strdup (gtk_entry_get_text (GTK_ENTRY (name)));
    event->location = g_strdup (gtk_entry_get_text (GTK_ENTRY (location)));
    event->start_time = gtk_combo_box_get_active_text (GTK_COMBO_BOX (start));
    
    /* If the start time is "All Day", make the end time an empty string. */
    if (g_ascii_strcasecmp (event->start_time, "All Day") == 0)
      event->end_time = g_strdup ("");
    else
      event->end_time = gtk_combo_box_get_active_text (GTK_COMBO_BOX (end));
  }
  
  gtk_widget_hide (dialog);
  return event;
}

/* Called for each event in the tree, this creates an XML element for each event. */
static gboolean
build_xml (GNode *node,
           gpointer data)
{
  GString *content = (GString*) data;
  
  content = g_string_append (content, "  <event>\n");
  g_string_append_printf (content, "    <name>%s</name>\n",
                          (gchar*) ((Event*) node->data)->name);
  g_string_append_printf (content, "    <location>%s</location>\n",
                          (gchar*) ((Event*) node->data)->location);
  g_string_append_printf (content, "    <day>%d</day>\n",
                          GPOINTER_TO_INT (node->parent->data));
  g_string_append_printf (content, "    <month>%d</month>\n",
                          GPOINTER_TO_INT (node->parent->parent->data));
  g_string_append_printf (content, "    <year>%d</year>\n",
                          GPOINTER_TO_INT (node->parent->parent->parent->data));
  g_string_append_printf (content, "    <start>%s</start>\n",
                          (gchar*) ((Event*) node->data)->start_time);
  g_string_append_printf (content, "    <end>%s</end>\n",
                          (gchar*) ((Event*) node->data)->end_time);
  content = g_string_append (content, "  </event>\n");

  return FALSE;
}

/* Save the current calendar because some type of event has occurred. */
static void
save_calendar ()
{
  GString *content;
  
  if (cal->children == NULL)
    content = g_string_new ("<calendar>\n</calendar>");
  else
  {
    content = g_string_new ("<calendar>\n");
    g_node_traverse (cal, G_IN_ORDER, G_TRAVERSE_LEAVES, -1, build_xml, content);
    content = g_string_append (content, "</calendar>");
  }
  
  if (!g_file_set_contents ((gchar*) cal->data, content->str, -1, NULL))
    g_warning ("File could not be written!");
}

/* Handle beginning tags, although only the <event> tag will be handled. */
static void 
xml_start (GMarkupParseContext *context,
           const gchar *element_name,
           const gchar **attribute_names,
           const gchar **attribute_values,
           gpointer data,
           GError **error)
{
  const gchar *element;
  guint i;

  /* If the tag is <event> free the data to make room for a new event. */
  element = g_markup_parse_context_get_element (context);
  if (g_ascii_strcasecmp (element, "event") == 0)
  {
    for (i = 0; i < 7; i++)
    {
      g_free (parsed[i]);
      parsed[i] = NULL;
    }
  }
}

/* When an </event> element is reached, add the event data. */
static void 
xml_end (GMarkupParseContext *context,
         const gchar *element_name,
         gpointer data,
         GError **error)
{
  GNode *node_year, *node_month, *node_day, *node_event;
  guint day, month, year, cur_day, cur_month, cur_year;
  const gchar *element;
  GtkWidget *calendar;
  Event *event;

  /* Return if the element is not </event>. */
  element = g_markup_parse_context_get_element (context);
  if (g_ascii_strcasecmp (element, "event") != 0)
    return;
  
  /* Convert the day, month and year into integers. */
  day = atoi (parsed[2]);
  month = atoi (parsed[3]);
  year = atoi (parsed[4]);
  
  /* Retrieve or create new nodes for the year, month and day. */
  node_year = add_or_retrieve_element (cal, year, FALSE);
  node_month = add_or_retrieve_element (node_year, month, FALSE);
  node_day = add_or_retrieve_element (node_month, day, FALSE);
  
  /* Create a new event out of the data read from the file. */
  event = g_slice_new (Event);
  event->name = g_strdup (parsed[0]);
  event->location = g_strdup (parsed[1]);
  event->start_time = g_strdup (parsed[5]);
  event->end_time = g_strdup (parsed[6]);
  
  /* Create a new event node and mark the day on the calendar if necessary. */
  calendar = glade_xml_get_widget (xml, "calendar");
  gtk_calendar_get_date (GTK_CALENDAR (calendar), &cur_year, &cur_month, &cur_day);
  node_event = g_node_new ((gpointer) event);
  node_event = g_node_prepend (node_day, node_event);
  
  /* Mark the day if it is in the currently displayed month. */
  if (year == cur_year && month == cur_month)
    gtk_calendar_mark_day (GTK_CALENDAR (calendar), day);
}

/* Handle data for the current element in the parser. */
static void
xml_element (GMarkupParseContext *context,
             const gchar *text,
             gsize text_len,
             gpointer data,
             GError **error)
{
  const gchar *element;
  gchar *copy;

  element = g_markup_parse_context_get_element (context);
  copy = g_strdup (text);
  
  /* Store data for the event so that an Event* can later be created. */
  if (element[0] == 'n') parsed[0] = copy;
  else if (element[0] == 'l') parsed[1] = copy;
  else if (element[0] == 'd') parsed[2] = copy;
  else if (element[0] == 'm') parsed[3] = copy;
  else if (element[0] == 'y') parsed[4] = copy;
  else if (element[0] == 's') parsed[5] = copy;
  else if (element[0] == 'e' && element[1] == 'n') parsed[6] = copy;
}

/* Handle errors that occur in parsing the XML file. */
static void
xml_err (GMarkupParseContext *context,
         GError *error,
         gpointer data)
{
  g_critical ("%s", error->message);
}

/* Parse the selected calendar, returning FALSE if it cannot be opened. */
static gboolean
open_calendar (gchar *filename)
{
  static GMarkupParser parser = { xml_start, xml_end, xml_element, NULL, xml_err };
  GtkWidget *treeview, *calendar;
  GMarkupParseContext *context;
  GtkListStore *store;
  gchar *content;
  
  if (!g_file_get_contents (filename, &content, NULL, NULL))
    return FALSE;
  
  treeview = glade_xml_get_widget (xml, "treeview");
  calendar = glade_xml_get_widget (xml, "calendar");
  
  /* Destroy the previous calendar to make room for the new calendar. */
  if (cal != NULL)
  {
    store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
    gtk_list_store_clear (GTK_LIST_STORE (store));
    gtk_calendar_clear_marks (GTK_CALENDAR (calendar));
    g_node_destroy (cal);
    cal = NULL;
  }
  
  /* Create the new calendar and parse the XML file. */
  cal = g_node_new ((gpointer) g_strdup (filename));
  context = g_markup_parse_context_new (&parser, 0, NULL, NULL);
  g_markup_parse_context_parse (context, content, -1, NULL);
  
  g_markup_parse_context_free (context);
  g_free (content);
  
  return TRUE;
}

/* Create a new calendar, closing the current calendar. */
void 
on_new_clicked (GtkToolButton *button)
{
  GtkWidget *dialog, *parent, *name, *calendar, *treeview;
  GtkTreeModel *store;
  gchar *filename, *temp;
  
  name = glade_xml_get_widget (xml, "name");
  parent = glade_xml_get_widget (xml, "window");
  calendar = glade_xml_get_widget (xml, "calendar");
  treeview = glade_xml_get_widget (xml, "treeview");
  
  /* Create a new GtkFileChooserDialog to allow the user to choose a location. */
  dialog = gtk_file_chooser_dialog_new ("Create a New Calendar", 
                                        GTK_WINDOW (parent),
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                        NULL);
  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

  gint result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_ACCEPT)
  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    
    /* Close the previously opened calendar. (It was saved during runtime.) */
    if (cal != NULL)
    {
      store = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
      gtk_list_store_clear (GTK_LIST_STORE (store));
      gtk_calendar_clear_marks (GTK_CALENDAR (calendar));
      g_node_destroy (cal);
      cal = NULL;
    }
    
    /* Setup a new calendar with no child nodes. */
    cal = g_node_new ((gpointer) g_strdup (filename));
    temp = g_strconcat ("<b>", filename, "</b>", NULL);
    gtk_label_set_markup (GTK_LABEL (name), temp);
    enable_menu_items ();
    
    g_free (filename);
    g_free (temp);
  }
  
  gtk_widget_destroy (dialog);
}

/* Open an existing calendar from the disk. */
void
on_open_clicked (GtkToolButton *button)
{
  GtkWidget *dialog, *parent, *name, *treeview;
  gchar *filename, *temp;
  
  name = glade_xml_get_widget (xml, "name");
  parent = glade_xml_get_widget (xml, "window");
  treeview = glade_xml_get_widget (xml, "treeview");
  
  /* Create a new GtkFileChooserDialog to allow the user to choose a calendar. */
  dialog = gtk_file_chooser_dialog_new ("Open an Existing Calendar", 
                                        GTK_WINDOW (parent),
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                        NULL);
  
  gint result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_ACCEPT)
  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    
    /* If the calendar was successfully opened, update the user interface. */
    if (open_calendar (filename))
    {
      temp = g_strconcat ("<b>", filename, "</b>", NULL);
      gtk_label_set_markup (GTK_LABEL (name), temp);
      enable_menu_items ();
      populate_current_date ();
      g_free (temp);
    }
    
    g_free (filename);
  }
  
  gtk_widget_destroy (dialog);
}

/* Add a new event to the currently selected day. */
void
on_add_clicked (GtkToolButton *button)
{
  GNode *node_year, *node_month, *node_day, *node_event;
  guint day, month, year;
  GtkWidget *calendar;
  Event *event;
  
  /* Retrieve information about the new event. */
  event = get_event_information ();
  if (event == NULL)
    return;
  
  calendar = glade_xml_get_widget (xml, "calendar");
  gtk_calendar_get_date (GTK_CALENDAR (calendar), &year, &month, &day);
  
  /* Retrieve or create nodes for the selected year, month, and day. */
  node_year = add_or_retrieve_element (cal, year, FALSE);
  node_month = add_or_retrieve_element (node_year, month, FALSE);
  node_day = add_or_retrieve_element (node_month, day, FALSE);
  
  /* Create a new event node and mark the day on the calendar. */
  node_event = g_node_new ((gpointer) event);
  node_event = g_node_prepend (node_day, node_event);
  gtk_calendar_mark_day (GTK_CALENDAR (calendar), day);
  populate_current_date ();
  save_calendar ();
}

/* Remove the selected event if it exists in the GtkTreeView. */
void
on_remove_clicked (GtkToolButton *button)
{
  GNode *node_year = NULL, *node_month = NULL, *node_day = NULL, *node;
  guint day, month, year, row;
  GtkWidget *treeview, *calendar;
  GtkTreeModel *store;
  GtkTreeSelection *selection;
  GtkTreeIter iter;
  GtkTreePath *path;
  
  treeview = glade_xml_get_widget (xml, "treeview");
  calendar = glade_xml_get_widget (xml, "calendar");
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
  
  if (gtk_tree_selection_get_selected (selection, &store, &iter))
  {
    /* Convert the path to an integer and remove the selected row. */
    path = gtk_tree_model_get_path (store, &iter);
    row = atoi (gtk_tree_path_to_string (path));
    gtk_list_store_remove (GTK_LIST_STORE (store), &iter);
  
    /* Find the event's node in the tree and remove it. */
    gtk_calendar_get_date (GTK_CALENDAR (calendar), &year, &month, &day);
    node_year = add_or_retrieve_element (cal, year, TRUE);
    node_month = add_or_retrieve_element (node_year, month, TRUE);
    node_day = add_or_retrieve_element (node_month, day, TRUE);
    node = g_node_nth_child (node_day, row);    
    g_node_destroy (node);
    
    /* If there are no events on the day, remove its node and unmark the day. Also
     * remove the month and year nodes if no more events exist. */
    if (g_node_n_children (node_day) == 0)
    {
      gtk_calendar_unmark_day (GTK_CALENDAR (calendar), day);
      g_node_destroy (node_day);
      
      if (g_node_n_children (node_month) == 0)
        g_node_destroy (node_month);
      if (g_node_n_children (node_year) == 0)
        g_node_destroy (node_year);
    }
  
    save_calendar ();
  }
}

/* Clear all of the events in the whole calendar. */
void
on_clear_clicked (GtkToolButton *button)
{
  GtkWidget *dialog, *treeview, *calendar;
  GtkListStore *store;
  gchar *filename;
  /* Create a dialog to ask the user if all of the events should be removed. */
  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, 
                                   GTK_BUTTONS_YES_NO, "%s\n%s",
                                   "All calendar events will be removed!",
                                   "Do you want to continue?");

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES)
  {
    /* Clear all of the events from the tree view. */
    treeview = glade_xml_get_widget (xml, "treeview");
    calendar = glade_xml_get_widget (xml, "calendar");
    store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
    gtk_list_store_clear (store);

    /* Destroy the calendar and create a new calendar out of it. */
    gtk_calendar_clear_marks (GTK_CALENDAR (calendar));
    filename = g_strdup ((gchar*) cal->data);
    g_node_destroy (cal);
    cal = NULL;
    cal = g_node_new ((gpointer) filename);
    save_calendar ();
  }
  
  gtk_widget_destroy (dialog);
}

/* Populate the GtkTreeView with new events when the day is changed. */
void
on_day_changed (GtkCalendar *calendar)
{
  if (cal != NULL)
    if (cal->children != NULL)
      populate_current_date ();
}

/* Handle changes in the month, which will require new days to be marked. */
void
on_month_changed (GtkCalendar *calendar)
{
  GNode *node_year = NULL, *node_month = NULL, *node;
  guint day, month, year;
  
  /* Return if no calendar is open or the calendar has no events. */
  if (cal == NULL)
    return;
  if (cal->children == NULL)
    return;

  /* Retrieve the selected day and destroy all of the marks on the calendar. */  
  gtk_calendar_get_date (calendar, &year, &month, &day);
  gtk_calendar_clear_marks (calendar);

  /* Retrieve the node corresponding to the selected year. */
  node_year = add_or_retrieve_element (cal, year, TRUE);
  
  /* If the year is found, retrieve the node corresponding to the selected month. */
  if (node_year != NULL)
    node_month = add_or_retrieve_element (node_year, month, TRUE);
  
  /* If the month is found, mark all of the days in that month that have events. */
  if (node_month != NULL)
    for (node = node_month->children; node != NULL; node = node->next)
      gtk_calendar_mark_day (calendar, GPOINTER_TO_INT (node->data));
}
