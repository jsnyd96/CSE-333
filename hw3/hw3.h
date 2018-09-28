#ifndef _HW3_H_
#define _HW3_H_

#include "Array2D.h"

// Contains the game's state.
typedef struct candy_model_t {
    Array2D array;
    int selectedRow;
    int selectedCol;
    int movesLeft;
    GtkWidget* selectedButton;
    GtkWidget* movesLabel;
    char movesLeftString[16];
} *CandyModel;

// Contains the colors for the candies.
typedef enum colors {BLUE, GREEN, ORANGE, PURPLE, RED, YELLOW} Color;

#endif
