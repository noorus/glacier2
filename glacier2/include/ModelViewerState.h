#pragma once
#include "Utilities.h"
#include "State.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Director;
  class ModelViewer;

  class ModelViewerState: public State, public Singleton<ModelViewerState> {
  protected:
    Director* mDirector;
    ModelViewer* mViewer;
  public:
    ModelViewerState();
    virtual void initialize( Game* game, GameTime time );
    virtual void pause( GameTime time );
    virtual void update( GameTime tick, GameTime time );
    virtual void resume( GameTime time );
    virtual void shutdown( GameTime time );
  };

}