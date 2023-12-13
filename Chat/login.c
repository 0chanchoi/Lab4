#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>

#define MAXLINE     1000
#define NAME_LEN    20

char *EXIT_STRING = "exit";

GtkWidget *text_view;
GtkWidget *entry;
GtkWidget *ip_entry;
GtkWidget *port_entry;
GtkWidget *id_entry;
GtkWidget *file_button; // 파일 선택 버튼 추가
int s;
char user_id[NAME_LEN];

void errquit(char *mesg) {
    perror(mesg);
    exit(1);
}

void send_message(GtkWidget *widget, gpointer data) {
    char bufmsg[MAXLINE];
    char bufall[MAXLINE + NAME_LEN];
    const gchar *text;

    text = gtk_entry_get_text(GTK_ENTRY(entry));
    sprintf(bufmsg, "%s\n", text);
    sprintf(bufall, "%s: %s\n", user_id, text);
    write(s, bufall, strlen(bufall));

    gtk_entry_set_text(GTK_ENTRY(entry), "");
}

gboolean receive_message(GIOChannel *source, GIOCondition condition, gpointer data) {
    char bufmsg[MAXLINE];
    int nbyte;

    if (condition & G_IO_IN) {
        nbyte = read(s, bufmsg, MAXLINE);
        bufmsg[nbyte] = 0;

        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, bufmsg, -1);
    }

    return TRUE;
}

int tcp_connect(int af, char *servip, unsigned short port) {
    struct sockaddr_in servaddr;
    int s;

    if ((s = socket(af, SOCK_STREAM, 0)) < 0)
        return -1;

    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = af;
    inet_pton(AF_INET, servip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        return -1;

    return s;
}

void send_file(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        char *filename = gtk_file_chooser_get_filename(chooser);

        FILE *file = fopen(filename, "rb");
        if (file == NULL) {
            g_print("Could not open file for reading: %s\n", filename);
            return;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *file_buffer = malloc(file_size); // 파일 크기만큼의 버퍼 할당
        fread(file_buffer, 1, file_size, file); // 파일을 버퍼에 읽음

        send(s, file_buffer, file_size, 0); // 파일을 서버로 전송

        fclose(file);
        free(file_buffer);
        g_print("File sent: %s\n", filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}


void connect_server(GtkWidget *widget, gpointer data) {
    const gchar *ip_text = gtk_entry_get_text(GTK_ENTRY(ip_entry));
    const gchar *port_text = gtk_entry_get_text(GTK_ENTRY(port_entry));
    const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(id_entry));

    if (strlen(ip_text) == 0 || strlen(port_text) == 0 || strlen(id_text) == 0) {
        printf("Please enter IP, Port, and User ID\n");
        return;
    }

    s = tcp_connect(AF_INET, (char *)ip_text, atoi(port_text));
    if (s == -1)
        errquit("tcp_connect fail");

    strcpy(user_id, (char *)id_text);

    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(widget));
    gtk_widget_hide(window);

    GtkWidget *chat_window;
    GtkWidget *vbox;
    GtkWidget *hbox;

    chat_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(chat_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(chat_window), vbox);

    text_view = gtk_text_view_new();
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0);
    gtk_widget_set_size_request(text_view, 400, 300);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);

    GtkWidget *send_button;
    send_button = gtk_button_new_with_label("Send");
    g_signal_connect(send_button, "clicked", G_CALLBACK(send_message), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), send_button, FALSE, FALSE, 0);

    // 파일 선택 버튼 생성
    file_button = gtk_button_new_with_label("Send File");
    g_signal_connect(file_button, "clicked", G_CALLBACK(send_file), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), file_button, FALSE, FALSE, 0);

    gtk_widget_show_all(chat_window);

    GIOChannel *channel = g_io_channel_unix_new(s);
    g_io_add_watch(channel, G_IO_IN, (GIOFunc)receive_message, NULL);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *connect_button;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    ip_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), ip_entry, FALSE, FALSE, 0);
    gtk_entry_set_text(GTK_ENTRY(ip_entry), "127.0.0.1");

    port_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), port_entry, FALSE, FALSE, 0);
    gtk_entry_set_text(GTK_ENTRY(port_entry), "9999");

    id_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(id_entry), "ID");
    gtk_entry_set_text(GTK_ENTRY(id_entry), "");
    gtk_box_pack_start(GTK_BOX(vbox), id_entry, FALSE, FALSE, 0);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    connect_button = gtk_button_new_with_label("Connect");
    g_signal_connect(connect_button, "clicked", G_CALLBACK(connect_server), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), connect_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

