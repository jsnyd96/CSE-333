#ifndef _CANDYMODEL_H_
#define _CANDYMODEL_H_

#include <stdbool.h>

// Functions for modeling the candy crush game.

// Contains the entire state of a game of candy crush.
// Hides fields from user.
struct c_state;
typedef struct c_state* CandyState;

// Creates a new state of candy crush with moves moves and a board populated by
// path's data.
// Returns null if path leads to an invalid file.
CandyState allocateModel(int moves, char* path);

// Swaps the candies found at (x1, y1) and (x2, y2), if the move is legal.
// Decrements the number of remaining moves by 1.
// Otherwise, the board remains unchanged.
// Redraws board?
// Return true if the move was legal, false if not.
bool swapCandiesModel(CandyState state, int x1, int y1, int x2, int y2);

// Returns the number signifying the candy in this location.
// Returns -1 if (x, y) is out of bounds.
int getCandyModel(CandyState state, int x, int y);

// Returns the number of rows in the board in state, -1 if state == null.
int getRowsModel(CandyState state);

// Returns the number of columns in the board in state, -1 if state == null.
int getColumnsModel(CandyState state);

// Returns the number of moves remaining in state.
// Requires that state != null.
int getMovesModel(CandyState state);

#endif
