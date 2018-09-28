#ifndef ARRAY2D_CALLBACKS_H
#define ARRAY2D_CALLBACKS_H

extern "C" {
    #include "Array2D.h"
}

// Contains all information
typedef struct board_space_t {
    int candy;
    int type;
} *BoardSpace;

// Frees an int contained inside an Array2D.
void Array2DFreeInt(APayload_t payload);

// Free a board space struct contained inside an Array2D.
void Array2DFreeBoardSpace(APayload_t payload);

// Serializes a single int into valid JSON.
char* Array2DSerializeInt(APayload_t payload);

// Serializes a single BoardSpace into valid JSON.
char* Array2DSerializeBoardSpace(APayload_t payload);

// Deserializes a single int from valid JSON.
APayload_t Array2DDeserializeInt(char* json);

// Deserializes a single BoardSpace from valid JSON.
APayload_t Array2DDeserializeBoardSpace(char* json);

#endif
