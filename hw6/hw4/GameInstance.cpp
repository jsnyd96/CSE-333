#include <string>
#include <stdexcept>
#include <iostream>

#include "GameInstance.h"
extern "C"{
    #include "Array2D_priv.h"
    #include "../jansson/include/jansson.h"
}

GameInstance::GameInstance(json_t *instance) {
    if (instance == NULL) {
        throw invalid_argument("Instance not found.");
    }

    // Read guaranteed values of valid file.
    json_t *definition = NULL;
    json_t *gameIDObject = NULL;
    json_t *extensionObject = NULL;
    json_t *initialBoardStateObject = NULL;
    json_t *movesAllowedObject = NULL;
    json_t *colorCountObject = NULL;

    try {
        definition = getJsonObject(instance, "gamedef");
        gameIDObject = getJsonObject(definition, "gameid");
        extensionObject = getJsonObject(definition, "extensioncolor");
        initialBoardStateObject = getJsonObject(definition, "boardstate");
        movesAllowedObject = getJsonObject(definition, "movesallowed");
        colorCountObject = getJsonObject(definition, "colors");
    } catch (invalid_argument e) {
        //json_decref(root);
        throw e;
    }

    // Using json objects to initialize fields.
    gameID = json_integer_value(gameIDObject);
    movesAllowed = json_integer_value(movesAllowedObject);
    colorCount = json_integer_value(colorCountObject);

    extension = makeArray(extensionObject, &Array2DDeserializeInt);
    initialBoardState = makeArray(initialBoardStateObject, &Array2DDeserializeInt);

    // Proceeding to gamestate values, which may or may not be there.
    json_t *state = NULL;
    try {
        state = getJsonObject(instance, "gamestate");
    } catch (invalid_argument e) {
        // Generate gamestate from definition.
        generateGameState();
        
        //json_decref(root);
        return;
    }
    
    // Gamestate is in file, copying values over.
    json_t *boardStateObject = NULL;
    json_t *boardCandiesObject = NULL;
    json_t *movesMadeObject = NULL;
    json_t *scoreObject = NULL;
    json_t *extensionOffsetObject = NULL;

    try {
        boardStateObject = getJsonObject(state, "boardstate");
        boardCandiesObject = getJsonObject(state, "boardcandies");
        movesMadeObject = getJsonObject(state, "movesmade");
        scoreObject = getJsonObject(state, "currentscore");
        extensionOffsetObject = getJsonObject(state, "extensionoffset");
    } catch (invalid_argument e) {
        //json_decref(root);
        throw e;
    }

    movesMade = json_integer_value(movesMadeObject);
    score = json_integer_value(scoreObject);
    
    // Copy over extension array.
    int extensionSize = json_array_size(extensionOffsetObject);
    extensionOffset = new int[extensionSize];
    for (int i = 0; i < extensionSize; i++) {
        extensionOffset[i] = json_integer_value(json_array_get(extensionOffsetObject, i));
    }

    boardState = makeArray(boardStateObject, &Array2DDeserializeInt);
    boardCandies = makeArray(boardCandiesObject, &Array2DDeserializeBoardSpace);

    //json_decref(root);
    return;
}

GameInstance::GameInstance(const GameInstance &original) {
	// Ints copied by value.
	this->gameID = original.gameID;
	this->movesAllowed = original.movesAllowed;
	this->colorCount = original.colorCount;
	this->movesMade = original.movesMade;
	this->score = original.score;
	
	// Copy over elements in the arrays.
    int rows = GetRowCountArray2D(original.boardState);
    int cols = GetColumnCountArray2D(original.boardState);
    int extensionRows = GetRowCountArray2D(original.extension);

    this->extensionOffset = new int[cols];
    for (int i = 0; i < cols; i++) {
	    // Same col count as boardState.
	    this->extensionOffset[i] = original.extensionOffset[i];
    }

    this->boardState = AllocateArray2D(rows, cols);
    this->boardCandies = AllocateArray2D(rows, cols);
    this->initialBoardState = AllocateArray2D(rows, cols);
    for (int i = 0; i < rows; i++) {
	    for (int j = 0; j < cols; j++) {
		    // boardState and boardCandies have the same dimensions.
		    int* newState = (int*)malloc(sizeof(int));
		    *newState = *((int*)GetElementArray2D(original.boardState, i, j));
		    SetElementArray2D(this->boardState, i, j, newState);

		    BoardSpace* newCandy = (BoardSpace*)malloc(sizeof(BoardSpace));
		    *newCandy = *((BoardSpace*)GetElementArray2D(original.boardCandies, i, j));
		    SetElementArray2D(this->boardCandies, i, j, newCandy);

            int* newInitial = (int*)malloc(sizeof(int));
		    *newInitial = *((int*)GetElementArray2D(original.initialBoardState, i, j));
		    SetElementArray2D(this->initialBoardState, i, j, newInitial);
	    }
    }

    this->extension = AllocateArray2D(extensionRows, cols);
    for (int i = 0; i < extensionRows; i++) {
        for (int j = 0; j < cols; j++) {
            int* newExtensionCandy = (int*)malloc(sizeof(int));
		    *newExtensionCandy = *((int*)GetElementArray2D(original.extension, i, j));
		    SetElementArray2D(this->extension, i, j, newExtensionCandy);
        }
    }
}

