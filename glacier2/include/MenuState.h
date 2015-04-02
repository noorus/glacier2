#pragma once
#include "Utilities.h"
#include "State.h"

// Glacier? Game Engine ? 2014 noorus
// All rights reserved.

namespace Glacier {

  class Director;

  class MenuState: public State, public Singleton<MenuState> {
  protected:
    Director* mDirector;
  public:
    MenuState();
    virtual void initialize( Game* game, GameTime time );
    virtual void pause( GameTime time );
    virtual void update( GameTime tick, GameTime time );
    virtual void draw( GameTime delta, GameTime time );
    virtual void resume( GameTime time );
    virtual void shutdown( GameTime time );
  };

}