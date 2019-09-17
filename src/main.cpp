#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <iomanip>
#include <unistd.h>

#include "read_data.h"
#include "probdata.h"
#include "fls.h"

using namespace std;

void getProblemName(
   const char* filename,   /* input filename         */
   char*       probname,   /* output problemname     */
   int         maxSize     /* maximum size of p.name */
   )
{
   int   i=0;
   int   j=0;
   int   l;

   /* first find end of string */
   while( filename[i]!=0 )
      ++i;
   l = i;

   /* go back until '.' or '/' or '\' appears */
   while( (i>0) && (filename[i]!='.') && (filename[i]!='/') && (filename[i]!='\\'))
      --i;

   /* if we found '.', search for '/' or '\\' */
   if( filename[i]=='.' ){
      l = i;
      while( (i>0) && (filename[i]!='/') && (filename[i]!='\\') )
         --i;
   }

   /* crrect counter */
   if( (filename[i]=='/') || (filename[i]=='\\') )
      ++i;

   /* copy name */
   while( (i<l) && (filename[i]!=0) ){
      probname[j++] = filename[i++];
      if( j>maxSize-1)
         exit(0);
   }
   probname[j] = 0;

}

int main( int argc, char** argv){

   cout << fixed << setprecision(3);

   int   opt;
   opterr = 0;

   char* filename = nullptr;
   int   nthreads = 1;
   int   timelimit = 5000;
   bool  quiet = false;
   int   memory = 8;

   while ( (opt = getopt(argc, argv, "f:p:t:qhm:re")) != -1)
   {
      switch ( opt )
      {
         case 'f':
            filename = optarg;
            break;

         case 'p':
            nthreads = atoi( optarg );
            break;

         case 't':
            timelimit = atoi( optarg );
            break;

         case 'q':
            quiet = true;
            break;

         case 'h':
            cout << "Usage: " << argv[0] << " [-f filename] [-p nthreads] [-t timelimit]" << endl;
            break;

         case 'm':
            memory = atoi ( optarg );
            break;

         default: /* '?' */
            cout << "Usage: " << argv[0] << " [-f filename] [-p nthreads] [-t timelimit]" << endl;
            break;
        }
    }

   if ( filename == nullptr )
   {
      cout << "Error: no input file" << endl;
      cout << "Usage: " << argv[0] << " [-f filename] [-p nthreads] [-t timelimit]" << endl;
      return -1;
   }

   assert( nthreads > 0 );
   assert( memory > 0 );

   memory /= nthreads;

   PROB_DATA probdata;

   readData( filename, &probdata );

   //char probname[100];
   //getProblemName( filename, probname, 100);

   cout << "filename: " << filename << endl;
   probdata.PROBDATAdispData();

   FLS fls;
   fls.FLSsetProbdata( &probdata );
   fls.FLSsetup();
   fls.FLSsolve();

   return 0;
}
