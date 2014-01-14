#include "StdAfx.h"
#include "DemoState.h"
#include "Engine.h"
#include "Game.h"
#include "Director.h"
#include "Camera.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  DemoState::DemoState(): State( L"DemoState" ) {}

  void DemoState::initialize( Game* game, GameTime time )
  {
    State::initialize( game, time );

    mDirector = new Director( mGame->getEngine()->getGraphics() );

    mDirector->getCamera()->lookAt( Vector3::ZERO );
  }

  void DemoState::pause( GameTime time )
  {
    State::pause( time );
  }

  void DemoState::resume( GameTime time )
  {
    State::resume( time );
  }

  void DemoState::update( GameTime tick, GameTime time )
  {
    //
  }

  void DemoState::shutdown( GameTime time )
  {
    SAFE_DELETE( mDirector );

    State::shutdown( time );
  }

}