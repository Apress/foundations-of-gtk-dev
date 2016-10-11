#include "myiface.h"

static void my_iface_class_init (gpointer iface);

/* Declare the interface type, registering it as a simply static type. Then, add
 * the interface's prerequisite type as GTK_TYPE_WIDGET. */
GType
my_iface_get_type ()
{
  static GType type = 0;

  if (!type)
  {
    type = g_type_register_static_simple (G_TYPE_INTERFACE, "MyIFace",
                                          sizeof (MyIFaceInterface),
                                          (GClassInitFunc) my_iface_class_init,
                                          0, NULL, 0);

    g_type_interface_add_prerequisite (type, GTK_TYPE_WIDGET);
  }

  return type;
}

/* Initialize the interface's class. These properties and signals will be added
 * to any class that implements the interface. */
static void
my_iface_class_init (gpointer iface)
{
  GType iface_type = G_TYPE_FROM_INTERFACE (iface);

  /* Install signals & properties here ... */
}

/* This function simply calls the class's installed implementation of the
 * print_message() function. */
void 
my_iface_print_message (MyIFace *obj,
                        gchar *message)
{
  MY_IFACE_GET_INTERFACE (obj)->print_message (obj, message);
}
