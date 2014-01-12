#include "StdAfx.h"
#include "DemoState.h"
#include "Engine.h"

namespace Glacier {

  DemoState::DemoState(): State( L"DemoState" ) {}

  void DemoState::initialize( Game* game, GameTime time )
  {
    State::initialize( game, time );
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
    State::shutdown( time );
  }

}