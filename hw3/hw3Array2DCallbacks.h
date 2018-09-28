#ifndef _HW3_ARRAY_2D_CALLBACKS_H_
#define _HW3_ARRAY_2D_CALLBACKS_H_

#include "Array2D.h"

APayload_t Deserializer(char* json);

void PayloadFree(APayload_t payload);

#endif
