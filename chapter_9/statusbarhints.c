#include <gtk/gtk.h>

static void create_popup_menu (GtkWidget*, GtkWidget*, GtkWidget*);
static void pulse_activated (GtkMenuItem*, GtkProgressBar*);
static void clear_activated (GtkMenuItem*, GtkProgressBar*);
static void fill_activated (GtkMenuItem*, GtkProgressBar*);
static gboolean statusbar_hint (GtkMenuItem*, GdkEventProximity*, GtkStatusbar*);
static gboolean button_press_event (GtkWidget*, GdkEventButton*, GtkWidget*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *progress, *eventbox, *statusbar, *vbox, *menu;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Status Bar Hints");
  gtk_widget_set_size_request (window, 250, -1);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  menu = gtk_menu_new ();
  eventbox = gtk_event_box_new ();
  progress = gtk_progress_bar_new ();
  statusbar = gtk_statusbar_new ();
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progress), "Nothing Yet Happened");
  create_popup_menu (menu, progress, statusbar);
  
  gtk_progress_bar_set_pulse_step (GTK_PROGRESS_BAR (progress), 0.05); ;  
  gtk_event_box_set_above_child (GTK_EVENT_BOX (eventbox), FALSE);
  
  g_signal_connect (G_OBJECT (eventbox), "button_press_event",
                    G_CALLBACK (button_press_event),
                    (gpointer) menu);
  
  vbox = gtk_vbox_new (FALSE, 5);  
  gtk_container_add (GTK_CONTAINER (eventbox), progress);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), eventbox);
  gtk_box_pack_start_defaults (GTK_BOX (vbox), statusbar);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  
  gtk_widget_set_events (eventbox, GDK_BUTTON_PRESS_MASK);
  gtk_widget_realize (eventbox);

  gtk_widget_show_all (window);
  gtk_main ();
  return 0;
}

/* Create the popup menu and attach it to the progress bar. */
static void
create_popup_menu (GtkWidget *menu,
                   GtkWidget *progress,
                   GtkWidget *statusbar)
{
  GtkWidget *pulse, *fill, *clear, *separator;
  
  pulse = gtk_menu_item_new_with_label ("Pulse Progress");
  fill = gtk_menu_item_new_with_label ("Set as Complete");
  clear = gtk_menu_item_new_with_label ("Clear Progress");
  separator = gtk_separator_menu_item_new ();
  
  g_signal_connect (G_OBJECT (pulse), "activate",
                    G_CALLBACK (pulse_activated), progress);
  g_signal_connect (G_OBJECT (fill), "activate",
                    G_CALLBACK (fill_activated), progress);
  g_signal_connect (G_OBJECT (clear), "activate",
                    G_CALLBACK (clear_activated), progress);
  
  g_signal_connect (G_OBJECT (pulse), "enter-notify-event",
                    G_CALLBACK (statusbar_hint), statusbar);
  g_signal_connect (G_OBJECT (pulse), "leave-notify-event",
                    G_CALLBACK (statusbar_hint), statusbar);
  g_signal_connect (G_OBJECT (fill), "enter-notify-event",
                    G_CALLBACK (statusbar_hint), statusbar);
  g_signal_connect (G_OBJECT (fill), "leave-notify-event",
                    G_CALLBACK (statusbar_hint), statusbar);
  g_signal_connect (G_OBJECT (clear), "enter-notify-event",
                    G_CALLBACK (statusbar_hint), statusbar);
  g_signal_connect (G_OBJECT (clear), "leave-notify-event",
                    G_CALLBACK (statusbar_hint), statusbar);
    
  g_object_set_data (G_OBJECT (pulse), "menuhint",
                     (gpointer) "Pulse the progress bar one step.");
  g_object_set_data (G_OBJECT (fill), "menuhint",
                     (gpointer) "Set the progress bar to 100%.");
  g_object_set_data (G_OBJECT (clear), "menuhint",
                     (gpointer) "Clear the progress bar to 0%.");
    
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), pulse);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), fill);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), clear);
  
  gtk_menu_attach_to_widget (GTK_MENU (menu), progress, NULL);
  gtk_widget_show_all (menu);
}

/* Create the poup menu with three items and a separator. Then, attach it to
 * the progress bar and show it to the user. */
static gboolean 
button_press_event (GtkWidget *eventbox, 
                    GdkEventButton *event, 
                    GtkWidget *menu)
{
  if ((event->button == 3) && (event->type == GDK_BUTTON_PRESS))
  {
    gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 
                    event->button, event->time);
    return TRUE;
  }

  return FALSE;
}

static void 
pulse_activated (GtkMenuItem *item, 
                 GtkProgressBar *progress)
{
  gtk_progress_bar_pulse (progress);
  gtk_progress_bar_set_text (progress, "Pulse!");
}

static void 
fill_activated (GtkMenuItem *item, 
                GtkProgressBar *progress)
{
  gtk_progress_bar_set_fraction (progress, 1.0);
  gtk_progress_bar_set_text (progress, "One Hundred Percent");
}

static void 
clear_activated (GtkMenuItem *item, 
                 GtkProgressBar *progress)
{
  gtk_progress_bar_set_fraction (progress, 0.0);
  gtk_progress_bar_set_text (progress, "Reset to Zero");
}

/* Add or remove a status bar menu hint, depending on whether this function
 * is initialized by a proximity-in-event or proximity-out-event. */
static gboolean 
statusbar_hint (GtkMenuItem *menuitem, 
                GdkEventProximity *event, 
                GtkStatusbar *statusbar)
{
  gchar *hint;
  guint id = gtk_statusbar_get_context_id (statusbar, "MenuItemHints");

  if (event->type == GDK_ENTER_NOTIFY)
  {
    hint = (gchar*) g_object_get_data (G_OBJECT (menuitem), "menuhint");
    gtk_statusbar_push (statusbar, id, hint);
  }
  else if (event->type == GDK_LEAVE_NOTIFY)
    gtk_statusbar_pop (statusbar, id);
  
  return FALSE;
}

