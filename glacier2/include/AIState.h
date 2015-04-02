#pragma once
#include "AIAgent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace AI {

    class State {
    public:
      virtual void enter( Agent* agent ) = 0;
      virtual void pause( Agent* agent ) = 0;
      virtual void resume( Agent* agent ) = 0;
      virtual void execute( Agent* agent ) = 0;
      virtual void leave( Agent* agent ) = 0;
    };

    typedef vector<State*> StateVector;

  }

}