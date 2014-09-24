#pragma once
#include "Types.h"
#include "State.h"
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \class Game
  //! Game state machine
  class Game: public EngineComponent {
  protected:
    StateVector mStates; //!< Game state stack
  public:
    Game( Engine* engine );
    virtual void componentTick( GameTime tick, GameTime time );
    void changeState( State* state );
    void pushState( State* state );
    void popState();
    void suspendState();
    void resumeState();
    virtual ~Game();
  };

  //! @}

}