/* read_data.h */
/*********************************************/
/* read data ( dim of matrix )               */
/* read data ( data )                        */
/*********************************************/
#ifndef READ_DATA_H
#define READ_DATA_H

#include <stdio.h>
#include "probdata.h"

#ifdef __cplusplus
extern "C" {
#endif


extern
void readData( const char* filename,
               PROB_DATA*  probdata
               );

#ifdef __cplusplus
}
#endif

#endif
