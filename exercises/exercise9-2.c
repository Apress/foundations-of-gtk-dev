#include <gtk/gtk.h>

typedef struct
{
  GtkWidget *textview, *search;
} TextEditor;

static void new_clicked (GtkToolButton*, TextEditor*);
static void open_clicked (GtkToolButton*, TextEditor*);
static void save_clicked (GtkToolButton*, TextEditor*);
static void cut_clicked (GtkToolButton*, TextEditor*);
static void copy_clicked (GtkToolButton*, TextEditor*);
static void paste_clicked (GtkToolButton*, TextEditor*);
static void find_clicked (GtkButton*, TextEditor*);

#define NUM_ENTRIES 8
static GtkActionEntry entries[] = 
{
  { "File", NULL, "_File", NULL, NULL, NULL },
  { "New", GTK_STOCK_NEW, NULL, NULL, 
     "Create a new file", G_CALLBACK (new_clicked) },
  { "Open", GTK_STOCK_OPEN, NULL, NULL, 
     "Open an existing file", G_CALLBACK (open_clicked) },
  { "Save", GTK_STOCK_SAVE, NULL, NULL, 
     "Save the document to a file", G_CALLBACK (save_clicked) },
  { "Edit", NULL, "_Edit", NULL, NULL, NULL },
  { "Cut", GTK_STOCK_CUT, NULL, NULL, 
    "Cut the selection to the clipboard", G_CALLBACK (cut_clicked) },
  { "Copy", GTK_STOCK_COPY, NULL, NULL, 
    "Copy the selection to the clipboard", G_CALLBACK (copy_clicked) },
  { "Paste", GTK_STOCK_PASTE, NULL, NULL, 
    "Paste text from the clipboard", G_CALLBACK (paste_clicked) },
};

int main (int argc,
          char *argv[])
{
  GtkWidget *window, *scrolled_win, *vbox, *searchbar, *menu, *find;
  TextEditor *editor = g_slice_new (TextEditor);
  GtkActionGroup *group;
  GtkUIManager *uimanager;
  
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Exercise 9-2");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 600, 500);
  
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  
  editor->textview = gtk_text_view_new ();
  editor->search = gtk_entry_new ();
  find = gtk_button_new_from_stock (GTK_STOCK_FIND);
  gtk_entry_set_text (GTK_ENTRY (editor->search), "Search for ...");
  
  g_signal_connect (G_OBJECT (find), "clicked",
                    G_CALLBACK (find_clicked), 
                    (gpointer) editor);
  
  group = gtk_action_group_new ("MainActionGroup");
  gtk_action_group_add_actions (group, entries, NUM_ENTRIES, (gpointer) editor);
  
  uimanager = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (uimanager, group, 0);
  gtk_ui_manager_add_ui_from_file (uimanager, "menu.ui", NULL);
  menu = gtk_ui_manager_get_widget (uimanager, "/MenuBar");
  
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (scrolled_win), editor->textview);

  searchbar = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (searchbar), editor->search, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (searchbar), find, FALSE, FALSE, 0);
  
  vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), menu, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), searchbar, FALSE, FALSE, 0);
  
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all (window);
  
  gtk_main();
  return 0;
}

/* Verify that the user want to create a new document. If so, delete
 * all of the text from the buffer. */
static void 
new_clicked (GtkToolButton *cut, 
             TextEditor *editor)
{
  GtkWidget *dialog;
  GtkTextBuffer *buffer;
  gint result;

  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, 
                       GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, 
                       "All changes will be lost. Do you want to continue?");

  result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_YES)
  {
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview));
    gtk_text_buffer_set_text (buffer, "", -1);
  }
  
  gtk_widget_destroy (dialog);
}

/* Replace the content of the current buffer with the content of a file. */
static void 
open_clicked (GtkToolButton *cut, 
              TextEditor *editor)
{
  GtkWidget *dialog;
  gint result;
  GtkTextBuffer *buffer;
  gchar *content, *file;
  
  dialog = gtk_file_chooser_dialog_new ("Choose a file ..", NULL,
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_APPLY,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        NULL);
  
  result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_APPLY)
  {
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview));
    file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

    g_file_get_contents (file, &content, NULL, NULL);
    gtk_text_buffer_set_text (buffer, content, -1);
    
    g_free (content);
    g_free (file);
  }

  gtk_widget_destroy (dialog);
}

/* Save the content of the current buffer to a file. */
static void 
save_clicked (GtkToolButton *cut, 
              TextEditor *editor)
{
  GtkWidget *dialog;
  gint result;
  GtkTextBuffer *buffer;
  gchar *content, *file;
  GtkTextIter start, end;
  
  dialog = gtk_file_chooser_dialog_new ("Save the file ..", NULL,
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        GTK_STOCK_SAVE, GTK_RESPONSE_APPLY,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        NULL);
  
  result = gtk_dialog_run (GTK_DIALOG (dialog));
  if (result == GTK_RESPONSE_APPLY)
  {
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview));
    file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      
    gtk_text_buffer_get_bounds (buffer, &start, &end);
    content = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);
      
    g_file_set_contents (file, content, -1, NULL);
    g_free (content);
    g_free (file);
  }

  gtk_widget_destroy (dialog);
}

/* Copy the selection to the clipboard and remove it from the buffer. */
static void 
cut_clicked (GtkToolButton *cut, 
             TextEditor *editor)
{
  GtkClipboard *clipboard;
  GtkTextBuffer *buffer;
  
  clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview));
  gtk_text_buffer_cut_clipboard (buffer, clipboard, TRUE);
}

/* Copy the selection to the clipboard. */
static void 
copy_clicked (GtkToolButton *copy, 
              TextEditor *editor)
{
  GtkClipboard *clipboard;
  GtkTextBuffer *buffer;
  
  clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview));
  gtk_text_buffer_copy_clipboard (buffer, clipboard);
}

/* Delete any selected text and insert the clipboard content into
 * the document. */
static void 
paste_clicked (GtkToolButton *paste, 
               TextEditor *editor)
{
  GtkClipboard *clipboard;
  GtkTextBuffer *buffer;
  
  clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview));
  gtk_text_buffer_paste_clipboard (buffer, clipboard, NULL, TRUE);
}

/* Search for a text string from the current cursor position if there is no
 * selected text, or one character after the cursor if there is. */
static void 
find_clicked (GtkButton *cut, 
              TextEditor *editor)
{
  const gchar *find;
  GtkTextBuffer *buffer;
  GtkTextMark *mark;
  GtkTextIter start, begin, end;
  GtkWidget *dialog;
  gboolean success;
  
  find = gtk_entry_get_text (GTK_ENTRY (editor->search));
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview));
  
  gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
  
  if (gtk_text_iter_compare (&start, &end) != 0)
    gtk_text_iter_forward_char (&start);

  success = gtk_text_iter_forward_search (&start, find, 0, &begin, &end, NULL);
  
  /* Select the instance on the screen if the string is found. Otherwise, tell
   * the user it has failed. */
  if (success)
  {
    mark = gtk_text_buffer_create_mark (buffer, NULL, &begin, FALSE);
    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (editor->textview), mark);
    gtk_text_buffer_delete_mark (buffer, mark);
    gtk_text_buffer_select_range (buffer, &begin, &end);
  }
  else
  {
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, 
                                     GTK_MESSAGE_INFO, GTK_BUTTONS_OK, 
                                     "The text was not found!");
    
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
  }
}