int GameInstance::fireTemplate(int up, int right) {
    int count = 0;
    for (int i = 0; i <= getRowCount() - up; i++) {
        for (int j = 0; j <= getColCount() - right; j++) {
            int candy = ((BoardSpace)GetElementArray2D(boardCandies, i, j))->candy;
            // Check candies above
            for (int k = 1; k < up; k++) {
                if (((BoardSpace)GetElementArray2D(boardCandies, i+k, j))->candy != candy) {
                    candy = -1;
                    break;
                }
            }
            // Check candies to the right
            for (int k = 1; k < right; k++) {
                if (((BoardSpace)GetElementArray2D(boardCandies, i, j+k))->candy != candy) {
                    candy = -1;
                    break;
                }
            }
            // Skip if no match or already empty
            if (candy != -1) {
                count++;
                // Set squares above to empty and remove jelly
                for (int k = 0; k < up; k++) {
                    ((BoardSpace)GetElementArray2D(boardCandies, i+k, j))->candy = -1;
                    int* state = (int*)GetElementArray2D(boardState, i+k, j);
                    if (*state > 0) {
                        (*state)--;
                        score++;
                    }
                }
                // Set squares to the right to empty and remove jelly
                for (int k = 1; k < right; k++) {
                    ((BoardSpace)GetElementArray2D(boardCandies, i, j+k))->candy = -1;
                    int* state = (int*)GetElementArray2D(boardState, i, j+k);
                    if (*state > 0) {
                        (*state)--;
                        score++;
                    }
                }
            }
        }
    }
    return count;
}

int GameInstance::fireTemplates() {
    int count = 0;
    count += fireTemplate(4, 1);
    count += fireTemplate(1, 4);
    count += fireTemplate(3, 1);
    count += fireTemplate(1, 3);
    return count;
}

void GameInstance::settle() {
    // Avoids the chance of an infinite loop of settling.
    for (int j = 0; j < 999; j++) {
        // Move all filled spaces down.
        for (int x = 0; x < getRowCount() - 1; x++) {
            for (int y = 0; y < getColCount(); y++) {
                if (getCandy(x, y) == -1) {
                    // Swap empty space with the nearest filled space above.
                    for (int i = x + 1; i < getRowCount(); i++) {
                        if (getCandy(i, y) != -1) {
                            SwapElementsArray2D(boardCandies, x, y, i, y);
                            break;
                        }
                    }
                }
            }
        }

        // Fill boardCandies from extension.
        for (int x = 0; x < getRowCount(); x++) {
            for (int y = 0; y < getColCount(); y++) {
                if (getCandy(x, y) == -1) {
                    int newCandy = *((int*)GetElementArray2D(extension, 
                        extensionOffset[y]%GetRowCountArray2D(extension), y));
                    extensionOffset[y]++;

                    ((BoardSpace)GetElementArray2D(boardCandies, x, y))->candy = newCandy;
                }
            }
        }

        
        if (fireTemplates() == 0) {
            break;
        }
    }
}

