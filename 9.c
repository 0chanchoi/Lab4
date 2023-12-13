#include <gtk/gtk.h>

static GtkWidget *entry;

// 숫자 버튼을 누를 때 호출되는 콜백 함수
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    const gchar *button_text = gtk_button_get_label(GTK_BUTTON(widget));

    gchar *new_text = g_strconcat(text, button_text, NULL);
    gtk_entry_set_text(GTK_ENTRY(entry), new_text);
    g_free(new_text);
}

// "=" 버튼을 누를 때 호출되는 콜백 함수
static void on_equal_clicked(GtkWidget *widget, gpointer data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    gchar *calc_result;

    // "ERROR"로 초기화
    //gtk_entry_set_text(GTK_ENTRY(entry), "ERROR");

    // 숫자 추출
    gdouble num1, num2;
    gboolean success = g_str_has_prefix(text, "-") ? g_strtod(text, &calc_result) : g_ascii_strtod(text, &calc_result);
    if (!success) {
        return;
    }
    num1 = g_ascii_strtod(text, NULL);

    // 사칙연산 수행
    gchar *operator = g_utf8_strchr(text, -1, '+');
    if (!operator) {
        operator = g_utf8_strchr(text, -1, '-');
    }
    if (!operator) {
        operator = g_utf8_strchr(text, -1, '*');
    }
    if (!operator) {
        operator = g_utf8_strchr(text, -1, '/');
    }

    if (operator) {
        num2 = g_ascii_strtod(operator + 1, NULL);
        gdouble result = 0.0;
        switch (*operator) {
            case '+':
                result = num1 + num2;
                break;
            case '-':
                result = num1 - num2;
                break;
            case '*':
                result = num1 * num2;
                break;
            case '/':
                if (num2 != 0) {
                    result = num1 / num2;
                } else {
                    gtk_entry_set_text(GTK_ENTRY(entry), "ERROR: Division by zero");
                    return;
                }
                break;
        }

        gchar *str_result = g_strdup_printf("%.2f", result);
        gtk_entry_set_text(GTK_ENTRY(entry), str_result);
        g_free(str_result);
    }
}

// GUI 생성 및 초기화
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Calculator");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    gchar *buttons[] = { "7", "8", "9", "/",
                         "4", "5", "6", "*",
                         "1", "2", "3", "-",
                         "0", ".", "=", "+" };

    guint i = 0;
    for (guint row = 1; row < 5; row++) {
        for (guint col = 0; col < 4; col++) {
            button = gtk_button_new_with_label(buttons[i]);
            gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);

            if (g_ascii_strcasecmp(buttons[i], "=") == 0) {
                g_signal_connect(button, "clicked", G_CALLBACK(on_equal_clicked), NULL);
            } else {
                g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
            }

            i++;
        }
    }

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.simplecalculator", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

