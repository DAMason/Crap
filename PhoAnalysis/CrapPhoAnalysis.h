// Busting out the actual inherited class def here so that it may be used in other root based thingies.

// Inherit from Crap

#include <TTree.h>
#include "../Crap.h"

// Inherit from Crap

class CrapPhoAnalysis: public Crap {
  public:
  CrapPhoAnalysis(TTree& tree):Crap(tree) {}
  /* Main analyzer function to be defined */
  virtual void Run();
};

