#ifndef __MY_MARQUEE_H__
#define __MY_MARQUEE_H__

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#define MY_MARQUEE_TYPE            (my_marquee_get_type ())
#define MY_MARQUEE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_MARQUEE_TYPE, MyMarquee))
#define MY_MARQUEE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MY_MARQUEE_TYPE, MyMarqueeClass))
#define IS_MY_MARQUEE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_MARQUEE_TYPE))
#define IS_MY_MARQUEE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_MARQUEE_TYPE))

typedef struct _MyMarquee       MyMarquee;
typedef struct _MyMarqueeClass  MyMarqueeClass;

struct _MyMarquee
{
  GtkWidget widget;
};

struct _MyMarqueeClass
{
  GtkWidgetClass parent_class;
};

GType      my_marquee_get_type (void) G_GNUC_CONST;
GtkWidget* my_marquee_new      (void);

void   my_marquee_set_message (MyMarquee *marquee, const gchar *message);
gchar* my_marquee_get_message (MyMarquee *marquee);

void my_marquee_set_speed (MyMarquee *marquee, gint speed);
gint my_marquee_get_speed (MyMarquee *marquee);

void my_marquee_slide (MyMarquee *marquee);

G_END_DECLS

#endif /* __MY_MARQUEE_H__ */
