#pragma once
#include "Types.h"
#include "EngineComponent.h"

namespace Glacier {

  class Physics: public EngineComponent {
  protected:
    //
  public:
    Physics( Engine* engine );
    virtual void componentPreUpdate( GameTime time );
    virtual void componentTick( GameTime tick, GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    virtual ~Physics();
  };

}