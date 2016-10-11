#include "mymarquee.h"

#define MARQUEE_MIN_WIDTH 300

static void my_marquee_class_init (MyMarqueeClass*);
static void my_marquee_init (MyMarquee*);
static void my_marquee_get_property (GObject*, guint, GValue*, GParamSpec*);
static void my_marquee_set_property (GObject*, guint, const GValue*, GParamSpec*);
         
static void my_marquee_realize (GtkWidget*);
static void my_marquee_size_request (GtkWidget*, GtkRequisition*);
static void my_marquee_size_allocate (GtkWidget*, GtkAllocation*);
static gint my_marquee_expose (GtkWidget*, GdkEventExpose*);

#define MY_MARQUEE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MY_MARQUEE_TYPE, MyMarqueePrivate))

typedef struct _MyMarqueePrivate  MyMarqueePrivate;

struct _MyMarqueePrivate 
{
  gchar *message;
  gint speed;
  gint current_x;
};

enum
{
  PROP_0,
  PROP_MESSAGE,
  PROP_SPEED
};

/* Get a GType that corresponds to MyMarquee. The first time this function is
 * called (on object instantiation), the type is registered. */
GType
my_marquee_get_type ()
{
  static GType marquee_type = 0;

  if (!marquee_type)
  {
    static const GTypeInfo marquee_info =
    {
      sizeof (MyMarqueeClass),
      NULL, 
      NULL,
      (GClassInitFunc) my_marquee_class_init,
      NULL, 
      NULL,
      sizeof (MyMarquee),
      0,
      (GInstanceInitFunc) my_marquee_init,
    };

    marquee_type = g_type_register_static (GTK_TYPE_WIDGET, "MyMarquee", 
                                           &marquee_info, 0);
  }

  return marquee_type;
}

/* Initialize the MyMarqueeClass class by overriding standard functions,
 * registering a private class and setting up signals and properties. */
