#include "StdAfx.h"
#include "DemoState.h"
#include "Engine.h"
#include "Game.h"
#include "Director.h"
#include "Camera.h"
#include "Graphics.h"
#include "Engine.h"
#include "ActionManager.h"
#include "ModelViewer.h"
#include "ServiceLocator.h"
#include "Exception.h"
#include "Win32.h"
#include "PhysXPhysics.h"
#include "PhysicsScene.h"
#include "PhysicsDebugVisualizer.h"
#include "WorldPrimitives.h"
#include "EntityManager.h"
#include "World.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const string cDemoStateTitle( "glacier² » demo" );

  DemoState::DemoState(): State( L"Demo" ) {}

  void DemoState::initialize( Game* game, GameTime time )
  {
    State::initialize( game, time );

    Locator::getGraphics().setRenderWindowTitle( cDemoStateTitle );

    Ogre::Plane plane( Vector3::UNIT_Y, 0.0f );
    Real width = 512.0f;
    Real height = 512.0f;
    mGround = new WorldPrimitives::Plane( gEngine->getWorld()->getPhysics(), plane, width, height, Vector3::ZERO );

    auto player = Locator::getEntities().create( "player" );
    player->spawn( Vector3( 0.0f, 1.0f, 0.0f ), Quaternion::IDENTITY );

    for ( int i = 1; i < 11; i++ )
    {
      auto cube = Locator::getEntities().create( "dev_cube" );
      cube->spawn( Vector3( 10.0f, i * 15.0f, 0.0f ), Quaternion::IDENTITY );
    }

    mDirector = new Director( &Locator::getGraphics(), player->getNode() );
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
    mDirector->getCamera()->applyMovement(
      gEngine->getActionManager()->getCameraController()->getMovement()
      );

    mDirector->getCamera()->update( (float)tick );
  }

  void DemoState::shutdown( GameTime time )
  {
    Locator::getEntities().clear();
    SAFE_DELETE( mGround );
    SAFE_DELETE( mDirector );

    State::shutdown( time );
  }

}