#pragma once
#include "Utilities.h"
#include "State.h"

namespace Glacier {

  class DemoState: public State, public Singleton<DemoState> {
  public:
    DemoState();
    virtual void initialize( Game* game, GameTime time );
    virtual void pause( GameTime time );
    virtual void update( GameTime tick, GameTime time );
    virtual void resume( GameTime time );
    virtual void shutdown( GameTime time );
  };

}