static void
my_marquee_class_init (MyMarqueeClass *klass)
{
  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;

  gobject_class = (GObjectClass*) klass;
  widget_class = (GtkWidgetClass*) klass;

  /* Override the standard functions for setting and retrieving properties. */
  gobject_class->set_property = my_marquee_set_property;
  gobject_class->get_property = my_marquee_get_property;

  /* Override the standard functions for realize, expose, and size changes. */
  widget_class->realize = my_marquee_realize;
  widget_class->expose_event = my_marquee_expose;
  widget_class->size_request = my_marquee_size_request;
  widget_class->size_allocate = my_marquee_size_allocate;
  
  /* Add MyMarqueePrivate as a private data class of MyMarqueeClass. */
  g_type_class_add_private (klass, sizeof (MyMarqueePrivate));
  
  /* Register four GObject properties, the message and the speed. */
  g_object_class_install_property (gobject_class, PROP_MESSAGE,
              g_param_spec_string ("message",
                                   "Marquee Message",
                                   "The message to scroll",
                                   "",
                                   G_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class, PROP_SPEED,
                 g_param_spec_int ("speed",
                                   "Speed of the Marquee",
                                   "The percentage of movement every second",
                                   1, 50, 25,
                                   G_PARAM_READWRITE));
}

/* Initialize the actual MyMarquee widget. This function is used to setup
 * the initial view of the widget and set necessary properties. */
static void
my_marquee_init (MyMarquee *marquee)
{
  MyMarqueePrivate *priv = MY_MARQUEE_GET_PRIVATE (marquee);

  priv->current_x = MARQUEE_MIN_WIDTH;
  priv->speed = 25;
}

/* This function is called when the programmer gives a new value for a widget
 * property with g_object_set(). */
static void
my_marquee_set_property (GObject *object,
                         guint prop_id,
                         const GValue *value,
                         GParamSpec *pspec)
{
  MyMarquee *marquee = MY_MARQUEE (object);

  switch (prop_id)
  {
    case PROP_MESSAGE:
      my_marquee_set_message (marquee, g_value_get_string (value));
      break;
    case PROP_SPEED:
      my_marquee_set_speed (marquee, g_value_get_int (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* This function is called when the programmer requests the value of a widget
 * property with g_object_get(). */
static void
my_marquee_get_property (GObject *object,
                         guint prop_id,
                         GValue *value,
                         GParamSpec *pspec)
{
  MyMarquee *marquee = MY_MARQUEE (object);
  MyMarqueePrivate *priv = MY_MARQUEE_GET_PRIVATE (marquee);

  switch (prop_id)
  {
    case PROP_MESSAGE:
      g_value_set_string (value, priv->message);
      break;
    case PROP_SPEED:
      g_value_set_int (value, priv->speed);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* Create and return a new instance of the MyMarquee widget. */
GtkWidget*
my_marquee_new ()
{
  return GTK_WIDGET (g_object_new (my_marquee_get_type (), NULL));
}

/* Called when the widget is realized. This usually happens when you call
 * gtk_widget_show() on the widget. */
static void
my_marquee_realize (GtkWidget *widget)
{
  MyMarquee *marquee;
  GdkWindowAttr attributes;
  gint attr_mask;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (IS_MY_MARQUEE (widget));

  /* Set the GTK_REALIZED flag so it is marked as realized. */
  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
  marquee = MY_MARQUEE (widget);

  /* Create a new GtkWindowAttr object that will hold info about the GdkWindow. */
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK);
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  
  /* Create a new GdkWindow for the widget. */
  attr_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  widget->window = gdk_window_new (widget->parent->window, &attributes, attr_mask);
  gdk_window_set_user_data (widget->window, marquee);

  /* Attach a style to the GdkWindow and draw a background color. */
  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
  gdk_window_show (widget->window);
}

/* Handle size requests for the widget. This function forces the widget to have
 * an initial size set according to the predefined width and the font size. */
static void 
my_marquee_size_request (GtkWidget *widget,
                         GtkRequisition *requisition)
{
  PangoFontDescription *fd;
  
  g_return_if_fail (widget != NULL || requisition != NULL);
  g_return_if_fail (IS_MY_MARQUEE (widget));

  fd = widget->style->font_desc;
  requisition->width = MARQUEE_MIN_WIDTH;
  requisition->height = (pango_font_description_get_size (fd) / PANGO_SCALE) + 10;
}

/* Handle size allocations for the widget. This does the actual resizing of the
 * widget to the requested allocation. */
static void
my_marquee_size_allocate (GtkWidget *widget,
                          GtkAllocation *allocation)
{
  MyMarquee *marquee;

  g_return_if_fail (widget != NULL || allocation != NULL);
  g_return_if_fail (IS_MY_MARQUEE (widget));

  widget->allocation = *allocation;
  marquee = MY_MARQUEE (widget);

  if (GTK_WIDGET_REALIZED (widget))
  {
    gdk_window_move_resize (widget->window, allocation->x, allocation->y, 
                            allocation->width, allocation->height);
  }
}

/* This function is called when an expose-event occurs on the widget. This means
 * that a part of the widget that was previously hidden is shown. */
static gint
my_marquee_expose (GtkWidget *widget,
                   GdkEventExpose *event)
{
  PangoFontDescription *fd;
  MyMarquee *marquee;
  MyMarqueePrivate *priv;
  PangoLayout *layout;
  PangoContext *context;
  gint width, height;

  g_return_val_if_fail (widget != NULL || event != NULL, FALSE);
  g_return_val_if_fail (IS_MY_MARQUEE (widget), FALSE);

  if (event->count > 0)
    return TRUE;

  marquee = MY_MARQUEE (widget);
  priv = MY_MARQUEE_GET_PRIVATE (marquee);
  fd = widget->style->font_desc;
  context = gdk_pango_context_get ();
  layout = pango_layout_new (context);
  g_object_unref (context);
  
  /* Create a new PangoLayout out of the message with the given font. */
  pango_layout_set_font_description (layout, fd);
  pango_layout_set_text (layout, priv->message, -1);
  pango_layout_get_size (layout, &width, &height);

  /* Clear the text from the background of the widget. */
  gdk_window_clear_area (widget->window, 0, 0, widget->allocation.width,
                         widget->allocation.height);

  /* Draw the PangoLayout on the widget, which is the message text. */
  gdk_draw_layout (widget->window,
                   widget->style->fg_gc[widget->state],
                   priv->current_x, 
                   (widget->allocation.height - (height / PANGO_SCALE)) / 2,
                   layout);
  
  return TRUE;
}

/* Move the message "speed" pixels to the left or wrap the message around. */
void 
my_marquee_slide (MyMarquee *marquee)
{
  PangoFontDescription *fd;
  GtkWidget *widget;
  MyMarqueePrivate *priv;
  PangoLayout *layout;
  PangoContext *context;
  gint width, height;
  
  g_return_if_fail (marquee != NULL);
  g_return_if_fail (IS_MY_MARQUEE (marquee));
  
  widget = GTK_WIDGET (marquee);
  priv = MY_MARQUEE_GET_PRIVATE (marquee);
  fd = widget->style->font_desc;
  context = gdk_pango_context_get ();
  layout = pango_layout_new (context);
  g_object_unref (context);
  
  /* Create a new PangoLayout out of the message with the given font. */
  pango_layout_set_font_description (layout, fd);
  pango_layout_set_text (layout, priv->message, -1);
  pango_layout_get_size (layout, &width, &height);

  /* Clear the text from the background of the widget. */
  gdk_window_clear_area (widget->window, 0, 0, widget->allocation.width,
                         widget->allocation.height);

  /* Scroll the message "speed" pixels to the left or wrap around. */
  priv->current_x = priv->current_x - priv->speed;
  if ((priv->current_x + (width / PANGO_SCALE)) <= 0)
    priv->current_x = widget->allocation.width;
  
  /* Draw the PangoLayout on the widget, which is the message text. */
  gdk_draw_layout (widget->window,
                   widget->style->fg_gc[widget->state],
                   priv->current_x, 
                   (widget->allocation.height - (height / PANGO_SCALE)) / 2,
                   layout);
}

/* Set the message that is displayed by the widget. */
void
my_marquee_set_message (MyMarquee *marquee, 
                        const gchar *message)
{
  MyMarqueePrivate *priv = MY_MARQUEE_GET_PRIVATE (marquee);
  
  if (priv->message)
  {
    g_free (priv->message);
    priv->message = NULL;
  }
  
  priv->message = g_strdup (message);
}

/* Retrieve the message that is displayed by the widget. You must free this
 * string after you are done using it! */
gchar* 
my_marquee_get_message (MyMarquee *marquee)
{
  return g_strdup (MY_MARQUEE_GET_PRIVATE (marquee)->message);
}

/* Set the number of pixels that the message will scroll. */
void 
my_marquee_set_speed (MyMarquee *marquee, 
                      gint speed)
{
  MyMarqueePrivate *priv = MY_MARQUEE_GET_PRIVATE (marquee);
  priv->speed = speed;
}

/* Retrieve the number of pixels that the message will scroll. */
gint 
my_marquee_get_speed (MyMarquee *marquee)
{
  return MY_MARQUEE_GET_PRIVATE (marquee)->speed;
}
