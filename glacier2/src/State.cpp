#include "StdAfx.h"
#include "State.h"

namespace Glacier {

  void State::initialize( Game* game, GameTime time )
  {
    mGame = game;
    mTime = 0.0;
  }

  void State::shutdown( GameTime time )
  {
    //
  }

}