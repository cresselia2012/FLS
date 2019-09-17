#ifndef FLS_H__
#define FLS_H__

using namespace std;

#include "probdata.h"
#include "solution.h"
#include <vector>

#define TEST 1

class FLS {

   PROB_DATA* probdata;

   vector<vector<int>> rowindex;
   vector<vector<int>> rowval;

   vector<vector<int>> colindex;
   vector<vector<int>> colval;

   void FLSsearchSolInFeasibleRegion( SOLUTION& current_sol );
   void FLSsearchSol( SOLUTION& init_sol, const vector<int>& relaxlist );

   public:

      FLS();                                 // default constructor
      FLS( const FLS &source );        // copy constructor
      FLS& operator=( const FLS& );    // assignment operator
      ~FLS();                                // destructor

      void FLSsetProbdata( PROB_DATA* set_probdata )
      { probdata = set_probdata; }
      void FLSsetup();
      void FLSsolve();

};

#endif
