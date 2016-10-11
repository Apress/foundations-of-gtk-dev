#include <gtk/gtk.h>
#include <glade/glade.h>
#include <string.h>

static gboolean draw_area (GtkWidget*, GdkEventExpose*);

/* A list of words to choose from. You could read a larger list from a file. */
static gchar *words[] = { "The GIMP Toolkit", "GtkDrawingArea", "GdkEventExpose" };

GladeXML *xml = NULL;
gchar *content;
gint word, state = 0;

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *area;
  GRand *random;
  gint i;

  gtk_init (&argc, &argv);

  xml = glade_xml_new ("hangman.glade", NULL, NULL);
  window = glade_xml_get_widget (xml, "window");
  area = glade_xml_get_widget (xml, "area");
  
  /* Choose the word to use and fill a string with the same number of periods. */
  random = g_rand_new ();
  word = g_rand_int_range (random, 0, 2);
  content = g_strnfill (strlen (words[word]), '.');
  
  /* Replace any periods in the solution with spaces. */
  for (i = 0; i < strlen (words[word]); i++)
    if (words[word][i] == ' ')
      content[i] = ' ';
  
  GTK_WIDGET_SET_FLAGS (area, GTK_CAN_FOCUS);
  g_signal_connect (G_OBJECT (area), "expose_event",  
                    G_CALLBACK (draw_area), NULL);
  
  glade_xml_signal_autoconnect (xml);
  gtk_widget_show_all (window);
  gtk_main ();
  
  return 0;
}

/* Draw the actual Hangman character based upon the current state of the game. */
static void
draw_hangman (GtkWidget *area,
              GdkGC *gcontext)
{
  static GdkPoint head[] = { {160,35}, {169,41}, {172,47}, {169,54}, {160,60},
                             {151,54}, {148,47}, {151,41} };

  /* There are five parts. A head, body, arms, and two legs since the arms of the
   * man are counted together. */
  switch (state)
  {
    case 5:
      gdk_draw_line (area->window, gcontext, 160, 90, 175, 115);
    case 4:
      gdk_draw_line (area->window, gcontext, 145, 115, 160, 90);
    case 3:
      gdk_draw_line (area->window, gcontext, 145, 75, 175, 75);
    case 2:
      gdk_draw_line (area->window, gcontext, 160, 60, 160, 90);
    case 1:
      gdk_draw_polygon (area->window, gcontext, FALSE, head, 8);
  }
  
}

/* Draw the game area without the actual man. This includes the border, the hanger
 * and the current state of the solution string. */
static gboolean
draw_area (GtkWidget *area,
           GdkEventExpose *event)
{
  static GdkPoint border[] = { {0,0}, {299,0}, {299,199}, {0,199}, {0,0} };
  static GdkPoint hanger[] = { {123,25}, {163,25}, {163,35}, {158,35}, {158,30}, 
                               {128,30}, {128,125}, {188,125}, {188,130}, {113,130},
                               {113,125}, {123,125}, {123,25} };

  GdkGC *gcontext = area->style->fg_gc[GTK_STATE_NORMAL];
  PangoFontDescription *fd;
  PangoLayout *layout;
  PangoContext *context;
  gint width, x;
  
  /* Draw the border and hanger for the game area. */
  gdk_window_clear (area->window);
  gdk_draw_lines (area->window, gcontext, border, 5);
  gdk_draw_polygon (area->window, gcontext, TRUE, hanger, 13);
  
  /* Create a PangoLayout for the string and display it in the drawing area. */
  context = gdk_pango_context_get ();
  layout = pango_layout_new (context);
  fd = pango_font_description_from_string ("Monospace 14");
  pango_font_description_set_stretch (fd, PANGO_STRETCH_EXPANDED);
  pango_layout_set_text (layout, content, -1);
  pango_layout_set_font_description (layout, fd);
  pango_layout_get_size (layout, &width, NULL);
  x = (300 - (width / PANGO_SCALE)) / 2;
  
  /* Draw the message on the GdkWindow and draw the current state of the man. */
  gdk_draw_layout (area->window, gcontext, x, 155, layout);
  draw_hangman (area, gcontext);
  
  pango_font_description_free (fd);
  g_object_unref (context);
  return TRUE;
}

/* Handle a GtkButton clicked signal emitted from one of the 26 letters. */
void
on_letter_clicked (GtkButton *button)
{
  GtkWidget *area, *dialog, *parent;
  gboolean set = FALSE;
  const gchar *label;
  gint i, finished = 0;
  
  parent = glade_xml_get_widget (xml, "window");
  area = glade_xml_get_widget (xml, "area");
  label = gtk_button_get_label (GTK_BUTTON (button));
  
  /* Loop through the word, checking upper and lower-case versions of the letter. */
  for (i = 0; i < strlen (words[word]); i++)
  {
    if (words[word][i] == label[0])
    {
      content[i] = label[0];
      set = TRUE;
    }
    else if (words[word][i] == (label[0] + 32))
    {
      content[i] = label[0] + 32;
      set = TRUE;
    }
    
    /* If finished is greater than 0, we know that the string is not complete. */
    if (content[i] == '.')
      finished++;
  }
  
  /* If the letter was not found, add a new body part. */
  if (!set)
    state++;
  
  /* Draw the game board and set the clicked button as disabled. */
  draw_area (area, NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
  
  /* If the full man is displayed, tell the user that he/she lost. */
  if (state >= 5)
  {
    dialog = gtk_message_dialog_new (GTK_WINDOW (parent), GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                     "You lost Hangman!");

    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
    gtk_main_quit ();
  }
  /* If the message is fully displayed, tell the user that he/she won. */
  else if (finished == 0)
  {
    dialog = gtk_message_dialog_new (GTK_WINDOW (parent), GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                     "You won Hangman!");
    
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
    gtk_main_quit ();
  }
}
