#pragma once
#include "EngineComponent.h"

namespace Glacier {

  class Graphics: public EngineComponent {
  public:
    Graphics( Engine* engine );
    virtual void componentPreUpdate( GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    virtual ~Graphics();
  };

}