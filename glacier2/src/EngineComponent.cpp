#include "StdAfx.h"
#include "EngineComponent.h"
#include "Engine.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  EngineComponent::EngineComponent( Engine* engine ): mEngine( engine )
  {
    //
  }

  Engine* EngineComponent::getEngine()
  {
    return mEngine;
  }

  void EngineComponent::componentPreUpdate( GameTime time )
  {
    throw std::runtime_error(
      "Called componentPreUpdate on an EngineComponent that does not implement it"
    );
  }

  void EngineComponent::componentTick( GameTime tick, GameTime time )
  {
    throw std::runtime_error(
      "Called componentTick on an EngineComponent that does not implement it"
    );
  }

  void EngineComponent::componentPostUpdate( GameTime delta, GameTime time )
  {
    throw std::runtime_error(
      "Called componentPostUpdate on an EngineComponent that does not implement it"
    );
  }

  EngineComponent::~EngineComponent()
  {
    //
  }

}