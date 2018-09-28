#include "hw3Array2DCallbacks.h"
#include "Array2D.h"

// Takes a valid json text, converts to an int, and returns a pointer to it.
APayload_t Deserializer(char* json) {
    int* newPayload = (int*)malloc(sizeof(int));
    char *ptr;
    *newPayload = (int)strtol(json, &ptr, 10); // Copy i to malloced memory.
    return (APayload_t)newPayload;
}

// Frees the memory storing the int.
void PayloadFree(APayload_t payload) {
    free(payload);
}
