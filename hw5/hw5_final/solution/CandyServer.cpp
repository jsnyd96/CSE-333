#include <string>
#include <iostream>

#include "CSE333.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "hw4/GameInstance.h"
extern "C" {
    #include <stdlib.h>
    #include <string.h>
    #include "jansson.h"
    #include <gtk/gtk.h>
}

#define BUF_SIZE 2048

using namespace hw5_net;
using namespace std;


// Contains the game's state and all view information.
typedef struct candy_model_t {
    GameInstance* instance;
    int selectedRow;
    int selectedCol;
    GtkWidget* selectedButton;
    GtkWidget* movesLabel;
    GtkWidget* scoreLabel;
    char movesString[16];
    char scoreString[12];
    ClientSocket socket;
    char* buf;
} *CandyModel;

// Contains the colors for the candies.
typedef enum colors {BLUE, GREEN, ORANGE, PURPLE, RED, YELLOW} Color;



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
        int direction = -1;
        if (strcmp(buttonName, "up") == 0) {
            if (model->selectedRow + 1 < rows) {
                direction = 0;
            }
        } else if (strcmp(buttonName, "down") == 0) {
            if (model->selectedRow > 0) {
                direction = 2;
            }
        } else if (strcmp(buttonName, "left") == 0) {
            if (model->selectedCol > 0) {
                direction = 3;
            }
        } else if (strcmp(buttonName, "right") == 0) {
            if (model->selectedCol + 1 < cols) {
                direction = 1;
            }
        }

        if (direction != -1) {
            // create move json
            json_t *root = json_object();
            json_object_set_new(root, "action", json_string("move"));
            json_object_set_new(root, "row", json_integer(model->selectedRow));
            json_object_set_new(root, "column", json_integer(model->selectedCol));
            json_object_set_new(root, "direction", json_integer(direction));

            // send move json to client
            char *moveDump = json_dumps(root, JSON_ENCODE_ANY);
            string move(moveDump);
            (model->socket).WrappedWrite(move.c_str(), move.length());
            printf("Sent move\n");
            free(moveDump);
            json_decref(root);

            // wait for update response
            int readCount;
            while (!(readCount = (model->socket).WrappedRead(model->buf, 2047)));
            (model->buf)[readCount] = '\0';
            root = json_loads(model->buf, JSON_DECODE_ANY, NULL);
            string action(json_string_value(json_object_get(root, "action")));
            if (action.compare("update") == 0) {
                delete(model->instance);
                model->instance = new GameInstance(json_object_get(root, "gameinstance"));

                // update view
                updateView(model);
                printf("Updated view\n");
            }
            json_decref(root);
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





int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        printf("Usage: ./CandyServer Path PortNumber\n");
        return 1;
    }

    // Set up socket
    int port;
    if (argc == 2) {
        port = 0;
    } else {
        sscanf(argv[2], "%i", &port);
    }
    
    ServerSocket socket(port);
    int socketFD;
    socket.BindAndListen(AF_INET, &socketFD);
    printf("Using port: %i\n", socket.port());
    printf("Process id: %i\n", getpid());

    int acceptedFd;
    string clientAddr;
    uint16_t clientPort;
    string clientDNSName;
    string serverAddress;
    string serverDNSName;

    // Wait for client connections
    while(true) {
        printf("Waiting for connection...\n");

        socket.Accept( &acceptedFd,
            &clientAddr,
            &clientPort,
            &clientDNSName,
            &serverAddress,
            &serverDNSName );

        ClientSocket peerSocket(acceptedFd);

        char buf[BUF_SIZE];
        int readCount;
        
        // Sits here until a message is recieved.
        while (!(readCount = peerSocket.WrappedRead(buf, BUF_SIZE - 1)));
        buf[readCount] = '\0';

        json_t *root = json_loads(buf, JSON_DECODE_ANY, NULL);
        string actionHello(json_string_value(json_object_get(root, "action")));
        if (actionHello.compare("hello") != 0) {
            json_decref(root);
            return 1;
        }
        json_decref(root);
        printf("Received greeting\n");

        // Send 'helloack' message to client.
        root = json_object();
        json_t *action = json_string("helloack");
        json_object_set_new(root, "action", action);
        json_t *gameInstance = json_load_file(argv[1], JSON_DECODE_ANY, NULL);
        json_object_set_new(root, "gameinstance", gameInstance);
        char *helloackDump = json_dumps(root, JSON_ENCODE_ANY);
        string helloack(helloackDump);
        peerSocket.WrappedWrite(helloack.c_str(), helloack.length());
        free(helloackDump);
        json_decref(root);

        printf("Sent helloack\n");

        // Recieve 'gameinstance', create window for the first time and update the view.
        while (!(readCount = peerSocket.WrappedRead(buf, BUF_SIZE - 1)));
        buf[readCount] = '\0';
        root = json_loads(buf, JSON_DECODE_ANY, NULL);

        string initialUpdate(json_string_value(json_object_get(root, "action")));
        if (initialUpdate.compare("update") != 0) {
            json_decref(root);
            return 1;
        }
        printf("Received initial update\n");

        CandyModel model = (CandyModel)malloc(sizeof(struct candy_model_t));
        model->instance = new GameInstance(json_object_get(root, "gameinstance"));
        json_decref(root);

        model->selectedRow = -1;
        model->selectedCol = -1;
        model->selectedButton = NULL;
        model->movesLabel = NULL;
        model->scoreLabel = NULL;
        sprintf(model->movesString, "%i moves made", (*model->instance).getMovesMade());
        sprintf(model->scoreString, "Score: %i", (*model->instance).getScore());

        model->socket = peerSocket;
        model->buf = buf;
        
        // Have buttons call functions that send messages to the server, and then update
        // based on returned 'gameinstance' message.

        printf("Generating view\n");
        gtk_init(0, NULL);
        buildWindow(model);
        gtk_main();

        delete(model->instance);
        free(model);

        // On exit, send 'bye' message and close() socket.

        root = json_object();
        json_object_set_new(root, "action", json_string("bye"));
        char *byeDump = json_dumps(root, JSON_ENCODE_ANY);
        string byeString(byeDump);
        peerSocket.WrappedWrite(byeString.c_str(), byeString.length());
        printf("Sent bye message\n");
        free(byeDump);
        json_decref(root);

        close(peerSocket.getAsFileDescriptor());
        printf("Connection closed\n");
    }

    return 0;
}