bool GameInstance::swap(int x, int y, int dir) {
    if (dir == 3) {
        // Swap up
        if (x >= 0 && x < getRowCount()-1 && y >= 0 && y < getRowCount()) {
           SwapElementsArray2D(boardCandies, x, y, x+1, y);
           if (fireTemplates() == 0) {
               SwapElementsArray2D(boardCandies, x, y, x+1, y);
               return false;
           } else {
               movesMade++;
               settle();
               return true;
           }
       }
    } else if (dir == 1) {
       // Swap right
       if (x >= 0 && x < getRowCount() && y >= 0 && y < getColCount()-1) {
           SwapElementsArray2D(boardCandies, x, y, x, y+1);
           if (fireTemplates() == 0) {
               SwapElementsArray2D(boardCandies, x, y, x, y+1);
               return false;
           } else {
               movesMade++;
               settle();
               return true;
           }
       }
    } else if (dir == 2) {
       // Swap down
       if (x > 0 && x < getRowCount() && y >= 0 && y < getColCount()) {
           SwapElementsArray2D(boardCandies, x, y, x-1, y);
           if (fireTemplates() == 0) {
               SwapElementsArray2D(boardCandies, x, y, x-1, y);
               return false;
           } else {
               movesMade++;
               settle();
               return true;
           }
       }
    } else if (dir == 0) {
        // Swap left
        if (x >= 0 && x < getRowCount() && y > 0 && y < getColCount()) {
           SwapElementsArray2D(boardCandies, x, y, x, y-1);
           if (fireTemplates() == 0) {
               SwapElementsArray2D(boardCandies, x, y, x, y-1);
               return false;
           } else {
               movesMade++;
               settle();
               return true;
           }
        }
    }
    return false;
}

json_t* GameInstance::save() {
    json_t *root = json_object();
    if (root == NULL) {
        throw "Json object failed to make.";
    }

    // Definition part of root.
    json_t *definitionJson = json_object();
    if (definitionJson == NULL) {
        json_decref(root);
        throw "Json object failed to make.";
    }
    
    json_t *extensionJson = NULL;
    try {
        extensionJson = serializeArray(extension, &Array2DSerializeInt);
    } catch (exception e) {
        json_decref(root);
        json_decref(definitionJson);
        throw e;
    }
    json_object_set_new(definitionJson, "extensioncolor", extensionJson);
    
    json_t *initialBoardStateJson = NULL;
    try {
        initialBoardStateJson = serializeArray(initialBoardState, &Array2DSerializeInt);
    } catch (exception e) {
        json_decref(root);
        json_decref(definitionJson);
        throw e;
    }
    json_object_set_new(definitionJson, "boardstate", initialBoardStateJson);

    json_t *movesAllowedJson = json_integer(movesAllowed);
    if (movesAllowedJson == NULL) {
        json_decref(root);
        json_decref(definitionJson);
        throw "Json object failed to make.";
    }
    json_object_set_new(definitionJson, "movesallowed", movesAllowedJson);

    json_t *gameIDJson = json_integer(gameID);
    if (gameIDJson == NULL) {
        json_decref(root);
        json_decref(definitionJson);
        throw "Json object failed to make.";
    }
    json_object_set_new(definitionJson, "gameid", gameIDJson);

    json_t *colorCountJson = json_integer(colorCount);
    if (colorCountJson == NULL) {
        json_decref(root);
        json_decref(definitionJson);
        throw "Json object failed to make.";
    }
    json_object_set_new(definitionJson, "colors", colorCountJson);

    json_object_set_new(root, "gamedef", definitionJson);

    // Now the game state part of root.
    json_t *stateJson = json_object();
    if (stateJson == NULL) {
        json_decref(root);
        throw "Json object failed to make.";
    }

    json_t *movesMadeJson = json_integer(movesMade);
    if (movesMadeJson == NULL) {
        json_decref(root);
        json_decref(stateJson);
        throw "Json object failed to make.";
    }
    json_object_set_new(stateJson, "movesmade", movesMadeJson);

    json_t *scoreJson = json_integer(score);
    if (scoreJson == NULL) {
        json_decref(root);
        json_decref(stateJson);
        throw "Json object failed to make.";
    }
    json_object_set_new(stateJson, "currentscore", scoreJson);

    json_t *extensionOffsetJson = json_array();
    if (extensionOffsetJson == NULL) {
        json_decref(root);
        json_decref(stateJson);
        throw "Json object failed to make.";
    }
    for (int i = 0; i < GetColumnCountArray2D(extension); i++) {
        json_array_append_new(extensionOffsetJson, json_integer(extensionOffset[i]));
    } 
    json_object_set_new(stateJson, "extensionoffset", extensionOffsetJson);

    json_t *boardCandiesJson = NULL;
    try {
        boardCandiesJson = serializeArray(boardCandies, &Array2DSerializeBoardSpace);
    } catch (exception e) {
        json_decref(root);
        json_decref(stateJson);
        throw e;
    }
    json_object_set_new(stateJson, "boardcandies", boardCandiesJson);

    json_t *boardStateJson = NULL;
    try {
        boardStateJson = serializeArray(boardState, &Array2DSerializeInt);
    } catch (exception e) {
        json_decref(root);
        json_decref(stateJson);
        throw e;
    }
    json_object_set_new(stateJson, "boardstate", boardStateJson);

    json_object_set_new(root, "gamestate", stateJson);
    return root;
}

