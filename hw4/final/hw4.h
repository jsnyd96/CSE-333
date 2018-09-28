#ifndef _HW4_H_
#define _HW4_H_

#include "GameInstance.h"
extern "C" {
    #include <gtk/gtk.h>
}

// Contains the game's state.
typedef struct candy_model_t {
    GameInstance* instance;
    int selectedRow;
    int selectedCol;
    GtkWidget* selectedButton;
    GtkWidget* movesLabel;
    GtkWidget* scoreLabel;
    char movesString[16];
    char scoreString[12];
} *CandyModel;

// Contains the colors for the candies.
typedef enum colors {BLUE, GREEN, ORANGE, PURPLE, RED, YELLOW} Color;

#endif
