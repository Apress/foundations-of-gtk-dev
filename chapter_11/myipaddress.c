#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <math.h>
#include "myipaddress.h"

#define MY_IP_ADDRESS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MY_IP_ADDRESS_TYPE, MyIPAddressPrivate))

typedef struct _MyIPAddressPrivate  MyIPAddressPrivate;

struct _MyIPAddressPrivate 
{
  guint address[4];
};

enum
{
  CHANGED_SIGNAL,
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_IP1,
  PROP_IP2,
  PROP_IP3,
  PROP_IP4
};

static void my_ip_address_class_init (MyIPAddressClass*);
static void my_ip_address_init (MyIPAddress*);
static void my_ip_address_get_property (GObject*, guint, GValue*, GParamSpec*);
static void my_ip_address_set_property (GObject*, guint, 
                                         const GValue*, GParamSpec*);

static void my_ip_address_render (MyIPAddress*);
static void my_ip_address_move_cursor (GObject*, GParamSpec*);
static gboolean my_ip_address_key_pressed (GtkEntry*, GdkEventKey*);

static guint my_ip_address_signals[LAST_SIGNAL] = { 0 };

/* Get a GType that corresponds to MyIPAddress. The first time this function is
 * called (on object instantiation), the type is registered. */
GType
my_ip_address_get_type (void)
{
  static GType entry_type = 0;

  if (!entry_type)
  {
    static const GTypeInfo entry_info =
    {
      sizeof (MyIPAddressClass),
      NULL, 
      NULL,
      (GClassInitFunc) my_ip_address_class_init,
      NULL, 
      NULL,
      sizeof (MyIPAddress),
      0,
      (GInstanceInitFunc) my_ip_address_init,
    };

    entry_type = g_type_register_static (GTK_TYPE_ENTRY, "MyIPAddress", 
                                         &entry_info, 0);
  }

  return entry_type;
}

/* Initialize the MyIPAddressClass class by overriding standard functions,
 * registering a private class and setting up signals and properties. */
