#include <gtk/gtk.h>

int main (int argc, 
          char *argv[])
{
  GtkWidget *dialog;
  GdkPixbuf *logo;
  GError *error = NULL;

  gtk_init (&argc, &argv);

  const gchar *authors[] = {
    "Author #1", 
    "Author #2",
    NULL
  };

  const gchar *documenters[] = {
    "Documenter #1",
    "Documenter #2",
    NULL
  };
  
  dialog = gtk_about_dialog_new ();
  
  /* You should edit '/path/to/logo.png' to point to the location of logo.png 
   * from the chapter_5 source directory on your system. */
  logo = gdk_pixbuf_new_from_file ("/path/to/logo.png", &error);

  /* Set the application logo or handle the error. */
  if (error == NULL)
    gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), logo);
  else
  {
    if (error->domain == GDK_PIXBUF_ERROR)
      g_print ("GdkPixbufError: %s\n", error->message);
    else if (error->domain == G_FILE_ERROR)
      g_print ("GFileError: %s\n", error->message);
    else
      g_print ("An error in the unexpected domain:%d has occurred!\n", error->domain);

    g_error_free (error);
  }

  /* Set application data that will be displayed in the main dialog. */
  gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (dialog), "GtkAboutDialog");
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), "1.0");
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), 
                                  "(C) 2007 Andrew Krause");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), 
                                 "All About GtkAboutDialog");

  /* Set the license text, which is usually loaded from a file. Also, set the
   * web site address and label. */
  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), "Free to all!");
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), 
                                "http://www.gtkbook.com");
  gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG (dialog), 
                                      "www.gtkbook.com");

  /* Set the application authors, documenters and translators. */
  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
  gtk_about_dialog_set_documenters (GTK_ABOUT_DIALOG (dialog), documenters);
  gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog), 
                                           "Translator #1\nTranslator #2");

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  return 0;
}
