#include "StdAfx.h"
#include "Graphics.h"

namespace Glacier {

  Graphics::Graphics( Engine* engine ): EngineComponent( engine )
  {
    //
  }

  void Graphics::componentPreUpdate( GameTime time )
  {
    Ogre::WindowEventUtilities::messagePump();
  }

  void Graphics::componentPostUpdate( GameTime delta, GameTime time )
  {
    // render here
  }

  Graphics::~Graphics()
  {
    //
  }

}