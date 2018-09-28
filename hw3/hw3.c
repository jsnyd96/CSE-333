#include <gtk/gtk.h>
#include <string.h>
#include "Array2D.h"
#include "hw3.h"
#include "hw3Array2DCallbacks.h"

// Updates selected coordinates when a button is pressed
void candyButtonPressed(GtkButton* button, gpointer data) {
    CandyModel model = (CandyModel)data;
    if (model->selectedButton != NULL) {
        gtk_button_set_relief((GtkButton*) model->selectedButton, GTK_RELIEF_NONE);
    }
    sscanf(gtk_widget_get_name(GTK_WIDGET (button)), "%i %i",
        &model->selectedRow, &model->selectedCol);
    model->selectedButton = GTK_WIDGET (button);
    gtk_button_set_relief((GtkButton*) model->selectedButton, GTK_RELIEF_NORMAL);
}

// Removes all buttons from the given container.
void emptyBoardGrid(GtkWidget *boardGrid) {
    GList* buttons = gtk_container_get_children(GTK_CONTAINER (boardGrid));
    int length = g_list_length(buttons);

    for (int i = 0; i < length; i++) {
        GtkWidget* button = g_list_nth_data(buttons, i);
        gtk_container_remove(GTK_CONTAINER (boardGrid), button);
    }

    g_list_free(buttons);
}

// Fills boardGrid with colored buttons based on model.
void fillBoardGrid(GtkWidget *boardGrid, CandyModel model) {
    for (int i = 0; i < GetRowCountArray2D(model->array); i++) {
        for (int j = 0; j < GetColumnCountArray2D(model->array); j++) {
            GtkWidget *candyButton = gtk_button_new();
            GtkWidget *candyImage;
            Color candy = *(int *)GetElementArray2D(model->array, i, j);
            switch (candy) {
                case GREEN:
                    candyImage = gtk_image_new_from_file("images/green.png");
                break;
                case ORANGE:
                    candyImage = gtk_image_new_from_file("images/orange.png");
                break;
                case PURPLE:
                    candyImage = gtk_image_new_from_file("images/purple.png");
                break;
                case RED:
                    candyImage = gtk_image_new_from_file("images/red.png");
                break;
                case YELLOW:
                    candyImage = gtk_image_new_from_file("images/yellow.png");
                break;
                case BLUE:
                    candyImage = gtk_image_new_from_file("images/blue.png");
                break;
            }

            char loc[10];
            sprintf(loc, "%i %i", i, j);
            gtk_widget_set_name(candyButton, loc);
            gtk_button_set_image((GtkButton *)candyButton, candyImage);
            gtk_button_set_relief((GtkButton *)candyButton, GTK_RELIEF_NONE);
            g_signal_connect(candyButton, "clicked", 
                G_CALLBACK (candyButtonPressed), model);
            gtk_grid_attach(GTK_GRID(boardGrid), candyButton, j, i, 1, 1);
        }
    }

    gtk_widget_show_all(boardGrid);
}

// Updates game after a move.
void updateGame(CandyModel model) {
    model->movesLeft--;
    sprintf(model->movesLeftString, "%i", model->movesLeft);
    strcat(model->movesLeftString, " moves left");

    // Update view.
    GtkWidget *grid = gtk_widget_get_parent(model->selectedButton);
    emptyBoardGrid(grid);
    fillBoardGrid(grid, model);

    model->selectedButton = NULL;
    model->selectedRow = -1;
    model->selectedCol = -1;
}

