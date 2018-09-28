#include "Array2D.h"
#include "Array2D_priv.h"
#include "stdlib.h"
#include "./jansson/include/jansson.h"
#include "string.h"

Array2D AllocateArray2D(int x, int y) {
    // Check valid arguments
    if (x <= 0 || y <= 0) {
        return NULL;
    }

    Array2D array = (Array2D)malloc(sizeof(Array2DInfo));
    if (array == NULL) {
        return NULL;
    }

    array->rows = x;
    array->cols = y;

    array->elements = (APayload_t**)malloc(sizeof(APayload_t*) * x);
    if (array->elements == NULL) {
        free(array);
        return NULL;
    }

    for (int i = 0; i < x; i++) {
        array->elements[i] = (APayload_t*)malloc(sizeof(APayload_t) * y);
        
        if (array->elements[i] == NULL) {
            // Free previously malloced rows
            for (int j = 0; j < i; j++) {
                free(array->elements[j]);
            }
            free(array->elements);
            free(array);
            return NULL;
        }

        for (int j = 0; j < y; j++) {
            array->elements[i][j] = NULL;
        }
    }

    return array;
}

Array2D AllocateArray2DFile(char* path, APayloadDeserializeFnPtr deserializer) {
    // Check arguments
    if (!strcmp("", path) || deserializer == NULL) {
        return NULL;
    }

    json_t *root; // json object containing all elements in file.
    json_error_t jError;

    root = json_load_file(path, 0, &jError);
    if (root == NULL) {
        return NULL;
    }

    int loadedRows = json_integer_value(json_object_get(root, "rows"));
    int loadedCols = json_integer_value(json_object_get(root, "columns"));
    Array2D array = AllocateArray2D(loadedRows, loadedCols);
    if (array == NULL) {
        json_decref(root);
        return NULL;
    }
    
    json_t *data = json_object_get(root, "data");
    if (data == NULL) {
        json_decref(root);
        return NULL;
    }

    // Copy over elements of the array.
    for (int i = 0; i < array->rows; i++) {
        for(int j = 0; j < array->cols; j++) {
            // Deserializes json string to payload.
            char* serialized = json_dumps(json_array_get(data, i*array->cols + j), JSON_ENCODE_ANY);
            if (serialized == NULL) {
                json_decref(root);
                return NULL;
            }
            array->elements[i][j] = deserializer(serialized);
            free(serialized);
        }
    }

    json_decref(root);
           
    return array;
}

APayload_t SetElementArray2D(Array2D array, int x, int y, APayload_t payload) {
    if (array == NULL || x >= array->rows || y >= array->cols || x < 0 || y < 0) {
        return NULL;
    }

    APayload_t original = array->elements[x][y];

    array->elements[x][y] = payload;

    return original;
}

APayload_t GetElementArray2D(Array2D array, int x, int y) {
    if (array == NULL || x >= array->rows || y >= array->cols || x < 0 || y < 0) {
        return NULL;
    }

    return array->elements[x][y];
}

bool SwapElementsArray2D(Array2D array, int x1, int y1, int x2, int y2) {
    // Check for valid coordinates.
    if (array == NULL || x1 >= array->rows || x2 >= array->rows ||
            y1 >= array->cols || y2 >= array->cols ||
            x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
        return false;
    }

    //Swap the stored pointers.
    APayload_t temp = array->elements[x1][y1];
    array->elements[x1][y1] = array->elements[x2][y2];
    array->elements[x2][y2] = temp;

    return true;
}

int GetRowCountArray2D(Array2D array) {
    if (array == NULL) {
        return -1;
    }

    return array->rows;
}

int GetColumnCountArray2D(Array2D array) {
    if (array == NULL) {
        return -1;
    }

    return array->cols;
}

bool SaveToFileArray2D(Array2D array, APayloadSerializeFnPtr serializer, 
        char* path) {
    // Check for valid arguments.
    if (array == NULL || serializer == NULL ||  !strcmp("", path) ) {
        return false;
    }

    json_t *data; // Stores serialized array data.
    json_error_t jError;

    data = json_array();
    if (data == NULL){
        return false;
    }
    
    // Serializes array and stores in json array.
    for (int i = 0; i < array->rows; i++) {
        for (int j = 0; j < array->cols; j++) {
            char* serialized = serializer(array->elements[i][j]);
            json_t *element = json_loads(serialized, JSON_DECODE_ANY, &jError);
            free(serialized);
            if (element == NULL) {
                json_decref(data);
                return false;
            }
            if (json_array_append_new(data, element) < 0) {
               json_decref(data);
               return false;
            }
        }
    }  

    // Combines json elements into root object.
    json_t *root = json_pack("{s:i, s:i, s:o}", "rows", array->rows, "columns", array->cols, "data", data);
    if (root == NULL) {
       json_decref(data);
       return false;
    }
    // Dumps root json object to file.
    if (json_dump_file(root, path, 0) < 0) {
        json_decref(root);
        return false;
    }

    json_decref(root);

    return true;
}

void FreeArray2D(Array2D array, APayloadFreeFnPtr freeFunction) {
    if (array != NULL && freeFunction != NULL) {
        for (int i = 0; i < array->rows; i++) {
            for (int j = 0; j < array->cols; j++) {
                freeFunction(array->elements[i][j]);
            }

            free(array->elements[i]);
        }

        free(array->elements);
        free(array);
    }
}
