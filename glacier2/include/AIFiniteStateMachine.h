#pragma once
#include "AIAgent.h"
#include "AIState.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup AI
  //! @{

  namespace AI {

    class FiniteStateMachine {
    protected:
      StateVector mStates;
      Agent* mOwner;
    public:
      FiniteStateMachine( Agent* owner );
      void pushState( State* state );
      void popState();
      void changeState( State* state );
      void execute( const GameTime delta );
    };

  }

  //! @}

  //! @}

}