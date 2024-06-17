#include <gtk/gtk.h>

GtkWidget *text_view;

// Callback function to quit the application
static void on_quit_activate(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

// Callback function to save the content
static void on_save_activate(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer;
    GtkTextIter start, end;
    gchar *text;
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    gchar *filename;

    dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(data), 
                                         GTK_FILE_CHOOSER_ACTION_SAVE, 
                                         "_Cancel", GTK_RESPONSE_CANCEL, 
                                         "_Save", GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(chooser);
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        g_file_set_contents(filename, text, -1, NULL);
        g_free(filename);
        g_free(text);
    }

    gtk_widget_destroy(dialog);
}

// Callback function to open and load a text file
static void on_open_activate(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer;
    gchar *text;
    gsize length;
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    gchar *filename;
    GError *error = NULL;

    dialog = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(data), 
                                         GTK_FILE_CHOOSER_ACTION_OPEN, 
                                         "_Cancel", GTK_RESPONSE_CANCEL, 
                                         "_Open", GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(chooser);
        if (g_file_get_contents(filename, &text, &length, &error)) {
            buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
            gtk_text_buffer_set_text(buffer, text, length);
            g_free(text);
        } else {
            g_print("Error opening file: %s\n", error->message);
            g_error_free(error);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Callback function to cut text
static void on_cut_activate(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_cut_clipboard(buffer, gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), TRUE);
}

// Callback function to copy text
static void on_copy_activate(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_copy_clipboard(buffer, gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
}

// Callback function to paste text
static void on_paste_activate(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_paste_clipboard(buffer, gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), NULL, TRUE);
}

// Callback function to toggle view (placeholder)
static void on_toggle_view_activate(GtkWidget *widget, gpointer data) {
    g_print("Toggle view menu item activated\n");
}

// Callback function to change the font
static void on_font_activate(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_font_chooser_dialog_new("Select Font", GTK_WINDOW(data));
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        PangoFontDescription *font_desc;
        gchar *fontname = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
        font_desc = pango_font_description_from_string(fontname);
        gtk_widget_override_font(text_view, font_desc);
        pango_font_description_free(font_desc);
        g_free(fontname);
    }
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *menu_bar;
    GtkWidget *file_menu, *edit_menu, *view_menu, *format_menu;
    GtkWidget *file_menu_item, *edit_menu_item, *view_menu_item, *format_menu_item;
    GtkWidget *open_menu_item, *save_menu_item, *quit_menu_item;
    GtkWidget *cut_menu_item, *copy_menu_item, *paste_menu_item;
    GtkWidget *toggle_view_menu_item;
    GtkWidget *font_menu_item;
    GtkWidget *scroll_window;

    gtk_init(&argc, &argv);

    // Create a new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Notes App");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the menu and text view
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a menu bar
    menu_bar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // Create the File menu
    file_menu = gtk_menu_new();
    file_menu_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);

    // Add Open, Save, and Quit items to the File menu
    open_menu_item = gtk_menu_item_new_with_label("Open");
    g_signal_connect(open_menu_item, "activate", G_CALLBACK(on_open_activate), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_menu_item);

    save_menu_item = gtk_menu_item_new_with_label("Save");
    g_signal_connect(save_menu_item, "activate", G_CALLBACK(on_save_activate), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_menu_item);

    quit_menu_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_menu_item, "activate", G_CALLBACK(on_quit_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_menu_item);

    // Create the Edit menu
    edit_menu = gtk_menu_new();
    edit_menu_item = gtk_menu_item_new_with_label("Edit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_menu_item);

    // Add Cut, Copy, and Paste items to the Edit menu
    cut_menu_item = gtk_menu_item_new_with_label("Cut");
    g_signal_connect(cut_menu_item, "activate", G_CALLBACK(on_cut_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), cut_menu_item);

    copy_menu_item = gtk_menu_item_new_with_label("Copy");
    g_signal_connect(copy_menu_item, "activate", G_CALLBACK(on_copy_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), copy_menu_item);

    paste_menu_item = gtk_menu_item_new_with_label("Paste");
    g_signal_connect(paste_menu_item, "activate", G_CALLBACK(on_paste_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), paste_menu_item);

    // Create the View menu
    view_menu = gtk_menu_new();
    view_menu_item = gtk_menu_item_new_with_label("View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_menu_item), view_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view_menu_item);

    // Add a Toggle View item to the View menu
    toggle_view_menu_item = gtk_menu_item_new_with_label("Toggle View");
    g_signal_connect(toggle_view_menu_item, "activate", G_CALLBACK(on_toggle_view_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), toggle_view_menu_item);

    // Create the Format menu
    format_menu = gtk_menu_new();
    format_menu_item = gtk_menu_item_new_with_label("Format");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(format_menu_item), format_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), format_menu_item);

    // Add a Font item to the Format menu
    font_menu_item = gtk_menu_item_new_with_label("Font");
    g_signal_connect(font_menu_item, "activate", G_CALLBACK(on_font_activate), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), font_menu_item);

    // Create a scrolled window
    scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scroll_window, TRUE, TRUE, 0);

    // Create a text view
    text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scroll_window), text_view);

    // Add padding to the text view
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 10);

    // Show all widgets
    gtk_widget_show_all(window);

    // Run the GTK main loop
    gtk_main();

    return 0;
}
