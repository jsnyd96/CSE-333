#ifndef GAME_INSTANCE_H
#define GAME_INSTANCE_H

#include <string>
#include "Array2DCallbacks.h"
extern "C" {
    #include "Array2D.h"
    #include "../jansson/include/jansson.h"
}

using namespace std;

class GameInstance
{
private:
    int gameID;
    int movesAllowed;
    int colorCount;
    Array2D extension;
    Array2D initialBoardState;
    Array2D boardState;
    Array2D boardCandies;
    int movesMade;
    int score;
    int* extensionOffset;

    // Checks the current board for the given template, leaving behind empty spaces.
    // Returns the number of templates fired.
    int fireTemplate(int up, int right);

    // Checks the current board for all templates and fires them by priority, 
    // leaving empty spaces ready to be filled by settling. Returns the number of templates fired.
    int fireTemplates();

    // Fills in the board from the top with new integers, checks for templates, and fires
    // them until the board is stable.
    void settle();

    // Helper function retrieve an object from parent.
    // Throws invalid_argument if name is not an object in parent.
    json_t* getJsonObject(json_t *parent, string name);



    // Creates a new array from the json object.
    // Throws invalid_argument if jsonArray is invalid.
    Array2D makeArray(json_t* jsonArray, APayloadDeserializeFnPtr deserializer);

    // Turns a Array2D into a jannson object contain the row count, column count, and
    // elements.
    // Throws invalid argument exception if array or serializer is NULL.
    json_t* serializeArray(Array2D array, APayloadSerializeFnPtr serializer);

    // Generates gamestate from gamedef fields.
    void generateGameState();

public:
    // Returns the candy stored at location (x, y).
    int getCandy(int x, int y) { return ((BoardSpace)GetElementArray2D(boardCandies, x, y))->candy; };

    // Returns the board state of location (x, y).
    int getState(int x, int y) { return (*(int *)GetElementArray2D(boardState, x, y)); };

    // Returns the number of moves made.
    int getMovesMade() { return movesMade; };

    // Returns the score.
    int getScore() { return score; };

    // Returns the number of rows.
    int getRowCount() { return GetRowCountArray2D(boardCandies); };

    // Returns the number of columns.
    int getColCount() { return GetColumnCountArray2D(boardCandies); };

    // Swaps the candy located at location (x, y) with the candy found one space
    // away in the given direction. The board is then updated for all the resulting
    // template firings. If no template is fired, the swap is reverted and false is returned.
    bool swap(int x, int y, int dir);

    // Saves the entire initial and current game state to a json file.
    // Throws an illegal argument exception if path is null or "".
    json_t* save();

    // Loads the entire initial and current game state from a json file.
    // Requires path to be a valid candy crush json file.
    GameInstance(json_t *instance);

    // Creates a copy of original.
    GameInstance(const GameInstance &original);

    // Destructor
    ~GameInstance();
};

#endif
