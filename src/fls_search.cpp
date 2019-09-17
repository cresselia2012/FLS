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

#include "fls.h"
#include "probdata.h"
#include "vector.h"
#include "solution.h"

using namespace std;

void FLS::FLSsearchSolInFeasibleRegion( SOLUTION& current_sol )
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
   const auto& ci = colindex;
   const auto& cv = colval;

   // search a solution in feasible region
   int stepsize;
   int candstepsize;
   int beststepsize;
   int bestindex;
   double delta;
   double bestdelta;
   while ( 1 )
   {
      beststepsize = 100;
      bestindex = -1;
      bestdelta = 0.0;
      for ( int j = 0; j < numvars; j++ )
      {
         #if TEST
         printf( "-- var_%d ---\n", j );
         #endif
         const auto& ci_j = ci[j];
         const auto& cv_j = cv[j];
         candstepsize = 100;

         assert( !ci_j.empty() );
         assert( ci_j.size() == cv_j.size() );
         const int end = (int) ci_j.size();
         for ( int i = 0; i < end; i++ )
         {
            const int ind = ci_j[i];
            const auto current_y_i = current_sol.SOLgetValY( ind );
            #if TEST
            printf( "ind = %d", ind );
            printf( ", y_%d = %d", ind, current_y_i );
            #endif
            if ( current_y_i >= 0 )
            {
               #if TEST
               printf("\n");
               #endif
               candstepsize = 0;
               break;
            }
            const int val = cv_j[i];
            assert( val > 0 );

            stepsize = - current_y_i / val;
            if ( candstepsize > stepsize )
               candstepsize = stepsize;
            #if TEST
            printf( ", stepsize = %d\n", stepsize );
            #endif
         }

         delta = 0.0;
         if ( candstepsize > 0 )
            delta = candstepsize * objcoefs[j];

         if ( bestdelta > delta )
         {
            bestdelta = delta;
            bestindex = j;
            beststepsize = candstepsize;
         }
         #if TEST
         printf( "candstepsize = %d", candstepsize );
         printf( ", delta = %f\n", delta );
         #endif
      }
      #if TEST
      printf( "bestindex = %d", bestindex );
      printf( ", beststepsize = %d", beststepsize );
      printf( ", bestdelta = %f\n", bestdelta );
      #endif

      if ( bestindex == -1 )
         break;

      // update solution
      current_sol.SOLaddObjVal( bestdelta );
      current_sol.SOLaddValX( bestindex, beststepsize );

      const auto& ci_j = ci[bestindex];
      const auto& cv_j = cv[bestindex];

      assert( !ci_j.empty() );
      assert( ci_j.size() == cv_j.size() );
      const int end = (int) ci_j.size();
      for ( int i = 0; i < end; i++ )
         current_sol.SOLaddValY( ci_j[i], cv_j[i] * beststepsize );

      #if TEST
      current_sol.SOLdisp();
      #endif
   }
}

