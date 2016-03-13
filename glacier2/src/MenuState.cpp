#include "StdAfx.h"
#include "MenuState.h"
#include "Engine.h"
#include "Game.h"
#include "Director.h"
#include "Camera.h"
#include "Graphics.h"
#include "Engine.h"
#include "ServiceLocator.h"
#include "Exception.h"
#include "Win32.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const string cMenuWindowTitle( "glacier? ? menu" );

  MenuState::MenuState(): State( L"Menu" ) {}

  void MenuState::initialize( Game* game, GameTime time )
  {
    State::initialize( game, time );

    Locator::getGraphics().setRenderWindowTitle( cMenuWindowTitle );

//    mDirector = new Director( &Locator::getGraphics() );
  }

  void MenuState::pause( GameTime time )
  {
    State::pause( time );
  }

  void MenuState::resume( GameTime time )
  {
    State::resume( time );
  }

  void MenuState::update( GameTime tick, GameTime time )
  {
//     mDirector->getCamera()->applyMovement(
//       gEngine->getActionManager()->getCameraController()->getMovement()
//     );
//
//     mDirector->getCamera()->update( (float)tick );
  }

  void MenuState::draw( GameTime delta, GameTime time )
  {
    //
  }

  void MenuState::shutdown( GameTime time )
  {
    SAFE_DELETE( mDirector );

    State::shutdown( time );
  }

}