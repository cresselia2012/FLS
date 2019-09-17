#ifndef PROBDATA_H__
#define PROBDATA_H__

using namespace std;

class PROB_DATA{
   int         numvars;
   int         numconss;
   double*     objcoefs;   // [numvars]
   double      constant;
   int*        conscoefs;  //[numconss*numvars], rowMajor
   int*        consrhs;    // [numconss]
   public:

      PROB_DATA();                                 // default constructor
      PROB_DATA( const PROB_DATA &source );        // copy constructor
      PROB_DATA& operator=( const PROB_DATA& );    // assignment operator
      ~PROB_DATA();                                // destructor

      bool PROBDATAalloc( const int numvars, const int numconss );
      void PROBDATAsetConstant( const int set_constant ){
         constant = set_constant;
      }

      auto PROBDATAgetObjCoefs() { return objcoefs; }
      auto PROBDATAgetConsCoefs() { return conscoefs; }
      auto PROBDATAgetConsRhs() { return consrhs; }

      auto PROBDATAcgetNumVars() const { return numvars; }
      auto PROBDATAcgetNumConss() const { return numconss; }
      auto PROBDATAcgetConstant() const { return constant; }
      auto PROBDATAcgetObjCoefs() const { return objcoefs; }
      auto PROBDATAcgetConsCoefs() const { return conscoefs; }
      auto PROBDATAcgetConsRhs() const { return consrhs; }

      void PROBDATAdispData() const;
};

#endif
