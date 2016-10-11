#include <glib.h>
#include <gmodule.h>

G_MODULE_EXPORT gboolean
print_the_message (gpointer data)
{
  g_print ("%s\n", (gchar*) data);
  return TRUE;
}

G_MODULE_EXPORT gboolean
print_another_one (gpointer data)
{
  g_print ("%s\n", (gchar*) data);
  return TRUE;
}
