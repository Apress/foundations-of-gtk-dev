#include <glib.h>

int main (int argc,
          char *argv[])
{
  /* Open the user's home directory for reading. */
  GDir *dir = g_dir_open (g_get_home_dir (), 0, NULL);
  const gchar *file;

  if (!g_file_test (g_get_home_dir (), G_FILE_TEST_IS_DIR))
    g_error ("Error: You do not have a home directory!");
    
  while ((file = g_dir_read_name (dir)))
    g_print ("%s\n", file);

  g_dir_close (dir);

  return 0;
}
