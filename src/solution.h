#ifndef SOLUTION_H__
#define SOLUTION_H__

using namespace std;

#include "probdata.h"

class SOLUTION{
   int      num_x;
   int      num_y;
   int*     x;
   int*     y;
   double   objval;

   public:

      SOLUTION();                               // default constructor
      SOLUTION( const int set_num_x, const int set_num_y );
      SOLUTION( const SOLUTION &source );       // copy constructor
      SOLUTION& operator=( const SOLUTION& );      // assignment operator
      ~SOLUTION();                                 // destructor

      auto SOLgetVecX() { return x; }
      auto SOLgetVecY() { return y; }

      auto SOLcgetNumX() const { return num_x; }
      auto SOLcgetNumY() const { return num_y; }
      auto SOLcgetVecX() const { return x; }
      auto SOLcgetVecY() const { return y; }
      auto SOLgetValX( int j ) const
      { assert( x != nullptr ); return x[j]; }
      auto SOLgetValY( int i ) const
      { assert( y != nullptr ); return y[i]; }
      auto SOLcgetObjVal() const { return objval; }

      void SOLsetObjVal( const double set_objval ) { objval = set_objval; }
      void SOLsetSolution( const SOLUTION& sol );

      void SOLaddValX( int j, int val )
      { assert( x != nullptr ); x[j] += val; }
      void SOLaddValY( int i, int val )
      { assert( y != nullptr ); y[i] += val; }
      void SOLaddObjVal( int val ) { objval += val; }

      bool SOLcheckBounds() const;
      bool SOLcheckEqu( const PROB_DATA* probdata ) const;
      bool SOLcheckConstraints( const PROB_DATA* probdata ) const;
      bool SOLcheck( const PROB_DATA* probdata ) const;
      double SOLcomputeObjVal( const PROB_DATA* probdata ) const;
      void SOLcomputeYandObjVal( const PROB_DATA* probdata );

      void SOLdisp() const;

};

#endif
