#include "StdAfx.h"
#include "State.h"
#include "Game.h"
#include "Engine.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  State::State( const wstring& name ): mName( name ), mPaused( false )
  {
    //
  }

  const wstring& State::getName()
  {
    return mName;
  }

  const bool State::isPaused()
  {
    return mPaused;
  }

  void State::initialize( Game* game, GameTime time )
  {
    mGame = game;
    mTime = 0.0;

    mGame->getEngine()->getConsole()->printf(
      Console::srcGame, L"Entering state %s at %f",
      mName.c_str(), time );
  }

  void State::pause( GameTime time )
  {
    mGame->getEngine()->getConsole()->printf(
      Console::srcGame, L"Pausing state %s at %f",
      mName.c_str(), time );

    mPaused = true;
  }

  void State::resume( GameTime time )
  {
    mGame->getEngine()->getConsole()->printf(
      Console::srcGame, L"Resuming state %s at %f",
      mName.c_str(), time );

    mPaused = false;
  }

  void State::shutdown( GameTime time )
  {
    mGame->getEngine()->getConsole()->printf(
      Console::srcGame, L"Leaving state %s at %f",
      mName.c_str(), time );
  }

}