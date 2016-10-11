#include <gtk/gtk.h>
#include <glade/glade.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

enum
{
  OP_NULL,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_POWER
} Operations;

GladeXML *xml;
gboolean decimal_set = FALSE;
gboolean clear_value = FALSE;
gboolean value_set = FALSE;
gdouble prev_value = 0;
gint pending_op = OP_NULL;

int main (int argc, 
          char *argv[])
{
  GtkWidget *window, *entry;
  PangoFontDescription *fd;
  guint i;

  gtk_init (&argc, &argv);

  xml = glade_xml_new ("calculator.glade", NULL, NULL);
  window = glade_xml_get_widget (xml, "window");
  entry = glade_xml_get_widget (xml, "output");
  
  fd = pango_font_description_from_string ("Monospace Bold 16");
  gtk_entry_set_text (GTK_ENTRY (entry), "0");
  gtk_widget_modify_font (entry, fd);
  
  /* Set user data for the operators and decimal buttons. */
  g_object_set_data (G_OBJECT (glade_xml_get_widget (xml, "add")), 
                     "operator", GINT_TO_POINTER (OP_ADD));
  g_object_set_data (G_OBJECT (glade_xml_get_widget (xml, "sub")), 
                     "operator", GINT_TO_POINTER (OP_SUBTRACT));
  g_object_set_data (G_OBJECT (glade_xml_get_widget (xml, "mul")), 
                     "operator", GINT_TO_POINTER (OP_MULTIPLY));
  g_object_set_data (G_OBJECT (glade_xml_get_widget (xml, "div")), 
                     "operator", GINT_TO_POINTER (OP_DIVIDE));
  g_object_set_data (G_OBJECT (glade_xml_get_widget (xml, "power")), 
                     "operator", GINT_TO_POINTER (OP_POWER));
  g_object_set_data (G_OBJECT (glade_xml_get_widget (xml, "decimal")), 
                     "number", GINT_TO_POINTER (10));
  
  /* Set the user data for the number buttons. */
  for (i = 0; i < 10; i++)
  {
    gchar *name = g_strdup_printf ("num_%i", i);
    g_object_set_data (G_OBJECT (glade_xml_get_widget (xml, name)),
                       "number", GINT_TO_POINTER (i));
  }
  
  glade_xml_signal_autoconnect (xml);
  gtk_widget_show_all (window);
  gtk_main ();
  
  return 0;
}

/* Perform the specified operation, either add, subtract, multiply, divide,
 * or the power operation. */
static void
do_operation (GtkEntry *entry,
              gdouble value)
{
  gchar *text;
  gint i;

  /* Perform the operation on prev_value with the new value and store the result
   * back into prev_value. */
  switch (pending_op)
  {
    case (OP_ADD):
      prev_value += value;
      break;
    case (OP_SUBTRACT):
      prev_value -= value;
      break;
    case (OP_MULTIPLY):
      prev_value *= value;
      break;
    case (OP_DIVIDE):
      prev_value /= value;
      break;
    case (OP_POWER):
      prev_value = pow (prev_value, value);
      break;
    default:
      return;
  }
  
  /* Reset the pending operation and create a string with the new value. */
  pending_op = OP_NULL;
  text = g_strdup_printf ("%f", prev_value);
  
  /* Remove preceeding zero characters that are not needed. */
  if (g_strrstr (text, ".") != NULL)
  {
    i = strlen (text) - 1;
    while (text[i] == '0' && i > 1)
    {
      text[i] = 0;
      i--;
    }
    
    if (text[i] == '.')
      text[i] = 0;
  }
  
  /* Write the result into the GtkEntry widget. */
  gtk_entry_set_text (entry, text);
}

