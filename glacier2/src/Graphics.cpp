#include "StdAfx.h"
#include "Graphics.h"

namespace Glacier {

  Graphics::Graphics( Engine* engine ): EngineComponent( engine )
  {
    //
  }

  void Graphics::preUpdate( GameTime time )
  {
    Ogre::WindowEventUtilities::messagePump();
  }

  void Graphics::postUpdate( GameTime delta, GameTime time )
  {
    // render here
  }

  Graphics::~Graphics()
  {
    //
  }

}