// Updates the model and view when a move is made.
void arrowButtonPressed(GtkButton* button, gpointer data) {
    CandyModel model = (CandyModel)data;
    int rows = GetRowCountArray2D(model->array);
    int cols = GetColumnCountArray2D(model->array);

    // Checks if the move is valid.
    if (model->selectedRow >= 0 && 
        model->selectedRow < rows &&
        model->selectedCol >= 0 &&
        model->selectedCol < cols) {
        // Check if destination is valid.
        const char* buttonName = gtk_widget_get_name(GTK_WIDGET (button));
        if (strcmp(buttonName, "up") == 0) {
            if (model->selectedRow - 1 >= 0) {
                SwapElementsArray2D(model->array, model->selectedRow,
                    model->selectedCol, model->selectedRow - 1,
                    model->selectedCol);
                updateGame(model);
            }
        } else if (strcmp(buttonName, "down") == 0) {
            if (model->selectedRow + 1 < rows) {
                SwapElementsArray2D(model->array, model->selectedRow,
                    model->selectedCol, model->selectedRow + 1,
                    model->selectedCol);
                updateGame(model);
            }
        } else if (strcmp(buttonName, "left") == 0) {
            if (model->selectedCol - 1 >= 0) {
                SwapElementsArray2D(model->array, model->selectedRow,
                    model->selectedCol, model->selectedRow,
                    model->selectedCol - 1);
                updateGame(model);
            }
        } else if (strcmp(buttonName, "right") == 0) {
            if (model->selectedCol + 1 < cols) {
                SwapElementsArray2D(model->array, model->selectedRow,
                    model->selectedCol, model->selectedRow,
                    model->selectedCol + 1);
                updateGame(model);
            }
        }

        // Update the label.
        gtk_label_set_text((GtkLabel*)model->movesLabel, model->movesLeftString);
    } else {
        printf("No button selected.\n");
    }
}

// Creates and fills window.
void buildWindow(CandyModel model) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "CSE333 Candy Crush");

    GtkWidget *windowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(window), windowBox);

    GtkWidget *boardGrid = gtk_grid_new();
    fillBoardGrid(boardGrid, model);
    gtk_container_add(GTK_CONTAINER(windowBox), boardGrid);

    GtkWidget *controlBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(windowBox), controlBox);

    model->movesLabel = gtk_label_new(model->movesLeftString);
    gtk_container_add(GTK_CONTAINER(controlBox), model->movesLabel);

    GtkWidget *buttonBox = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_container_add(GTK_CONTAINER(controlBox), buttonBox);

    GtkWidget *buttonLeft = gtk_button_new();
    GtkWidget *imageLeft = gtk_image_new_from_file("images/left.png");
    gtk_button_set_image((GtkButton *)buttonLeft, imageLeft);
    gtk_widget_set_name(buttonLeft, "left");
    g_signal_connect(buttonLeft, "clicked", G_CALLBACK (arrowButtonPressed), model);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonLeft);

    GtkWidget *buttonRight = gtk_button_new();
    GtkWidget *imageRight = gtk_image_new_from_file("images/right.png");
    gtk_button_set_image((GtkButton *)buttonRight, imageRight);
    gtk_widget_set_name(buttonRight, "right");
    g_signal_connect(buttonRight, "clicked", G_CALLBACK (arrowButtonPressed), model);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonRight);

    GtkWidget *buttonUp = gtk_button_new();
    GtkWidget *imageUp = gtk_image_new_from_file("images/up.png");
    gtk_button_set_image((GtkButton *)buttonUp, imageUp);
    gtk_widget_set_name(buttonUp, "up");
    g_signal_connect(buttonUp, "clicked", G_CALLBACK (arrowButtonPressed), model);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonUp);

    GtkWidget *buttonDown = gtk_button_new();
    GtkWidget *imageDown = gtk_image_new_from_file("images/down.png");
    gtk_button_set_image((GtkButton *)buttonDown, imageDown);
    gtk_widget_set_name(buttonDown, "down");
    g_signal_connect(buttonDown, "clicked", G_CALLBACK (arrowButtonPressed), model);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonDown);

    gtk_widget_show_all(window);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

int main (int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid call: ./hw3 path\n");
        return -1;
    }

    CandyModel model = (CandyModel)malloc(sizeof(struct candy_model_t));
    model->array = AllocateArray2DFile(argv[1], &Deserializer);
    model->selectedRow = -1;
    model->selectedCol = -1;
    model->movesLeft = 30;
    model->movesLabel = NULL;
    model->selectedButton = NULL;
    strcpy(model->movesLeftString, "30 moves left");

    gtk_init(0, NULL);
    buildWindow(model);
    gtk_main();

    FreeArray2D(model->array, &PayloadFree);
    free(model);

    return 0;
}