/* Handle number and decimal button clicks. */
void 
num_clicked (GtkButton *button)
{
  GtkWidget *entry;
  gint num;
  gchar *text;
  
  /* Retrieve the number that is stored in user data. */
  num = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "number"));
  entry = glade_xml_get_widget (xml, "output");
  
  /* Clear the value if a new number should be entered. */
  if (clear_value)
  {
    gtk_entry_set_text (GTK_ENTRY (entry), "0");
    decimal_set = FALSE;
    clear_value = FALSE;
  }
  
  /* Append a decimal place to the GtkEntry. Make sure to keep track of whether
   * the decimal place was already entered. */
  if (num == 10)
  {
    text = g_strdup_printf ("%s.", gtk_entry_get_text (GTK_ENTRY (entry)));
    
    if (decimal_set || strlen (text) > 9)
      return;

    decimal_set = TRUE;
  }
  /* Append a number place to the GtkEntry if the length is less than 10. */
  else
  {
    text = g_strdup_printf ("%s%i", gtk_entry_get_text (GTK_ENTRY (entry)), num);
  
    if (strlen (text) > 10)
      return;
  }
  
  /* Remove preceeding zeros except when the second character is a decimal point. */
  while (text[0] == '0' && text[1] != '.')
  {
    if (strlen (text) == 1)
      break;
    ++text;
  }
  
  gtk_entry_set_text (GTK_ENTRY (entry), text);
}

/* Perform any pending operations because the equal button was pressed. */
void
equal_clicked (GtkButton *button)
{
  GtkWidget *entry = glade_xml_get_widget (xml, "output");
  gdouble value = strtod (gtk_entry_get_text (GTK_ENTRY (entry)), NULL);
  
  do_operation (GTK_ENTRY (entry), value);
  clear_value = TRUE;
  value_set = FALSE;
}

/* Handle an operation button click. */
void
op_clicked (GtkButton *button)
{
  GtkWidget *entry;
  gdouble value;
  gint op;
  
  entry = glade_xml_get_widget (xml, "output");
  op = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "operator"));
  value = strtod (gtk_entry_get_text (GTK_ENTRY (entry)), NULL);
  
  /* Perform any pending operations and then store the new operation. */
  do_operation (GTK_ENTRY (entry), value);
  pending_op = op;
  clear_value = TRUE;
  
  /* Set the current value as the previous value if it should be overwritten. */
  if (!value_set)
  {
    prev_value = value;
    value_set = TRUE;
  }
}

/* Handle the +/- sign button click. */
void 
sign_clicked (GtkButton *button)
{
  GtkWidget *entry;
  gchar *text;
  gdouble value;
  gint i;
  
  entry = glade_xml_get_widget (xml, "output");
  value = strtod (gtk_entry_get_text (GTK_ENTRY (entry)), NULL) * -1;
  
  /* You cannot negate a value of zero. */
  if (value == 0.0)
    return;
  
  /* Remove any unnecessary zeros from the negated string. */
  text = g_strdup_printf ("%f", value);
  if (g_strrstr (text, ".") != NULL)
  {
    i = strlen (text) - 1;
    while (text[i] == '0' && i > 1)
    {
      text[i] = 0;
      i--;
    }
    
    if (text[i] == '.')
      text[i] = 0;
  }
  
  gtk_entry_set_text (GTK_ENTRY (entry), text);
}

/* Handle the square root button click. */
void 
sqrt_clicked (GtkButton *button)
{
  GtkWidget *entry;
  gchar *text;
  gdouble value;
  gint i;
  
  /* Take the square root of the current value. */
  entry = glade_xml_get_widget (xml, "output");
  value = pow (strtod (gtk_entry_get_text (GTK_ENTRY (entry)), NULL), 0.5);
  
  /* Remove any unnecessary zeros from the value and put it back in the GtkEntry. */
  text = g_strdup_printf ("%f", value);
  if (g_strrstr (text, ".") != NULL)
  {
    i = strlen (text) - 1;
    while (text[i] == '0' && i > 1)
    {
      text[i] = 0;
      i--;
    }
    
    if (text[i] == '.')
      text[i] = 0;
  }
  
  gtk_entry_set_text (GTK_ENTRY (entry), text);
}

/* When the clear button is clicked, reset the calculator. */
void 
clear_clicked (GtkButton *button)
{
  GtkWidget *entry;
  
  entry = glade_xml_get_widget (xml, "output");
  gtk_entry_set_text (GTK_ENTRY (entry), "0");
  
  decimal_set = FALSE;
  clear_value = FALSE;
  value_set = FALSE;
  prev_value = 0;
  pending_op = OP_NULL;
}
