#include <gtk/gtk.h>
#include <glade/glade.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static void setup_tree_view (GtkWidget*);
void on_ping_clicked (GtkButton*);
void on_stop_clicked (GtkButton*);

enum
{
  SEQ = 0,
  ADDRESS,
  TTL,
  TIME,
  UNITS,
  COLUMNS
};

GIOChannel *channel = NULL;
GladeXML *xml = NULL;
GPid pid;

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *treeview;

  gtk_init (&argc, &argv);

  xml = glade_xml_new ("ping.glade", NULL, NULL);
  window = glade_xml_get_widget (xml, "window");
  treeview = glade_xml_get_widget (xml, "output");
  
  setup_tree_view (treeview);
  glade_xml_signal_autoconnect (xml);
  gtk_widget_show_all (window);
  gtk_main ();
  
  return 0;
}

/* Create the new tree view with five columns, each with a single cell renderer. */
static void
setup_tree_view (GtkWidget *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Seq", renderer, 
                                                     "text", SEQ, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Address", renderer, 
                                                     "text", ADDRESS, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("TTL", renderer, 
                                                     "text", TTL, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Time", renderer, 
                                                     "text", TIME, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Units", renderer, 
                                                     "text", UNITS, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}

/* Parse a line of output, which may actually be more than one line. To handle
 * multiple lines, the string is split with g_strsplit(). */
static void
parse_output (gchar *line)
{
  gchar **lines, **ptr;
  GtkWidget *treeview;
  GtkListStore *store;
  GtkTreeIter iter;
  
  /* Load the list store, split the string into lines and create a pointer. */
  treeview = glade_xml_get_widget (xml, "output");
  store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
  lines = g_strsplit (line, "\n", -1);
  ptr = lines;
  
  /* Loop through each of the lines, parsing its content. */
  while (*ptr != NULL)
  {
    /* If this is an empty string, move to the next string. */
    if (g_ascii_strcasecmp (*ptr, "") == 0)
    {
      ++ptr;
      continue;
    }
    /* Parse the line if it contains information about the current ping. */
    else if (g_ascii_strncasecmp (*ptr, "64 bytes", 8) == 0)
    {
      gchar *seq, *address, *ttl, *time, *find, *find2;
      GString *temp = g_string_new (*ptr);
      
      /* Extract the IP address of the computer you are pinging. */
      find = g_strstr_len (temp->str, temp->len, "(");
      find2 = g_strstr_len (temp->str, temp->len, ")");
      address = g_strndup (find + 1, find2 - find - 1);
      temp = g_string_erase (temp, 0, find2 - temp->str + 12);
      
      /* Extract the current ping count from the output. */
      find = g_strstr_len (temp->str, temp->len, " ttl=");
      seq = g_strndup (temp->str, find - temp->str);
      temp = g_string_erase (temp, 0, find - temp->str + 5);
      
      /* Extract the IP's "Time To Live" from the output. */
      find = g_strstr_len (temp->str, temp->len, " time=");
      ttl = g_strndup (temp->str, find - temp->str);
      temp = g_string_erase (temp, 0, find - temp->str + 6);
      
      /* Extract the time it took for this ping operation from the output. */
      find = g_strstr_len (temp->str, temp->len, " ");
      time = g_strndup (temp->str, find - temp->str);
      temp = g_string_erase (temp, 0, find - temp->str + 1);
      
      /* Append the information for the current ping operation. */
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, SEQ, seq, ADDRESS, address, TTL, ttl,
                          TIME, time, UNITS, temp->str, -1);
      
      g_string_free (temp, TRUE);
      g_free (seq);
      g_free (address);
      g_free (ttl);
      g_free (time);
    }
    
    ++ptr;
  }
  
  g_strfreev (lines);
}

/* This is a watch function that is called when the IO channel has data to read. */
static gboolean
read_output (GIOChannel *channel,
		         GIOCondition condition,
		         gpointer data)
{
	GError *error = NULL;
	GIOStatus status;
	gchar *line;
	gsize len, term;

  /* Read the current line of data from the IO channel. */
	status = g_io_channel_read_line (channel, &line, &len, &term, &error);
	
	/* If some type of error has occurred, handle it. */
	if (status != G_IO_STATUS_NORMAL || line == NULL || error != NULL) 
	{
		if (error) 
		{
			g_warning ("Error reading IO channel: %s", error->message);
			g_error_free (error);
		}

    /* Disable the stop button and enable the ping button for future action. */
    gtk_widget_set_sensitive (glade_xml_get_widget (xml, "stop"), FALSE);
    gtk_widget_set_sensitive (glade_xml_get_widget (xml, "ping"), TRUE);
    
    if (channel != NULL)
      g_io_channel_shutdown (channel, TRUE, NULL);
    channel = NULL;
    
		return FALSE;
	}

  /* Parse the line if an error has not occurred. */
  parse_output (line);
	g_free (line);

	return TRUE;
}

/* Perform a ping operation when the Ping button is clicked. */
void 
on_ping_clicked (GtkButton *button)
{
  GtkWidget *requests, *radio, *address, *treeview;
  GtkListStore *store;
  gint argc, num, fout, ret;
  const gchar *location;
  gchar *command;
  gchar **argv;
  
  requests = glade_xml_get_widget (xml, "requests");
  radio = glade_xml_get_widget (xml, "requests_num");
  address = glade_xml_get_widget (xml, "address");
  treeview = glade_xml_get_widget (xml, "output");
  
  /* Create a new tree model with five columns for ping output. */
  store = gtk_list_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                              G_TYPE_STRING, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
  g_object_unref (store);
  
  /* Retrieve the current ping parameters entered by the user. */
  num = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (requests));
  location = gtk_entry_get_text (GTK_ENTRY (address));
  
  /* Return if an address was not entered into the GtkEntry widget. */
  if (g_utf8_strlen (location, -1) == 0)
    return;
  /* Otherwise, build the command based upon the user's preferences. */
  else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radio)))
  {
    if (num == 0)
      return;
    command = g_strdup_printf ("ping %s -c %d", location, num);
  }
  else
    command = g_strdup_printf ("ping %s", location);
  
  /* Parse the command and launch the process, monitoring standard output. */
  g_shell_parse_argv (command, &argc, &argv, NULL);
  ret = g_spawn_async_with_pipes (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL,
                                  &pid, NULL, &fout, NULL, NULL);
  
  g_strfreev (argv);
  g_free (command);
  
  if (!ret)
    g_warning ("The 'ping' instruction has failed!");
  else
  {
    /* Disable the Ping button and enable the Stop button. */
    gtk_widget_set_sensitive (glade_xml_get_widget (xml, "stop"), TRUE);
    gtk_widget_set_sensitive (glade_xml_get_widget (xml, "ping"), FALSE);
	  
	  /* Create a new IO channel and monitor it for data to read. */
	  channel = g_io_channel_unix_new (fout);
    g_io_add_watch (channel, G_IO_IN | G_IO_ERR | G_IO_HUP, read_output, NULL);
	  g_io_channel_unref (channel);
  }
}

/* Kill the current ping process when the Stop button is pressed. */
void 
on_stop_clicked (GtkButton *button)
{
  gtk_widget_set_sensitive (glade_xml_get_widget (xml, "stop"), FALSE);
  gtk_widget_set_sensitive (glade_xml_get_widget (xml, "ping"), TRUE);
  
  kill (pid, SIGINT);
}
