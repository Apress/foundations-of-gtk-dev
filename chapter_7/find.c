#include <gtk/gtk.h>

typedef struct
{
  GtkWidget *entry, *textview;
} Widgets;

static void search (GtkButton*, Widgets*);

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *scrolled_win, *vbox, *hbox, *find;
  Widgets *w = g_slice_new (Widgets);

  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Searching Buffers");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  w->textview = gtk_text_view_new ();
  w->entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (w->entry), "Search for ...");
  find = gtk_button_new_from_stock (GTK_STOCK_FIND);
  
  g_signal_connect (G_OBJECT (find), "clicked",
                    G_CALLBACK (search),
                    (gpointer) w);
  
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolled_win, 250, 200);
  gtk_container_add (GTK_CONTAINER (scrolled_win), w->textview);
  
  hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), w->entry, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), find, FALSE, TRUE, 0);

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
  
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);
  
  gtk_main();
  return 0;
}

/* Search for the entered string within the GtkTextView. Then tell the user
 * how many times it was found. */
static void 
search (GtkButton *button, 
        Widgets *w)
{
  const gchar *find;
  gchar *output;
  GtkWidget *dialog;
  GtkTextBuffer *buffer; 
  GtkTextIter start, begin, end;
  gboolean success;
  gint i = 0;
  
  find = gtk_entry_get_text (GTK_ENTRY (w->entry));
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w->textview));

  gtk_text_buffer_get_start_iter (buffer, &start);
  success = gtk_text_iter_forward_search (&start, (gchar*) find, 0, 
                                          &begin, &end, NULL);

  while (success)
  {
    gtk_text_iter_forward_char (&start);
    success = gtk_text_iter_forward_search (&start, (gchar*) find, 0, 
                                            &begin, &end, NULL);
    start = begin;
    i++;
  }

  output = g_strdup_printf ("The string '%s' was found %i times!", find, i);
  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, 
                                   GTK_BUTTONS_OK, output, NULL);
    
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  g_free (output);
}
