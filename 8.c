#include <gtk/gtk.h>

// GtkButton 위젯 콜백 함수
void on_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(data),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Hello, this is a simple GTK application!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *text_view;
    GtkWidget *tree_view;
    GtkWidget *menu_bar;
    GtkWidget *menu;
    GtkWidget *menu_item;
    GtkTextBuffer *buffer;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple GTK App");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    button = gtk_button_new_with_label("Click me!");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), window);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 0, 1, 1);

    label = gtk_label_new("Type something:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 2, 1, 1);

    text_view = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, "This is a GtkTextView widget.", -1);
    gtk_grid_attach(GTK_GRID(grid), text_view, 1, 0, 1, 3);

    tree_view = gtk_tree_view_new();
    gtk_grid_attach(GTK_GRID(grid), tree_view, 0, 3, 2, 1);

    menu_bar = gtk_menu_bar_new();
    menu = gtk_menu_new();
    menu_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);
    gtk_grid_attach(GTK_GRID(grid), menu_bar, 0, 4, 2, 1);

    GtkWidget *dialog_button = gtk_button_new_with_label("Show Message Dialog");
    gtk_grid_attach(GTK_GRID(grid), dialog_button, 0, 5, 1, 1);
    g_signal_connect(dialog_button, "clicked", G_CALLBACK(on_button_clicked), window);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

