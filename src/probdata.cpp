#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#include "probdata.h"
#include "vector.h"

using namespace std;

#define debug_class	0
#define debug	0

PROB_DATA::PROB_DATA(){	// default constructor
#if debug_class
	cout << "PROB_DATA: default constructor" << endl;
#endif
   numvars = -1;
   numconss = -1;
   objcoefs = nullptr;
   conscoefs = nullptr;
   consrhs = nullptr;
   constant = 0;
}

PROB_DATA::PROB_DATA( const PROB_DATA &source )
{	// copy constructor
#if debug_class
	cout << "PROB_DATA: copy constructor" << endl;
#endif
   numvars = source.numvars;
   numconss = source.numconss;
   constant = source.constant;

	if( numvars > 0 )
   {
      assert( numconss > 0 );
		assert( source.objcoefs!= nullptr );
		assert( source.conscoefs!= nullptr );
		assert( source.consrhs!= nullptr );

      objcoefs = new double[numvars];
      conscoefs = new int[numconss * numvars];
      consrhs = new int[numconss];

      Copy_vec( source.objcoefs, objcoefs, numvars );
      Copy_vec( source.conscoefs, conscoefs, numconss * numvars );
      Copy_vec( source.consrhs, consrhs, numconss );
	}
   else
   {
      assert( numconss <= 0 );
      objcoefs = nullptr;
      conscoefs = nullptr;
      consrhs = nullptr;
	}
}

// assignment operator
PROB_DATA& PROB_DATA::operator=( const PROB_DATA& source )
{
#if debug_class
	cout << "PROB_DATA: assignment operator" << endl;
#endif

	if( this != &source )
   {
      numvars = source.numvars;
      numconss = source.numconss;
      constant = source.constant;

      delete[] objcoefs;
      delete[] conscoefs;
      delete[] consrhs;
	   if( numvars > 0 )
      {
         assert( numconss > 0 );
	   	assert( source.objcoefs!= nullptr );
	   	assert( source.conscoefs!= nullptr );
	   	assert( source.consrhs!= nullptr );

         objcoefs = new double[numvars];
         conscoefs = new int[numconss * numvars];
         consrhs = new int[numconss];

         Copy_vec( source.objcoefs, objcoefs, numvars );
         Copy_vec( source.conscoefs, conscoefs, numconss * numvars );
         Copy_vec( source.consrhs, consrhs, numconss );
	   }
      else
      {
         assert( numconss <= 0 );
         objcoefs = nullptr;
         conscoefs = nullptr;
         consrhs = nullptr;
	   }
	}

	return *this;
}

// destructor
PROB_DATA::~PROB_DATA()
{
#if debug_class
	cout << "PROB_DATA: destructor" << endl;
#endif
   delete[] objcoefs;
   delete[] conscoefs;
   delete[] consrhs;
   objcoefs = nullptr;
   conscoefs = nullptr;
   consrhs = nullptr;
}

bool PROB_DATA::PROBDATAalloc(
      const int set_numvars,
      const int set_numconss
      )
{
   if ( numvars > 0 )
   {
      assert( numconss <= 0 );
      assert( objcoefs != nullptr );
      assert( conscoefs != nullptr );
      assert( consrhs != nullptr );
      return false;
   }

   numvars = set_numvars;
   numconss = set_numconss;

   assert( objcoefs == nullptr );
   assert( conscoefs == nullptr );
   assert( consrhs == nullptr );

   objcoefs = new double[numvars];
   conscoefs = new int[numvars * numconss];
   consrhs = new int[numconss];

   return true;
}

void PROB_DATA::PROBDATAdispData() const
{
   cout << "------------------------" << endl;
   cout << "numvars: \t" << numvars << endl;
   cout << "numconss: \t" << numconss << endl;
   cout << "constant: \t" << constant << endl;
   cout << "obj.f:" << endl;
   printv( numvars, objcoefs );
   cout << "constraints:" << endl;
   for ( int i = 0, ct = 0; i < numconss; i++ )
   {
      cout << "C" << i << ": ";
      for ( int j = 0; j < numvars; j++ )
         cout << conscoefs[ct++] << " ";
      cout << "<= " << consrhs[i] << endl;
   }
   cout << "------------------------" << endl;
}
