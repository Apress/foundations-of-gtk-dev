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

enum
{
  MY_MARQUEE_SCROLL_LEFT = 0,
  MY_MARQUEE_SCROLL_RIGHT
} MyMarqueeScrollDirection;

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

GType      my_marquee_get_type ();
GtkWidget* my_marquee_new      (const gchar *message);

void my_marquee_add_message (MyMarquee *marquee, const gchar *message);
void my_marquee_remove_message (MyMarquee *marquee, const gchar *message);

void my_marquee_set_speed (MyMarquee *marquee, gint speed);
gint my_marquee_get_speed (MyMarquee *marquee);

void my_marquee_set_scroll_dir (MyMarquee *marquee, gint speed);
gint my_marquee_get_scroll_dir (MyMarquee *marquee);

void my_marquee_slide (MyMarquee *marquee);

G_END_DECLS

#endif /* __MY_MARQUEE_H__ */
