#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <assert.h>
#include <cmath>

#include "solution.h"
#include "vector.h"
#include "probdata.h"

using namespace std;

#define debug_class  0
#define debug  0

SOLUTION::SOLUTION(){   // default constructor
#if debug_class
   cout << "SOLUTION: default constructor" << endl;
#endif
   num_x = -1;
   num_y = -1;
   x = nullptr;
   y = nullptr;
   objval = 999999;
}

SOLUTION::SOLUTION(
      const int set_num_x,
      const int set_num_y
      )
{
   assert( set_num_x > 0 );
   assert( set_num_y > 0 );

   num_x = set_num_x;
   num_y = set_num_y;

   x = new int[num_x];
   y = new int[num_y];

   objval = 999999;
}

SOLUTION::SOLUTION( const SOLUTION &source )
{  // copy constructor
#if debug_class
   cout << "SOLUTION: copy constructor" << endl;
#endif
   num_x = source.num_x;
   num_y = source.num_y;
   objval = source.objval;

   if( num_x > 0 )
   {
      assert( num_y > 0 );
      assert( source.x != nullptr );
      assert( source.y != nullptr );
      x = new int[num_x];
      y = new int[num_y];
      Copy_vec( source.x, x, num_x );
      Copy_vec( source.y, y, num_y );
   }
   else
   {
      x = nullptr;
      y = nullptr;
   }
}

// assignment operator
SOLUTION& SOLUTION::operator=( const SOLUTION& source )
{
#if debug_class
   cout << "SOLUTION: assignment operator" << endl;
#endif

   if( this != &source )
   {
      num_x = source.num_x;
      num_y = source.num_y;
      objval = source.objval;

      if( num_x > 0 )
      {
         assert( num_y > 0 );
         assert( source.x != nullptr );
         assert( source.y != nullptr );
         delete[] x;
         delete[] y;
         x = new int[num_x];
         y = new int[num_y];
         Copy_vec( source.x, x, num_x );
         Copy_vec( source.y, y, num_y );
      }
      else
      {
         x = nullptr;
         y = nullptr;
      }
   }

   return *this;
}

// destructor
SOLUTION::~SOLUTION()
{
#if debug_class
   cout << "SOLUTION: destructor" << endl;
#endif
   delete[] x;
   delete[] y;
   x = nullptr;
   y = nullptr;
}

//void SOLUTION::set_sol(
//   int      s_m,
//   double   *s_val,
//   double   s_objval
//   )
//{
//   assert( s_m > 0 );
//   assert( s_val != nullptr );
//   assert( s_objval > 0.0 );
//   assert( val == nullptr );
//
//   m = s_m;
//   objval = s_objval;
//
//   val = new double[m];
//
//   Copy_vec( s_val, val, m);
//}
//
bool SOLUTION::SOLcheckBounds() const
{
   assert( x != nullptr );
   assert( y != nullptr );

   for ( int i = 0; i < num_y; i++ )
   {
      if ( y[i] > 0 )
         return false;
   }

   for ( int i = 0; i < num_x; i++ )
   {
      if ( x[i] < 0 )
         return false;
   }

   return true;
}

void SOLUTION::SOLdisp() const
{
   cout << "**********************************" << endl;
   cout << "var_x: ";
   printv( num_x, x );
   cout << "var_y: ";
   printv( num_y, y );
   cout << "objval: " << objval << endl;
   cout << "**********************************" << endl;
}

void SOLUTION::SOLsetSolution( const SOLUTION& sol )
{
   assert( sol.SOLcgetNumX() == num_x );
   assert( sol.SOLcgetNumY() == num_y );
   assert( x != nullptr );
   assert( y != nullptr );

   Copy_vec( sol.SOLcgetVecX(), x, num_x );
   Copy_vec( sol.SOLcgetVecY(), y, num_y );
   objval = sol.SOLcgetObjVal();
}

bool SOLUTION::SOLcheckEqu( const PROB_DATA* probdata ) const
{
   assert( x != nullptr );
   assert( y != nullptr );

   const auto conscoefs = probdata->PROBDATAcgetConsCoefs();
   const auto consrhs = probdata->PROBDATAcgetConsRhs();

   int ct = 0;
   for ( int i = 0; i < num_y; i++ )
   {
      int buf = - consrhs[i];
      for ( int j = 0; j < num_x; j++ )
      {
         buf += x[j] * conscoefs[ct++];
      }
      //cout << "buf: " << buf << endl;
      //cout << "y: " << y[i] << endl;
      if ( y[i] != buf )
         return false;
   }
   assert ( ct == num_x * num_y );

   return true;
}


bool SOLUTION::SOLcheckConstraints( const PROB_DATA* probdata ) const
{
   if ( !SOLcheckBounds() )
      return false;

   return SOLcheckEqu( probdata );
}

double SOLUTION::SOLcomputeObjVal( const PROB_DATA* probdata ) const
{
   const auto objcoefs = probdata->PROBDATAcgetObjCoefs();

   auto result = probdata->PROBDATAcgetConstant();

   for ( int i = 0; i < num_x; i++ )
   {
      result += objcoefs[i] * x[i];
   }

   return result;
}

void SOLUTION::SOLcomputeYandObjVal( const PROB_DATA* probdata )
{
   assert( x != nullptr );
   assert( y != nullptr );

   const auto conscoefs = probdata->PROBDATAcgetConsCoefs();
   const auto consrhs = probdata->PROBDATAcgetConsRhs();

   int ct = 0;
   for ( int i = 0; i < num_y; i++ )
   {
      y[i] = - consrhs[i];
      for ( int j = 0; j < num_x; j++ )
      {
         y[i] += x[j] * conscoefs[ct++];
      }
   }

   assert ( ct == num_x * num_y );

   objval = SOLcomputeObjVal( probdata );
}

bool SOLUTION::SOLcheck( const PROB_DATA* probdata ) const
{
   if ( !SOLcheckConstraints( probdata ) )
      return false;

   bool result = false;
   if ( pow( SOLcomputeObjVal( probdata ) - objval, 2 ) < 0.000001 )
      result = true;

   return true;
}