static void
my_ip_address_class_init (MyIPAddressClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  /* Override the standard functions for setting and retrieving properties. */
  gobject_class->set_property = my_ip_address_set_property;
  gobject_class->get_property = my_ip_address_get_property;

  /* Add MyIPAddressPrivate as a private data class of MyIPAddressClass. */
  g_type_class_add_private (klass, sizeof (MyIPAddressPrivate));

  /* Register the ip-changed signal, which will be emitted when the ip changes. */
  my_ip_address_signals[CHANGED_SIGNAL] = 
         g_signal_new ("ip-changed", G_TYPE_FROM_CLASS (klass),
                       G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                       G_STRUCT_OFFSET (MyIPAddressClass, ip_changed),
                       NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
  
  /* Register four GObject properties, one for each ip address number. */
  g_object_class_install_property (gobject_class, PROP_IP1,
                 g_param_spec_int ("ip-number-1",
                                   "IP Address Number 1",
                                   "The first IP address number",
                                   0, 255, 0,
                                   G_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class, PROP_IP2,
                 g_param_spec_int ("ip-number-2",
                                   "IP Address Number 2",
                                   "The second IP address number",
                                   0, 255, 0,
                                   G_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class, PROP_IP3,
                 g_param_spec_int ("ip-number-3",
                                   "IP Address Number 3",
                                   "The third IP address number",
                                   0, 255, 0,
                                   G_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class, PROP_IP4,
                 g_param_spec_int ("ip-number-4",
                                   "IP Address Number 1",
                                   "The fourth IP address number",
                                   0, 255, 0,
                                   G_PARAM_READWRITE));
}

/* This function is called when the programmer gives a new value for a widget
 * property with g_object_set(). */
static void
my_ip_address_set_property (GObject *object,
                            guint prop_id,
                            const GValue *value,
                            GParamSpec *pspec)
{
  MyIPAddress *ipaddress = MY_IP_ADDRESS (object);
  gint address[4] = { -1, -1, -1, -1 };

  switch (prop_id)
  {
    case PROP_IP1:
      address[0] = g_value_get_int (value);
      my_ip_address_set_address (ipaddress, address);
      break;
    case PROP_IP2:
      address[1] = g_value_get_int (value);
      my_ip_address_set_address (ipaddress, address);
      break;
    case PROP_IP3:
      address[2] = g_value_get_int (value);
      my_ip_address_set_address (ipaddress, address);
      break;
    case PROP_IP4:
      address[3] = g_value_get_int (value);
      my_ip_address_set_address (ipaddress, address);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* This function is called when the programmer requests the value of a widget
 * property with g_object_get(). */
static void
my_ip_address_get_property (GObject *object,
                            guint prop_id,
                            GValue *value,
                            GParamSpec *pspec)
{
  MyIPAddress *ipaddress = MY_IP_ADDRESS (object);
  MyIPAddressPrivate *priv = MY_IP_ADDRESS_GET_PRIVATE (ipaddress);

  switch (prop_id)
  {
    case PROP_IP1:
      g_value_set_int (value, priv->address[0]);
      break;
    case PROP_IP2:
      g_value_set_int (value, priv->address[1]);
      break;
    case PROP_IP3:
      g_value_set_int (value, priv->address[2]);
      break;
    case PROP_IP4:
      g_value_set_int (value, priv->address[3]);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* Initialize the actual MyIPAddress widget. This function is used to setup
 * the initial view of the widget and set necessary properties. */
static void
my_ip_address_init (MyIPAddress *ipaddress)
{
  MyIPAddressPrivate *priv = MY_IP_ADDRESS_GET_PRIVATE (ipaddress);
  PangoFontDescription *fd;
  guint i;
  
  for (i = 0; i < 4; i++)
    priv->address[i] = 0;
  
  fd = pango_font_description_from_string ("Monospace");
  gtk_widget_modify_font (GTK_WIDGET (ipaddress), fd);
  my_ip_address_render (ipaddress);
  pango_font_description_free (fd);
  
  /* The key-press-event signal will be used to filter out certain keys. We will
   * also monitory the cursor-position property so it can be moved correctly. */
  g_signal_connect (G_OBJECT (ipaddress), "key-press-event",
                    G_CALLBACK (my_ip_address_key_pressed), NULL);
  g_signal_connect (G_OBJECT (ipaddress), "notify::cursor-position",
                    G_CALLBACK (my_ip_address_move_cursor), NULL);
}

/* Create and return a new instance of the MyIPAddress widget. */
GtkWidget*
my_ip_address_new ()
{
  return GTK_WIDGET (g_object_new (my_ip_address_get_type (), NULL));
}

/* Return a string that represents the currently displayed IP address. */
gchar* 
my_ip_address_get_address (MyIPAddress *ipaddress)
{
  MyIPAddressPrivate *priv = MY_IP_ADDRESS_GET_PRIVATE (ipaddress);
  
  return g_strdup_printf ("%d.%d.%d.%d", priv->address[0], priv->address[1], 
                          priv->address[2], priv->address[3]);
}

/* Set the content of the IP address. Ignore any values that are out of bounds
 * so the programmer can provide a negative value for fields not to change. */
void 
my_ip_address_set_address (MyIPAddress *ipaddress, 
                           gint address[4])
{
  MyIPAddressPrivate *priv = MY_IP_ADDRESS_GET_PRIVATE (ipaddress);
  guint i;
  
  for (i = 0; i < 4; i++)
  {
    if (address[i] >= 0 && address[i] <= 255)
    {
      priv->address[i] = address[i];
    }
  }
  
  my_ip_address_render (ipaddress);
  g_signal_emit_by_name ((gpointer) ipaddress, "ip-changed");
}

/* Render the current content of the IP address in the GtkEntry widget. */
static void 
my_ip_address_render (MyIPAddress *ipaddress)
{
  MyIPAddressPrivate *priv = MY_IP_ADDRESS_GET_PRIVATE (ipaddress);
  GString *text;
  guint i;
  
  /* Create a string that displays the IP address content, adding spaces if a
   * number cannot fill three characters. */
  text = g_string_new (NULL);
  for (i = 0; i < 4; i++)
  {
    gchar *temp = g_strdup_printf ("%3i.", priv->address[i]);
    text = g_string_append (text, temp);
    g_free (temp);
  }
  
  /* Remove the trailing decimal place and add the string to the GtkEntry. */
  text = g_string_truncate (text, 15);
  gtk_entry_set_text (GTK_ENTRY (ipaddress), text->str);
  g_string_free (text, TRUE);
}

/* Force the cursor to always be at the end of one of the four numbers. */
static void 
my_ip_address_move_cursor (GObject *entry, 
                           GParamSpec *spec)
{
  gint cursor = gtk_editable_get_position (GTK_EDITABLE (entry));
  
  if (cursor <= 3)
    gtk_editable_set_position (GTK_EDITABLE (entry), 3);
  else if (cursor <= 7)
    gtk_editable_set_position (GTK_EDITABLE (entry), 7);
  else if (cursor <= 11)
    gtk_editable_set_position (GTK_EDITABLE (entry), 11);
  else
    gtk_editable_set_position (GTK_EDITABLE (entry), 15);
    
}

/* Handle key presses of numbers, tabs, backspaces and returns. */
static gboolean 
my_ip_address_key_pressed (GtkEntry *entry, 
                           GdkEventKey *event)
{
  MyIPAddressPrivate *priv = MY_IP_ADDRESS_GET_PRIVATE (entry);
  guint k = event->keyval;
  gint cursor, value;
  
  /* If the key is an integer, append the new number to the address. This is only
   * done if the resulting number will be less than 255. */
  if ((k >= GDK_0 && k <= GDK_9) || (k >= GDK_KP_0 && k <= GDK_KP_9))
  {
    cursor = floor (gtk_editable_get_position (GTK_EDITABLE (entry)) / 4);
    value = g_ascii_digit_value (event->string[0]);
    
    if ((priv->address[cursor] == 25) && (value > 5))
      return TRUE;
    
    if (priv->address[cursor] < 26)
    {
      priv->address[cursor] *= 10;
      priv->address[cursor] += value;
      my_ip_address_render (MY_IP_ADDRESS (entry));
      gtk_editable_set_position (GTK_EDITABLE (entry), (4 * cursor) + 3);
      g_signal_emit_by_name ((gpointer) entry, "ip-changed");
    }
  }
  /* Move to the next number or wrap around to the first. */
  else if (k == GDK_Tab)
  {
    cursor = (floor (gtk_editable_get_position (GTK_EDITABLE (entry)) / 4) + 1);
    gtk_editable_set_position (GTK_EDITABLE (entry), (4 * (cursor % 4)) + 3);
  }
  /* Delete the last digit of the current number. This just divides the number by
   * 10, relying on the fact that any remainder will be ignored. */
  else if (k == GDK_BackSpace)
  {
    cursor = floor (gtk_editable_get_position (GTK_EDITABLE (entry)) / 4);
    priv->address[cursor] /= 10;
    my_ip_address_render (MY_IP_ADDRESS (entry));
    gtk_editable_set_position (GTK_EDITABLE (entry), (4 * cursor) + 3);
    g_signal_emit_by_name ((gpointer) entry, "ip-changed");
  }
  /* Activate the GtkEntry widget, which corresponds to the activate signal. */
  else if ((k == GDK_Return) || (k == GDK_KP_Enter))
    gtk_widget_activate (GTK_WIDGET (entry));
  
  return TRUE;
}
