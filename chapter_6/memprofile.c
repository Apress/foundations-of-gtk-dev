#include <glib.h>

int main (int argc,
          char *argv[])
{
  GSList *list = NULL;
  
  /* Set the GMemVTable to the default table. This will help track memory
   * information, which will be output by g_mem_profile(). */
  g_mem_set_vtable (glib_mem_profiler_table);

  /* Call g_mem_profile() when the application exits. */
  g_atexit (g_mem_profile);

  list = (GSList*) g_malloc (sizeof (GSList));
  list->next = (GSList*) g_malloc (sizeof (GSList));
  
  g_free (list->next);

  return 0;
}
