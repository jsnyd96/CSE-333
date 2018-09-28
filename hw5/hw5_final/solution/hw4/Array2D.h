#ifndef _ARRAY2D_H_
#define _ARRAY2D_H_

#include <stdbool.h>
#include "../jansson/include/jansson.h"

// Functions and types visible to client go here.

// A pointer to the client's data stored as an array element.
typedef void* APayload_t;

// Definition of a_info found in Array2D_priv.h.
struct a_info;
typedef struct a_info* Array2D;

// Function from client to free the data pointed to by APayload_t.
typedef void(*APayloadFreeFnPtr)(APayload_t payload);

// Function from client to serialize an array element.
// Serialized form must be valid JSON.
// Returned value must be freed by caller.
typedef char*(*APayloadSerializeFnPtr)(APayload_t payload);

// Function from client to deserialize an array element.
typedef APayload_t(*APayloadDeserializeFnPtr)(char* json);

// Return a constructed Array2D with the given dimensions.
// All elements are set to NULL.
// Returns NULL if either rows or columns is <= 0.
Array2D AllocateArray2D(int rows, int columns);

// Reconstructs a saved Array2D from file, with same rows, columns,
// and elements.
// Returns NULL if path or path contents are invalid.
Array2D AllocateArray2DFile(char* path, APayloadDeserializeFnPtr deserializer);

// Stores the payload in row x column y of array.
// Returns the original value in location [x][y], or null if [x][y] is out of bounds.
APayload_t SetElementArray2D(Array2D array, int x, int y, APayload_t payload);

// Returns the value stored at element (x,y) of the array.
// Returns null if (x,y) is out of bounds. 
APayload_t GetElementArray2D(Array2D array, int x, int y);

// Swap the value at [x1][y1] with the value at [x2][y2].
// Returns false if either set of coordinates are out of bounds.
bool SwapElementsArray2D(Array2D array, int x1, int y1, int x2, int y2);

// Returns the number of rows in the array, or -1 on failure.
int GetRowCountArray2D(Array2D array);

// Returns the number of columns in the array, or -1 on failure.
int GetColumnCountArray2D(Array2D array);

// Saves array to a json file.
// Returns true if successfully saved.
bool SaveToFileArray2D(Array2D array, APayloadSerializeFnPtr serializer, 
        char* path);

// Frees the structure and all held payloads.
void FreeArray2D(Array2D array, APayloadFreeFnPtr freeFunction);

#endif
