#pragma once
#include "Utilities.h"
#include "State.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Director;
  class PhysicsScene;
  class PhysicsDebugVisualizer;

  namespace Primitives {
    class Plane;
  }

  class DemoState: public State, public Singleton<DemoState> {
  protected:
    Director* mDirector;
    Primitives::Plane* mGround;
  public:
    DemoState();
    Director* getDirector() { return mDirector; }
    virtual void initialize( Game* game, GameTime time );
    virtual void pause( GameTime time );
    virtual void update( GameTime tick, GameTime time );
    virtual void resume( GameTime time );
    virtual void shutdown( GameTime time );
  };

}