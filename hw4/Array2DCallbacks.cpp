#include "Array2DCallbacks.h"
extern "C" {
    #include "Array2D.h"
}

#define MAX_CHARS 50

void Array2DFreeInt(APayload_t payload) {
    free(payload);
}

void Array2DFreeBoardSpace(APayload_t payload) {
    free(payload);
}

char* Array2DSerializeInt(APayload_t payload) {
    char* serializedInt = (char*)malloc(sizeof(char) * MAX_CHARS);

    sprintf(serializedInt, "%i", *((int*)payload));
    return serializedInt;
}

char* Array2DSerializeBoardSpace(APayload_t payload) {
    BoardSpace space = (BoardSpace)payload;
    char* serializedSpace = (char*)malloc(sizeof(char) * MAX_CHARS);

    sprintf(serializedSpace, "{\"color\": %i, \"type\": %i}", space->candy,
        space->type);
    return serializedSpace;
}

APayload_t Array2DDeserializeInt(char* json) {
    int* integer = (int*)malloc(sizeof(int));

    sscanf(json, "%i", integer);

    return (APayload_t) integer;
}

APayload_t Array2DDeserializeBoardSpace(char* json) {
    BoardSpace space = (BoardSpace)malloc(sizeof(struct board_space_t));

    sscanf(json, "{\"color\": %i, \"type\": %i}", &(space->candy), &(space->type));

    return (APayload_t)space;
}
