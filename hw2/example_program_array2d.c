#include <stdio.h>
#include <stdlib.h>

#include "Array2D.h"

// APayload_t PayloadDeserialize(int rep);

// int PayloadSerialize(APayload_t);

APayload_t Deserializer(char* json);

char* Serializer(APayload_t payload);

void PayloadFree(APayload_t payload);

void PrintArray(Array2D array);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid call: ./hw2 path\n");
        return -1;
    }

    printf("[1] Deserialize %s\n", argv[1]);
    Array2D array = AllocateArray2DFile(argv[1], &Deserializer);
    PrintArray(array);
    printf("\n");

    printf("[2] Set [1][1] to 100\n");
    int* data = (int*)malloc(sizeof(int));
    *data = 100;
    APayload_t original = SetElementArray2D(array, 1, 1, (APayload_t)data);
    free(original);
    PrintArray(array);
    printf("\n");

    printf("[3] swap [1][1] and [0][0]\n");
    SwapElementsArray2D(array, 1, 1, 0, 0);
    PrintArray(array);
    printf("\n");

    printf("[4] swap [1][1] and [10][10]\n");
    SwapElementsArray2D(array, 1, 1, 10, 10);
    PrintArray(array);
    printf("\n");

    printf("[5] serialize array to file json.out\n");
    SaveToFileArray2D(array, &Serializer, "json.out");
    printf("\n");

    printf("[6] destroy array\n");
    FreeArray2D(array, &PayloadFree);  

    return 0;
}

APayload_t Deserializer(char* json) {
    int* newPayload = (int*)malloc(sizeof(int));
    char *ptr;
    *newPayload = (int)strtol(json, &ptr, 10); // Copy i to malloced memory.
    printf("\tdeserialized %i\n", *newPayload);
    //newPayload->x = i;
    return (APayload_t)newPayload;
}

char* Serializer(APayload_t payload) {
    char* numberString = (char*)malloc(10*sizeof(char)); // Enough digits to hold an int.
    sprintf(numberString, "%i", *((int*)payload));
    printf("\tserialized %i\n", *((int*)payload));
    return numberString;
}

void PayloadFree(APayload_t payload) {
    printf("\tdestroyed %i\n", *((int*)payload));
    free(payload);
}

void PrintArray(Array2D array) {
    int rows = GetRowCountArray2D(array);
    int cols = GetColumnCountArray2D(array);
    printf("Array is %i x %i\n", rows, cols);
    for (int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            printf("%i ", *((int*)GetElementArray2D(array, i, j)));
        }
    }
    printf("\n");
}
