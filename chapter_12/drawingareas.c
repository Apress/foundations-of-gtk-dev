#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

static gboolean button_pressed (GtkWidget*, GdkEventButton*, GPtrArray*);
static gboolean motion_notify (GtkWidget*, GdkEventMotion*, GPtrArray*);
static gboolean key_pressed (GtkWidget*, GdkEventKey*, GPtrArray*);
static gboolean expose_event (GtkWidget*, GdkEventExpose*, GPtrArray*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *area;
  GPtrArray *parray;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Drawing Areas");
  gtk_widget_set_size_request (window, 400, 300);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  /* Create a pointer array to hold image data. Then, add event masks to the new
   * drawing area widget. */
  parray = g_ptr_array_sized_new (5000);
  area = gtk_drawing_area_new ();
  GTK_WIDGET_SET_FLAGS (area, GTK_CAN_FOCUS);
  gtk_widget_add_events (area, GDK_BUTTON_PRESS_MASK | 
                               GDK_BUTTON_MOTION_MASK | 
                               GDK_KEY_PRESS_MASK);
  
  g_signal_connect (G_OBJECT (area), "button_press_event",
                    G_CALLBACK (button_pressed), parray);
  g_signal_connect (G_OBJECT (area), "motion_notify_event",
                    G_CALLBACK (motion_notify), parray);
  g_signal_connect (G_OBJECT (area), "key_press_event",
                    G_CALLBACK (key_pressed), parray);
  g_signal_connect (G_OBJECT (area), "expose_event",  
                    G_CALLBACK (expose_event), parray);

  gtk_container_add (GTK_CONTAINER (window), area);
  gtk_widget_show_all (window);
  
  /* You must do this after the widget is visible because it must first
   * be realized for the GdkWindow to be valid! */
  gdk_window_set_cursor (area->window, gdk_cursor_new (GDK_PENCIL));

  gtk_main ();
  return 0;
}

/* Redraw all of the points when an expose-event occurs. If you do not do this,
 * the drawing area will be cleared. */
static gboolean
expose_event (GtkWidget *area,
              GdkEventExpose *event,
              GPtrArray *parray)
{
  guint i, x, y;
  GdkPoint points[5];
  
  /* Loop through the coordinates, redrawing them onto the drawing area. */
  for (i = 0; i < parray->len; i = i + 2)
  {
    x = GPOINTER_TO_INT (parray->pdata[i]);
    y = GPOINTER_TO_INT (parray->pdata[i+1]);
    
    points[0].x = x;   points[0].y = y;
    points[1].x = x+1; points[1].y = y;
    points[2].x = x-1; points[2].y = y;
    points[3].x = x;   points[3].y = y+1;
    points[4].x = x;   points[4].y = y-1;
    
    gdk_draw_points (area->window,
                     area->style->fg_gc[GTK_WIDGET_STATE (area)],
                     points, 5);
  }
  
  return TRUE;
}

/* Draw a point where the user pressed the mouse and points on each of the
 * four sides of that point. */
static gboolean 
button_pressed (GtkWidget *area,
                GdkEventButton *event,
                GPtrArray *parray)
{
  gint x = event->x, y = event->y;
  GdkPoint points[5] = { {x,y}, {x+1,y}, {x-1,y}, {x,y+1}, {x,y-1} };

  gdk_draw_points (area->window,
                   area->style->fg_gc[GTK_WIDGET_STATE (area)],
                   points, 5);

  g_ptr_array_add (parray, GINT_TO_POINTER (x));
  g_ptr_array_add (parray, GINT_TO_POINTER (y));
  
  return FALSE;
}

/* Draw a point where the moved the mouse pointer while a button was
 * pressed along with points on each of the four sides of that point. */
static gboolean
motion_notify (GtkWidget *area,
               GdkEventMotion *event,
               GPtrArray *parray)
{
  gint x = event->x, y = event->y;
  GdkPoint points[5] = { {x,y}, {x+1,y}, {x-1,y}, {x,y+1}, {x,y-1} };

  gdk_draw_points (area->window,
                   area->style->fg_gc[GTK_WIDGET_STATE (area)],
                   points, 5);

  g_ptr_array_add (parray, GINT_TO_POINTER (x));
  g_ptr_array_add (parray, GINT_TO_POINTER (y));

  return FALSE;
}

/* Clear the drawing area when the user presses the Delete key. */
static gboolean 
key_pressed (GtkWidget *area,
             GdkEventKey *event,
             GPtrArray *parray)
{
  if (event->keyval == GDK_Delete)
  {
    gdk_window_clear (area->window);
    g_ptr_array_remove_range (parray, 0, parray->len);
  }
  
  return FALSE;
}
