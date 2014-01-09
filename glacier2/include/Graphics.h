#pragma once
#include "EngineComponent.h"

namespace Glacier {

  class Graphics: public EngineComponent {
  public:
    Graphics( Engine* engine );
    virtual void preUpdate( GameTime time );
    virtual void postUpdate( GameTime delta, GameTime time );
    virtual ~Graphics();
  };

}