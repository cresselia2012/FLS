/* read_data.h */
/*********************************************/
/* read data ( dim of matrix )               */
/* read data ( data )                        */
/*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "read_data.h"
#include "probdata.h"

void readData( const char  *filename,
               PROB_DATA*  probdata
               )
{
   assert( probdata != nullptr );

   ifstream ifs( filename );

   if ( !ifs )
   {
       cout << "Error! File can not be opened" << endl;
   }

   string str = "";

   // get numvars
   getline( ifs, str );
   getline( ifs, str );

   int numvars = stoi( str );

   assert( numvars > 0 );

   // get numrow
   getline( ifs, str );
   getline( ifs, str );

   int numconss = stoi( str );

   assert( numconss > 0 );

   probdata->PROBDATAalloc( numvars, numconss );

   auto objcoefs = probdata->PROBDATAgetObjCoefs();
   auto conscoefs = probdata->PROBDATAgetConsCoefs();
   auto consrhs = probdata->PROBDATAgetConsRhs();

   assert( objcoefs != nullptr );
   assert( conscoefs != nullptr );
   assert( consrhs != nullptr );

   // get coefs in objective function
   getline( ifs, str );
   getline( ifs, str );

   string tmp = "";
   istringstream stream(str);
   int ct = 0;

   ct = 0;

   while ( getline( stream, tmp, ' ' ) )
      objcoefs[ct++] = stod( tmp );

   assert( ct == numvars );

   // get constant
   getline( ifs, str );
   getline( ifs, str );

   int constant = stod( str );

   probdata->PROBDATAsetConstant( constant );

   // get rhs
   getline( ifs, str );
   getline( ifs, str );

   stream = istringstream(str);

   ct = 0;

   while ( getline( stream, tmp, ' ' ) )
      consrhs[ct++] = stoi( tmp );

   assert ( ct == numconss );

   // get coefs in constraints

   getline( ifs, str );

   ct = 0;

   while ( getline( ifs, str ) )
   {
      stream = istringstream(str);

      while ( getline( stream, tmp, ' ' ) )
         conscoefs[ct++] = stoi( tmp );
   }

   assert( ct == numvars * numconss );
}
