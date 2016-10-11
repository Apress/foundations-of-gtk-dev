#ifndef __MY_IFACE_H__
#define __MY_IFACE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MY_TYPE_IFACE    (my_iface_get_type ())
#define MY_IFACE(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IFACE, MyIFace))
#define MY_IS_IFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IFACE))

#define MY_IFACE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MY_TYPE_IFACE, MyIFaceInterface))

typedef struct _MyIFace           MyIFace;
typedef struct _MyIFaceInterface  MyIFaceInterface;

struct _MyIFaceInterface 
{
  GTypeInterface parent;

  void (*print_message) (MyIFace *obj, gchar *message);
};

GType my_iface_get_type ();
void my_iface_print_message (MyIFace *obj, gchar *message);

G_END_DECLS

#endif /* __MY_IFACE_H__ */
