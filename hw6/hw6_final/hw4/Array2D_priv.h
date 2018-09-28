#ifndef _ARRAY2D_PRIV_H_
#define _ARRAY2D_PRIV_H_

// Structs/functions the client shouldn't see go here.

#include "Array2D.h"

// Contains the dimensions and an array of pointers to client payloads.
typedef struct a_info {
    int rows, cols;
    APayload_t** elements;
} Array2DInfo;

#endif