void FLS::FLSsearchSol( SOLUTION& init_sol, const vector<int>& relaxlist )
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

   // parameters
   const int maxstepsize = 10;
   const double init_maxrate = numconss;
   const double init_minrate = 0.5;
   const double addrate = 0.1;

   // setup
   SOLUTION current_sol = init_sol;

   double* penaltyrate = new double[numconss];

   for ( int i = 0; i < numconss; i++ )
   {
      penaltyrate[i] = init_maxrate;
   }

   for ( auto i : relaxlist )
   {
      assert( i >= 0 );
      assert( i < numconss );
      penaltyrate[i] = init_minrate;
   }

   #if TEST
   cout << "FLSsearchSol-----------------------------------" << endl;
   current_sol.SOLdisp();
   cout << "relaxlist: ";
   for ( auto val: relaxlist ) cout << val << " ";
   cout << endl;
   #endif

   // get private varialbles
   const auto& ci = colindex;
   const auto& cv = colval;

   // search a solution in feasible region
   int candstepsize;
   int beststepsize;
   int bestindex;
   double delta;
   double canddelta;
   double bestdelta;

   while ( 1 )
   {
      beststepsize = 0;
      bestindex = -1;
      bestdelta = 0.0;
      #if TEST
      cout << "penaltyrate: ";
      printv( numconss, penaltyrate );
      #endif
      for ( int j = 0; j < numvars; j++ )
      {
         #if TEST
         printf( "-- var_%d ---\n", j );
         #endif
         const auto& ci_j = ci[j];
         const auto& cv_j = cv[j];
         const auto objcoef = objcoefs[j];

         assert( !ci_j.empty() );
         assert( ci_j.size() == cv_j.size() );
         const int end = (int) ci_j.size();

         // positive move
         canddelta = 0.0;
         candstepsize = 0;
         for ( int k = 1; k <= maxstepsize; k++ )
         {
            delta = objcoef * k;

            #if TEST
            printf( "p:k = %d\n", k );
            #endif
            for ( int i = 0; i < end; i++ )
            {
               const int ind = ci_j[i];
               const auto current_y_i = current_sol.SOLgetValY( ind );
               const auto penalty = - objcoef * penaltyrate[ind];
               const int val = cv_j[i];
               const int f_border = - current_y_i / val;
               const auto c_border = f_border + 1;

               assert( val > 0 );

               #if TEST
               printf( " - ind = %d", ind );
               printf( ", y_%d = %d", ind, current_y_i );
               printf( ", p_%d = %f", ind, penalty );
               if ( current_y_i < 0 )
               {
                  printf( ", f_b_%d = %d", ind, f_border );
                  printf( ", c_b_%d = %d\n", ind, c_border );
               }
               else
                  printf("\n");
               #endif

               if ( current_y_i >= 0 )
               {
                  delta += k * penalty * val;
               }
               else if ( k >= c_border )
               {
                  delta += penalty * ( current_y_i + k * val );
               }
            }

            #if TEST
            printf( " - delta = %f", delta );
            printf( ", stepsize = %d\n", k );
            #endif
            if ( canddelta > delta )
            {
               candstepsize = k;
               canddelta = delta;
            }
            else
            {
               break;
            }
         }

         #if TEST
         printf( "canddelta = %f", canddelta );
         printf( ", candstepsize = %d\n", candstepsize );
         #endif

         if ( bestdelta > canddelta )
         {
            bestdelta = canddelta;
            beststepsize = candstepsize;
            bestindex = j;
         }

         // negative move
         canddelta = 0.0;
         candstepsize = 0;

         auto neg_end = max( - current_sol.SOLgetValX( j ), - maxstepsize );

         for ( int k = -1; k >= neg_end; k-- )
         {
            delta = objcoef * k;

            #if TEST
            printf( "n:k = %d\n", k );
            #endif
            for ( int i = 0; i < end; i++ )
            {
               const int ind = ci_j[i];
               const auto current_y_i = current_sol.SOLgetValY( ind );
               const auto penalty = - objcoef * penaltyrate[ind];
               const int val = cv_j[i];
               const int c_border = (int) ceil( (double) - current_y_i / val );
               const int f_border = c_border - 1;

               assert( val > 0 );

               #if TEST
               printf( " - ind = %d", ind );
               printf( ", y_%d = %d", ind, current_y_i );
               printf( ", p_%d = %f", ind, penalty );
               if ( current_y_i > 0 )
               {
                  printf( ", f_b_%d = %d", ind, f_border );
                  printf( ", c_b_%d = %d\n", ind, c_border );
               }
               else
                  printf("\n");
               #endif

               if ( current_y_i > 0 )
               {
                  if ( k >= c_border )
                     delta += k * penalty * val;
                  else
                     delta += penalty * ( current_y_i + val * k );
               }
            }

            #if TEST
            printf( " - delta = %f", delta );
            printf( ", stepsize = %d\n", k );
            #endif
            if ( canddelta > delta )
            {
               candstepsize = k;
               canddelta = delta;
            }
            else
            {
               break;
            }
         }

         #if TEST
         printf( "canddelta = %f", canddelta );
         printf( ", candstepsize = %d\n", candstepsize );
         #endif

         if ( bestdelta > canddelta )
         {
            bestdelta = canddelta;
            beststepsize = candstepsize;
            bestindex = j;
         }
      }
      #if TEST
      printf( "bestindex = %d", bestindex );
      printf( ", beststepsize = %d", beststepsize );
      printf( ", bestdelta = %f\n", bestdelta );
      #endif

      if ( bestindex == -1 )
      {
         if ( current_sol.SOLcheckBounds() )
            break;

         // update penaltyrate
         const auto current_y = current_sol.SOLcgetVecY();
         for ( int i = 0; i < numconss; i++ )
         {
            if ( current_y[i] > 0 )
               penaltyrate[i] += (double) current_y[i] * addrate;
         }
         continue;
      }

      // update solution
      current_sol.SOLaddObjVal( objcoefs[bestindex] * beststepsize );
      current_sol.SOLaddValX( bestindex, beststepsize );

      const auto& ci_j = ci[bestindex];
      const auto& cv_j = cv[bestindex];

      assert( !ci_j.empty() );
      assert( ci_j.size() == cv_j.size() );
      const int end = (int) ci_j.size();
      for ( int i = 0; i < end; i++ )
         current_sol.SOLaddValY( ci_j[i], cv_j[i] * beststepsize );

      #if TEST
      current_sol.SOLdisp();
      #endif

      assert( current_sol.SOLcheckEqu( probdata ) );
      if ( ( init_sol.SOLcgetObjVal() > current_sol.SOLcgetObjVal() ) && current_sol.SOLcheckBounds() )
      {
         #if TEST
         cout << "update!" << endl;
         #endif
         init_sol.SOLsetSolution( current_sol );
      }

      // update penaltyrate
      const auto current_y = current_sol.SOLcgetVecY();
      for ( int i = 0; i < numconss; i++ )
      {
         if ( current_y[i] > 0 )
            penaltyrate[i] += (double) current_y[i] * addrate;
      }
   }
}


