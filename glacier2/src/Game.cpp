#include "StdAfx.h"
#include "Game.h"
#include "Engine.h"
#include "EngineComponent.h"

namespace Glacier {

  Game::Game( Engine* engine ): EngineComponent( engine )
  {
    //
  }

  void Game::componentTick( GameTime tick, GameTime time )
  {
    if ( !mStates.empty() )
      mStates.back()->update( tick, time );
  }

  void Game::changeState( State* state )
  {
    if ( !mStates.empty() )
    {
      mStates.back()->shutdown( mEngine->getTime() );
      mStates.pop_back();
    }

    mStates.push_back( state );

    state->initialize( this, mEngine->getTime() );
  }

  void Game::pushState( State* state )
  {
    if ( !mStates.empty() )
      mStates.back()->pause( mEngine->getTime() );

    mStates.push_back( state );

    state->initialize( this, mEngine->getTime() );
  }

  void Game::popState()
  {
    if ( !mStates.empty() )
    {
      mStates.back()->shutdown( mEngine->getTime() );
      mStates.pop_back();
    }

    if ( !mStates.empty() )
      mStates.back()->resume( mEngine->getTime() );
  }

  void Game::suspendState()
  {
    if ( !mStates.empty() )
      mStates.back()->shutdown( mEngine->getTime() );
  }

  void Game::resumeState()
  {
    if ( !mStates.empty() )
      mStates.back()->initialize( this, mEngine->getTime() );
  }

  Game::~Game()
  {
    while ( !mStates.empty() )
    {
      mStates.back()->shutdown( mEngine->getTime() );
      mStates.pop_back();
    }
  }

}