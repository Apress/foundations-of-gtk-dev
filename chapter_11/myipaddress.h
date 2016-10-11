#ifndef __MY_IP_ADDRESS_H__
#define __MY_IP_ADDRESS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkentry.h>

G_BEGIN_DECLS

#define MY_IP_ADDRESS_TYPE            (my_ip_address_get_type ())
#define MY_IP_ADDRESS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_IP_ADDRESS_TYPE, MyIPAddress))
#define MY_IP_ADDRESS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MY_IP_ADDRESS_TYPE, MyIPAddressClass))
#define IS_MY_IP_ADDRESS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_IP_ADDRESS_TYPE))
#define IS_MY_IP_ADDRESS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_IP_ADDRESS_TYPE))

typedef struct _MyIPAddress        MyIPAddress;
typedef struct _MyIPAddressClass   MyIPAddressClass;

struct _MyIPAddress
{
  GtkEntry entry;
};

struct _MyIPAddressClass
{
  GtkEntryClass parent_class;
  
  void (* ip_changed) (MyIPAddress *ipaddress);
};

GType      my_ip_address_get_type (void) G_GNUC_CONST;
GtkWidget* my_ip_address_new      (void);

gchar* my_ip_address_get_address (MyIPAddress *ipaddress);
void   my_ip_address_set_address (MyIPAddress *ipaddress, gint address[4]);

G_END_DECLS

#endif /* __MY_IP_ADDRESS_H__ */
