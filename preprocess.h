#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include"data_structure.h"

void init()
{
    for( int i=1; i<=itemnum; i++ )
    {
        current_solution[i] = random()%2;
    }
}


#endif