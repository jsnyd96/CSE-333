#include <string>
#include "hw4.h"
extern "C" {
    #include <stdlib.h>
    #include <string.h>
}


// Updates selected coordinates when a button is pressed.
void candyButtonPressed(GtkButton* button, gpointer data) {
    CandyModel model = (CandyModel)data;
    if (model->selectedButton != NULL) {
        gtk_button_set_relief((GtkButton*) model->selectedButton, GTK_RELIEF_NONE);
    }
    // Store selected location
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
        GtkWidget* button = GTK_WIDGET(g_list_nth_data(buttons, i));
        gtk_container_remove(GTK_CONTAINER (boardGrid), button);
    }

    g_list_free(buttons);
}

// Fills boardGrid with colored buttons based on model.
void fillBoardGrid(GtkWidget *boardGrid, CandyModel model) {
    int rows = (*model->instance).getRowCount();
    int cols = (*model->instance).getColCount();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            GtkWidget *candyButton = gtk_button_new();
            GtkWidget *candyImage;

            // Get color and state
            Color candy = (Color)((*model->instance).getCandy(rows - 1 - i, j));
            int state = (*model->instance).getState(rows - 1 - i, j);

            switch (candy) {
                case GREEN:
                    if (state) candyImage = gtk_image_new_from_file("images/regular/state1/green.png");
                    else       candyImage = gtk_image_new_from_file("images/regular/state0/green.png");
                break;
                case ORANGE:
                    if (state) candyImage = gtk_image_new_from_file("images/regular/state1/orange.png");
                    else       candyImage = gtk_image_new_from_file("images/regular/state0/orange.png");
                break;
                case PURPLE:
                    if (state) candyImage = gtk_image_new_from_file("images/regular/state1/purple.png");
                    else       candyImage = gtk_image_new_from_file("images/regular/state0/purple.png");
                break;
                case RED:
                    if (state) candyImage = gtk_image_new_from_file("images/regular/state1/red.png");
                    else       candyImage = gtk_image_new_from_file("images/regular/state0/red.png");
                break;
                case YELLOW:
                    if (state) candyImage = gtk_image_new_from_file("images/regular/state1/yellow.png");
                    else       candyImage = gtk_image_new_from_file("images/regular/state0/yellow.png");
                break;
                case BLUE:
                    if (state) candyImage = gtk_image_new_from_file("images/regular/state1/blue.png");
                    else       candyImage = gtk_image_new_from_file("images/regular/state0/blue.png");
                break;
            }

            char loc[10];
            sprintf(loc, "%i %i", rows - 1 - i, j);
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

// Updates view after a move.
void updateView(CandyModel model) {
    sprintf(model->movesString, "%i moves made", (*model->instance).getMovesMade());
    sprintf(model->scoreString, "Score: %i", (*model->instance).getScore());

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
    int rows = (*model->instance).getRowCount();
    int cols = (*model->instance).getColCount();

    // Checks if the move is valid.
    if (model->selectedRow >= 0 && 
        model->selectedRow < rows &&
        model->selectedCol >= 0 &&
        model->selectedCol < cols) {
        // Check if destination is valid.
        const char* buttonName = gtk_widget_get_name(GTK_WIDGET (button));
        if (strcmp(buttonName, "up") == 0) {
            if (model->selectedRow + 1 < rows) {
                if ((*model->instance).swap(model->selectedRow, model->selectedCol, 0))
                    updateView(model);
            }
        } else if (strcmp(buttonName, "down") == 0) {
            if (model->selectedRow > 0) {
                if ((*model->instance).swap(model->selectedRow, model->selectedCol, 2))
                    updateView(model);
            }
        } else if (strcmp(buttonName, "left") == 0) {
            if (model->selectedCol > 0) {
                if ((*model->instance).swap(model->selectedRow, model->selectedCol, 3))
                    updateView(model);
            }
        } else if (strcmp(buttonName, "right") == 0) {
            if (model->selectedCol + 1 < cols) {
                if ((*model->instance).swap(model->selectedRow, model->selectedCol, 1))
                    updateView(model);
            }
        }

        // Update the labels.
        gtk_label_set_text((GtkLabel*)model->movesLabel, model->movesString);
        gtk_label_set_text((GtkLabel*)model->scoreLabel, model->scoreString);
    } else {
        printf("No button selected.\n");
    }
}

// Creates and fills window.
void buildWindow(CandyModel model) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "CSE333 Candy Crush");

    GtkWidget *windowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), windowBox);

    GtkWidget *boardGrid = gtk_grid_new();
    fillBoardGrid(boardGrid, model);
    gtk_container_add(GTK_CONTAINER(windowBox), boardGrid);

    GtkWidget *controlBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(windowBox), controlBox);

    model->movesLabel = gtk_label_new(model->movesString);
    gtk_container_add(GTK_CONTAINER(controlBox), model->movesLabel);
    model->scoreLabel = gtk_label_new(model->scoreString);
    gtk_container_add(GTK_CONTAINER(controlBox), model->scoreLabel);

    GtkWidget *buttonBox = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_container_add(GTK_CONTAINER(controlBox), buttonBox);

    GtkWidget *buttonLeft = gtk_button_new();
    GtkWidget *imageLeft = gtk_image_new_from_file("images/direction/left.png");
    gtk_button_set_image((GtkButton *)buttonLeft, imageLeft);
    gtk_widget_set_name(buttonLeft, "left");
    g_signal_connect(buttonLeft, "clicked", G_CALLBACK (arrowButtonPressed), model);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonLeft);

    GtkWidget *buttonRight = gtk_button_new();
    GtkWidget *imageRight = gtk_image_new_from_file("images/direction/right.png");
    gtk_button_set_image((GtkButton *)buttonRight, imageRight);
    gtk_widget_set_name(buttonRight, "right");
    g_signal_connect(buttonRight, "clicked", G_CALLBACK (arrowButtonPressed), model);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonRight);

    GtkWidget *buttonUp = gtk_button_new();
    GtkWidget *imageUp = gtk_image_new_from_file("images/direction/up.png");
    gtk_button_set_image((GtkButton *)buttonUp, imageUp);
    gtk_widget_set_name(buttonUp, "up");
    g_signal_connect(buttonUp, "clicked", G_CALLBACK (arrowButtonPressed), model);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonUp);

    GtkWidget *buttonDown = gtk_button_new();
    GtkWidget *imageDown = gtk_image_new_from_file("images/direction/down.png");
    gtk_button_set_image((GtkButton *)buttonDown, imageDown);
    gtk_widget_set_name(buttonDown, "down");
    g_signal_connect(buttonDown, "clicked", G_CALLBACK (arrowButtonPressed), model);
    gtk_container_add(GTK_CONTAINER(buttonBox), buttonDown);

    gtk_widget_show_all(window);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

int main (int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid call: ./hw4 path\n");
        return -1;
    }

    CandyModel model = (CandyModel)malloc(sizeof(struct candy_model_t));
    model->instance = new GameInstance(*(new string(argv[1])));
    model->selectedRow = -1;
    model->selectedCol = -1;
    model->selectedButton = NULL;
    model->movesLabel = NULL;
    model->scoreLabel = NULL;
    sprintf(model->movesString, "%i moves made", (*model->instance).getMovesMade());
    sprintf(model->scoreString, "Score: %i", (*model->instance).getScore());

    gtk_init(0, NULL);
    buildWindow(model);
    gtk_main();

    printf("Writing test.out\n");
    model->instance->save("./test.out");
    delete(model->instance);
    free(model);

    return 0;
}