json_t* GameInstance::getJsonObject(json_t *parent, string name) {
    json_t* child = json_object_get(parent, name.c_str());
    
    if (child == NULL) {
        throw invalid_argument(name + " field not found in file.");
    }
    return child;
}

Array2D GameInstance::makeArray(json_t* jsonArray, APayloadDeserializeFnPtr deserializer) {
    // Ensure valid format first.

    json_t *dataObject = getJsonObject(jsonArray, "data");

    int rows = json_integer_value(getJsonObject(jsonArray, "rows"));
    int cols = json_integer_value(getJsonObject(jsonArray, "columns"));
    Array2D array = AllocateArray2D(rows, cols);

    if (array == NULL) {
        throw "Tried to make array with invalid bounds.";
    }

    for (int i = 0; i < array->rows; i++) {
        for(int j = 0; j < array->cols; j++) {
            // Deserializes json string to payload.
            json_t* elementObject = json_array_get(dataObject, i*array->cols + j);
            if (elementObject == NULL) {
                throw "Invalid element in array data.";
            }
            char* jsonString = json_dumps(elementObject, JSON_ENCODE_ANY);
            array->elements[i][j] = deserializer(jsonString);
            free(jsonString);
        }
    }

    return array;
}

json_t* GameInstance::serializeArray(Array2D array, APayloadSerializeFnPtr serializer) {
    if (array == NULL || serializer == NULL) {
        throw invalid_argument("Null arguments");
    }

    json_t *root = json_object();
    if (root == NULL) {
        throw "Json object failed to make.";
    }

    int rowCount = GetRowCountArray2D(array);
    int columnCount = GetColumnCountArray2D(array);

    json_t *rowsJson = json_integer(rowCount);
    if (rowsJson == NULL) {
        json_decref(root);
        throw "Json object failed to make.";
    }
    json_object_set_new(root, "rows", rowsJson);

    json_t *columnsJson = json_integer(columnCount);
    if (columnsJson == NULL) {
        json_decref(root);
        throw "Json object failed to make.";
    }
    json_object_set_new(root, "columns", columnsJson);

    json_t *arrayJson = json_array();
    if (arrayJson == NULL) {
        json_decref(root);
        throw "Json object failed to make.";
    }
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < columnCount; j++) {
            char* serializedElement = serializer(GetElementArray2D(array, i, j));
            json_error_t jError;
            json_array_append_new(arrayJson, json_loads(serializedElement,
                JSON_DECODE_ANY, &jError));
            free(serializedElement);
        }
    }
    json_object_set_new(root, "data", arrayJson);

    return root;
}

void GameInstance::generateGameState() {
    movesMade = 0;
    score = 0;

    int columnCount = GetColumnCountArray2D(initialBoardState);
    int rowCount = GetRowCountArray2D(initialBoardState);
    extensionOffset = (int*)malloc(columnCount * sizeof(int));
    for (int i = 0; i < columnCount; i++) {
        extensionOffset[i] = 0;
    }

    // Initialize boardState from initialBoardState
    boardState = AllocateArray2D(rowCount, columnCount);
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < columnCount; j++) {
            // Need to copy over the value pointed to, not the pointer itself.
            int* newInt = (int*)malloc(sizeof(int));
            *newInt = *((int*)GetElementArray2D(initialBoardState, i, j));
            SetElementArray2D(boardState, i, j, (APayload_t)newInt);
        }
    }

    // Initialize boardCandies and settle it.
    boardCandies = AllocateArray2D(rowCount, columnCount);
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < columnCount; j++) {
            BoardSpace newSpace = (BoardSpace)malloc(sizeof(BoardSpace));
            newSpace->candy = -1;
            newSpace->type = 0;
            SetElementArray2D(boardCandies, i, j, (APayload_t)newSpace);
        }
    }

    settle();
}

GameInstance::~GameInstance() {
    FreeArray2D(extension, &Array2DFreeInt);
    FreeArray2D(initialBoardState, &Array2DFreeInt);
    FreeArray2D(boardState, &Array2DFreeInt);
    FreeArray2D(boardCandies, &Array2DFreeBoardSpace);
    delete[] extensionOffset;
}
