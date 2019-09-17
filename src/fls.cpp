#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#include "fls.h"
#include "probdata.h"
#include "vector.h"
#include "solution.h"

using namespace std;

#define debug_class	0
#define debug	0

FLS::FLS(){	// default constructor
#if debug_class
	cout << "FLS: default constructor" << endl;
#endif
   probdata = nullptr;
}

FLS::FLS( const FLS &source )
{	// copy constructor
#if debug_class
	cout << "FLS: copy constructor" << endl;
#endif
   probdata = source.probdata;
   rowindex = source.rowindex;
   rowval = source.rowval;
   colindex = source.colindex;
   colval = source.colval;
}

// assignment operator
FLS& FLS::operator=( const FLS& source )
{
#if debug_class
	cout << "FLS: assignment operator" << endl;
#endif

	if( this != &source )
   {
      probdata = source.probdata;
      rowindex = source.rowindex;
      rowval = source.rowval;
      colindex = source.colindex;
      colval = source.colval;
	}

	return *this;
}

// destructor
FLS::~FLS()
{
#if debug_class
	cout << "FLS: destructor" << endl;
#endif
   probdata = nullptr;

   for ( auto v : rowindex )
   {
      v.clear();
      v.shrink_to_fit();
   }
   rowindex.clear();
   rowindex.shrink_to_fit();

   for ( auto v : rowval )
   {
      v.clear();
      v.shrink_to_fit();
   }
   rowval.clear();
   rowval.shrink_to_fit();

   for ( auto v : colindex )
   {
      v.clear();
      v.shrink_to_fit();
   }
   colindex.clear();
   colindex.shrink_to_fit();

   for ( auto v : colval )
   {
      v.clear();
      v.shrink_to_fit();
   }
   colval.clear();
   colval.shrink_to_fit();
}

void FLS::FLSsetup()
{
   // get probdata
   assert( probdata != nullptr );

   const auto conscoefs = probdata->PROBDATAcgetConsCoefs();
   const auto numvars = probdata->PROBDATAcgetNumVars();
   const auto numconss = probdata->PROBDATAcgetNumConss();

   assert( conscoefs != nullptr );
   assert( numvars > 0 );
   assert( numconss > 0 );

   // set rowindex and rowval
   rowindex = vector<vector<int>> ( numconss );
   rowval = vector<vector<int>> ( numconss );

   int ct;
   int val;

   ct = 0;
   for ( int i = 0; i < numconss; i++ )
   {
      for ( int j = 0; j < numvars; j++ )
      {
         val = conscoefs[ct++];
         assert( val >= 0 );
         if ( val > 0 )
         {
            rowindex[i].push_back( j );
            rowval[i].push_back( val );
         }
      }
      rowindex[i].shrink_to_fit();
      rowval[i].shrink_to_fit();
   }
   assert( ct == numconss * numvars );

   // set colindex and colval
   colindex = vector<vector<int>> ( numvars );
   colval = vector<vector<int>> ( numvars );

   int ind;

   for ( int i = 0; i < numconss; i++ )
   {
      for ( int j = 0, end = (int) rowindex[i].size(); j < end; j++ )
      {
         ind = rowindex[i][j];
         assert( ind >= 0 );
         assert( ind < numvars );
         colindex[ind].push_back( i );
         colval[ind].push_back( rowval[i][j] );
      }
   }
   for ( auto& vec : colindex )
      vec.shrink_to_fit();
   for ( auto& vec : colval )
      vec.shrink_to_fit();

   //for ( auto vec : colval )
   //{
   //   for ( auto val : vec )
   //      cout << val << " ";
   //   cout << endl;
   //}
}

void FLS::FLSsolve()
{
   // get probdata
   const auto pd = probdata;
   const auto numvars = pd->PROBDATAcgetNumVars();
   const auto numconss = pd->PROBDATAcgetNumConss();
   const auto objcoefs = pd->PROBDATAcgetObjCoefs();
   const auto conscoefs = pd->PROBDATAcgetConsCoefs();
   const auto consrhs = pd->PROBDATAcgetConsRhs();

   assert( pd != nullptr );
   assert( numvars > 0 );
   assert( numconss > 0 );
   assert( objcoefs != nullptr );
   assert( conscoefs != nullptr );
   assert( consrhs != nullptr );

   // get private varialbles
   //auto& ri = rowindex;
   //auto& rv = rowval;
   //const auto& ci = colindex;
   //const auto& cv = colval;

   // set an init solution: x = 0
   SOLUTION current_sol( numvars, numconss );
   auto current_x = current_sol.SOLgetVecX();

   Gen_ZeroVec( numvars, current_x );

   current_sol.SOLcomputeYandObjVal( pd );

   #if TEST
   current_sol.SOLdisp();
   #endif

   assert( current_sol.SOLcheck( pd ) );
   FLSsearchSolInFeasibleRegion( current_sol );

   #if TEST
   current_sol.SOLdisp();
   #endif

   vector<int> relaxlist;

   for ( int i = 0; i < numconss; i++ )
   {
      if ( current_sol.SOLgetValY( i ) == 0 )
      {
         relaxlist.push_back( i );
      }
   }

   assert( current_sol.SOLcheck( pd ) );
   FLSsearchSol( current_sol, relaxlist );
   #if TEST
   current_sol.SOLdisp();
   #endif

}
