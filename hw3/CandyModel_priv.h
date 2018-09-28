#ifndef _CANDYMODEL_PRIV_H_
#define _CANDYMODEL_PRIV_H_

#include "CandyModel.h"
#include "Array2D.h"

// Contains the state of candy crush.
typedef struct c_state {
    int moves;
    Array2D board;
} *CandyState;



#endif
