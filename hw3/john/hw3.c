#include <gtk/gtk.h>
#include "Array2D.h"

struct candy_model_t {
    Array2D array;
    int selectedRow;
    int selectedCol;
    int movesLeft;
    char movesLeftString[16];
};
typedef struct candy_model_t *CandyModel;

void fillBoardGrid(GtkWidget *boardGrid, CandyModel model) { 
    for (int i = 0; i < GetRowCountArray2D(model->array); i++) {
        for (int j = 0; j < GetColumnCountArray2D(model->array); j++) {
            GtkWidget *candyButton = gtk_button_new();
            GtkWidget *candyImage;
            int candy = *(int *)GetElementArray2D(model->array, i, j);
            if (candy == 1) {
                candyImage = gtk_image_new_from_file("images/green.png");
            } else if (candy == 2) {
                candyImage = gtk_image_new_from_file("images/orange.png");
            } else if (candy == 3) {
                candyImage = gtk_image_new_from_file("images/purple.png");
            } else if (candy == 4) {
                candyImage = gtk_image_new_from_file("images/red.png");
            } else if (candy == 5) {
                candyImage = gtk_image_new_from_file("images/yellow.png");
            } else {
                candyImage = gtk_image_new_from_file("images/blue.png");
            }
            gtk_button_set_image((GtkButton *)candyButton, candyImage);
            gtk_button_set_relief((GtkButton *)candyButton, GTK_RELIEF_NONE);
            gtk_grid_attach(GTK_GRID(boardGrid), candyButton, j, i, 1, 1);
        }
    }
}

void buildWindow(GtkWidget *window, CandyModel model) {
    gtk_window_set_title(GTK_WINDOW(window), "CSE333 Candy Crush");

    GtkWidget *windowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(window), windowBox);

    GtkWidget *boardGrid = gtk_grid_new();
    fillBoardGrid(boardGrid, model);
    gtk_container_add(GTK_CONTAINER(windowBox), boardGrid);

    GtkWidget *controlBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(windowBox), controlBox);

    GtkWidget *labelMoves = gtk_label_new("30 moves left");
    gtk_container_add(GTK_CONTAINER(controlBox), labelMoves);

    GtkWidget *buttonBox = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_container_add(GTK_CONTAINER(controlBox), buttonBox);

    GtkWidget *buttonLeft = gtk_button_new();
    GtkWidget *imageLeft = gtk_image_new_from_file("images/left.png");
    gtk_button_set_image((GtkButton *)buttonLeft, imageLeft);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonLeft);

    GtkWidget *buttonRight = gtk_button_new();
    GtkWidget *imageRight = gtk_image_new_from_file("images/right.png");
    gtk_button_set_image((GtkButton *)buttonRight, imageRight);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonRight);

    GtkWidget *buttonUp = gtk_button_new();
    GtkWidget *imageUp = gtk_image_new_from_file("images/up.png");
    gtk_button_set_image((GtkButton *)buttonUp, imageUp);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonUp);

    GtkWidget *buttonDown = gtk_button_new();
    GtkWidget *imageDown = gtk_image_new_from_file("images/down.png");
    gtk_button_set_image((GtkButton *)buttonDown, imageDown);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonDown);

    gtk_widget_show_all(window);
}

APayload_t Deserializer(char* json) {
    int* newPayload = (int*)malloc(sizeof(int));
    char *ptr;
    *newPayload = (int)strtol(json, &ptr, 10); // Copy i to malloced memory.
    return (APayload_t)newPayload;
}

void PayloadFree(APayload_t payload) {
    free(payload);
}

int main (int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid call: .hw3 path\n");
        return -1;
    }

    CandyModel model = (CandyModel)malloc(sizeof(struct candy_model_t));
    model->array = AllocateArray2DFile(argv[1], &Deserializer);
    model->selectedRow = -1;
    model->selectedCol = -1;
    model->movesLeft = 30;

    int a = 1;
    gtk_init(&a, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    buildWindow(window, model);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_main();

    FreeArray2D(model->array, &PayloadFree);
    free(model);

    return 0;
}
