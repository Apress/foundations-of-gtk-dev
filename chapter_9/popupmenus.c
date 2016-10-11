#include <gtk/gtk.h>

static void create_popup_menu (GtkWidget*, GtkWidget*);
static void pulse_activated (GtkMenuItem*, GtkProgressBar*);
static void clear_activated (GtkMenuItem*, GtkProgressBar*);
static void fill_activated (GtkMenuItem*, GtkProgressBar*);
static gboolean button_press_event (GtkWidget*, GdkEventButton*, GtkWidget*);

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *progress, *eventbox, *menu;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Popup Menus");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 250, -1);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  /* Create all of the necessary widgets and initialize the popup menu. */
  menu = gtk_menu_new ();
  eventbox = gtk_event_box_new ();
  progress = gtk_progress_bar_new ();
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progress), "Nothing Yet Happened");
  create_popup_menu (menu, progress);
  
  gtk_progress_bar_set_pulse_step (GTK_PROGRESS_BAR (progress), 0.05); 
  gtk_event_box_set_above_child (GTK_EVENT_BOX (eventbox), FALSE);
  
  g_signal_connect (G_OBJECT (eventbox), "button_press_event",
                    G_CALLBACK (button_press_event), menu);
  
  gtk_container_add (GTK_CONTAINER (eventbox), progress);
  gtk_container_add (GTK_CONTAINER (window), eventbox);
  
  gtk_widget_set_events (eventbox, GDK_BUTTON_PRESS_MASK);
  gtk_widget_realize (eventbox);

  gtk_widget_show_all (window);
  gtk_main ();
  return 0;
}

/* Create the popup menu and attach it to the progress bar. This will make sure
 * that the accelerators will work from application load. */
static void
create_popup_menu (GtkWidget *menu,
                   GtkWidget *progress)
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
