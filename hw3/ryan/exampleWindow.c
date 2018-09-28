
#include <gtk/gtk.h>
#include <stdlib.h>

char* label1 = "two";
char* label2 = "one";

static void swap(GtkWidget *widget, gpointer data);

static void drawButtons(GtkWidget* buttonGrid);

static void drawButtons(GtkWidget* buttonGrid) {

    printf("Drawing buttons!\n");
    // Remove all currently stored buttons.
    GList* heldButtons = gtk_container_get_children(GTK_CONTAINER (buttonGrid));

    int size = g_list_length(heldButtons);
    printf("The grid contains %i buttons.\n", size);
    GtkWidget* currentButton;
    for (int i = 0; i<size; i++) {
        currentButton = g_list_nth(heldButtons, i)->data;
        //g_list_remove(heldButtons, currentButton);
        //gtk_container_remove(GTK_CONTAINER (buttonGrid), (GtkWidget*)currentButton);
        gtk_widget_destroy(GTK_WIDGET (currentButton));
    }

    char* temp = label1;
    label1 = label2;
    label2 = temp;
    

    GtkWidget *button1;
    GtkWidget *button2;

    button1 = gtk_button_new_with_label(label1);
    gtk_button_set_relief(button1, GTK_RELIEF_NONE);
    g_signal_connect(button1, "clicked", G_CALLBACK (swap), NULL);
    gtk_container_add(GTK_CONTAINER (buttonGrid), button1);

    button2 = gtk_button_new_with_label(label2);
    gtk_button_set_relief(button2, GTK_RELIEF_NONE);
    gtk_container_add(GTK_CONTAINER (buttonGrid), button2);

    gtk_widget_show_all(buttonGrid);
}

static void swap(GtkWidget *widget, gpointer data) {
    printf("Parent is found: %i\n", gtk_widget_get_ancestor(widget, GTK_TYPE_GRID) != 0);
    drawButtons(gtk_widget_get_ancestor(widget, GTK_TYPE_GRID));
}



static void activate (GtkApplication* app, gpointer        user_data)
{
    GtkWidget *window;
//    GtkWidget *button1;
//    GtkWidget *button2;
    GtkWidget *buttonGrid;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    buttonGrid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER (window), buttonGrid);
    
    drawButtons(buttonGrid);

/*    button1 = gtk_button_new_with_label(label1);
    gtk_button_set_relief(button1, GTK_RELIEF_NONE);
    g_signal_connect(button1, "clicked", G_CALLBACK (swap), NULL);
    gtk_container_add(GTK_CONTAINER (buttonGrid), button1);

    button2 = gtk_button_new_with_label(label2);
    gtk_button_set_relief(button2, GTK_RELIEF_NONE);
    gtk_container_add(GTK_CONTAINER (buttonGrid), button2);
*/

    gtk_widget_show_all (window);
}

int main (int    argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
