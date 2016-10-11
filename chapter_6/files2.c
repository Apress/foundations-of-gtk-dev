#include <glib.h>

static void handle_error (GError*);

int main (int argc,
          char *argv[])
{
  gchar *filename, *content;
  gsize bytes;
  GIOChannel *write, *read;
  GError *error = NULL;

  /* Build a filename in the user's home directory. */
  filename = g_build_filename (g_get_home_dir(), "temp", NULL);

  /* Set the contents of the given file and report any errors. */
  write = g_io_channel_new_file (filename, "w", &error);
  handle_error (error);
  g_io_channel_write_chars (write, "Hello World!", -1, &bytes, NULL);
  g_io_channel_close (write);
  
  if (!g_file_test (filename, G_FILE_TEST_EXISTS))
    g_error ("Error: File does not exist!\n");

  /* Get the contents of the given file and report any errors. */
  read = g_io_channel_new_file (filename, "r", &error);
  handle_error (error);
  g_io_channel_read_to_end (read, &content, &bytes, NULL);
  g_print ("%s\n", content);
  
  g_io_channel_close (read);
  g_free (content);
  g_free (filename);

  return 0;
}

static void 
handle_error (GError *error)
{
  if (error != NULL)
  {
    g_print (error->message);
    g_clear_error (&error);
  }
}
