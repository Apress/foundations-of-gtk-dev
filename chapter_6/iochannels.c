#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

static void entry_changed (GtkEditable*, GIOChannel*);
static void setup_app (gint input[], gint output[], gint pid);
static gboolean iochannel_read (GIOChannel*, GIOCondition, GtkEntry*);

gulong signal_id = 0;

int main (int argc, 
          char* argv[])
{
  gint child_to_parent[2], parent_to_child[2], pid, ret_value;

  /* Setup read and write pipes for the child and parent processes. */
  ret_value = pipe (parent_to_child);
  if (ret_value == -1)
  {
    g_error ("Error: %s\n", g_strerror (errno));
   exit (1);
  }
  
  ret_value = pipe (child_to_parent);
  if (ret_value == -1)
  {
    g_error ("Error: %s\n", g_strerror (errno));
    exit (1);
  }

  /* Fork the application, setting up both instances accordingly. */
  pid = fork ();
  switch (pid)
  {
    case -1:
      g_error ("Error: %s\n", g_strerror (errno));
      exit (1);
    case 0:
      gtk_init (&argc, &argv);
      setup_app (parent_to_child, child_to_parent, pid);
      break;
    default:
      gtk_init (&argc, &argv);
      setup_app (child_to_parent, parent_to_child, pid);
  }

  gtk_main ();
  return 0;
}

/* Setup the GUI aspects of each window and setup IO channel watches. */
static void
setup_app (gint input[],
           gint output[],
           gint pid)
{
  GtkWidget *window, *entry;
  GIOChannel *channel_read, *channel_write;
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  entry = gtk_entry_new ();
  
  gtk_container_add (GTK_CONTAINER (window), entry);
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 200, -1);
  gtk_widget_show_all (window);
  
  /* Close the unnecessary pipes for the given process. */
  close (input[1]);
  close (output[0]);
  
  /* Create read and write channels out of the remaining pipes. */
  channel_read = g_io_channel_unix_new (input[0]);
  channel_write = g_io_channel_unix_new (output[1]);
  
  if (channel_read == NULL || channel_write == NULL)
    g_error ("Error: The GIOChannels could not be created!\n");
  
  /* Watch the read channel for changes. This will send the appropriate data. */
  if (!g_io_add_watch (channel_read, G_IO_IN | G_IO_HUP,
      (GIOFunc) iochannel_read, (gpointer) entry))
    g_error ("Error: Read watch could not be added to the GIOChannel!\n");

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  signal_id = g_signal_connect (G_OBJECT (entry), "changed",
                                G_CALLBACK (entry_changed),
                                (gpointer) channel_write);

  /* Set the window title depending on the process identifier. */
  if (pid == 0)
    gtk_window_set_title (GTK_WINDOW (window), "Child Process");
  else
    gtk_window_set_title (GTK_WINDOW (window), "Parent Process");
}

/* Read the message from the pipe and set the text to the GtkEntry. */
static gboolean 
iochannel_read (GIOChannel *channel, 
                GIOCondition condition, 
                GtkEntry *entry)
{
  GIOStatus ret_value;
  gchar *message;
  gsize length;
  
  /* The pipe has died unexpectedly, so exit the application. */
  if (condition & G_IO_HUP)
    g_error ("Error: The pipe has died!\n");
  
  /* Read the data that has been sent through the pipe. */
  ret_value = g_io_channel_read_line (channel, &message, &length, NULL, NULL);
  if (ret_value == G_IO_STATUS_ERROR)
    g_error ("Error: The line could not be read!\n");
  
  /* Synchronize the GtkEntry text, blocking the changed signal. Otherwise, an
   * infinite loop of communication would ensue. */
  g_signal_handler_block ((gpointer) entry, signal_id);
  message[length-1] = 0;
  gtk_entry_set_text (entry, message);
  g_signal_handler_unblock ((gpointer) entry, signal_id);
  
  return TRUE;
}

/* Write the new contents of the GtkEntry to the write IO channel. */
static void 
entry_changed (GtkEditable *entry, 
               GIOChannel *channel)
{
  gchar *text;
  gsize length;
  GIOStatus ret_value;
  
  text = g_strconcat (gtk_entry_get_text (GTK_ENTRY (entry)), "\n", NULL);
  
  /* Write the text to the channel so that the other process will get it. */
  ret_value = g_io_channel_write_chars (channel, text, -1, &length, NULL);
  if (ret_value == G_IO_STATUS_ERROR)
    g_error ("Error: The changes could not be written to the pipe!\n");
  else
    g_io_channel_flush (channel, NULL);
}
