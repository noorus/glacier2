#include "StdAfx.h"
#include "EngineComponent.h"
#include "Engine.h"

namespace Glacier {

  EngineComponent::EngineComponent( Engine* engine ): mEngine( engine )
  {
    //
  }

  void EngineComponent::preUpdate( GameTime time )
  {
    throw std::runtime_error(
      "Called preUpdate on an EngineComponent that does not implement it"
    );
  }

  void EngineComponent::update( GameTime tick, GameTime time )
  {
    throw std::runtime_error(
      "Called update on an EngineComponent that does not implement it"
    );
  }

  void EngineComponent::postUpdate( GameTime delta, GameTime time )
  {
    throw std::runtime_error(
      "Called postUpdate on an EngineComponent that does not implement it"
    );
  }

  EngineComponent::~EngineComponent()
  {
    //
  }

}