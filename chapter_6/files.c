#include <glib.h>

static void handle_error (GError*);

int main (int argc,
          char *argv[])
{
  gchar *filename, *content;
  gsize bytes;
  GError *error = NULL;

  /* Build a filename in the user's home directory. */
  filename = g_build_filename (g_get_home_dir(), "temp", NULL);

  /* Set the contents of the given file and report any errors. */
  g_file_set_contents (filename, "Hello World!", -1, &error);
  handle_error (error);

  if (!g_file_test (filename, G_FILE_TEST_EXISTS))
    g_error ("Error: File does not exist!\n");

  /* Get the contents of the given file and report any errors. */
  g_file_get_contents (filename, &content, &bytes, &error);
  handle_error (error);
  g_print ("%s\n", content);

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
