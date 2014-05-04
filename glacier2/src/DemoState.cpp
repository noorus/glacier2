#include "StdAfx.h"
#include "DemoState.h"
#include "Engine.h"
#include "Game.h"
#include "Director.h"
#include "Camera.h"
#include "Graphics.h"
#include "Engine.h"
#include "ActionManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  DemoState::DemoState(): State( L"DemoState" ) {}

  void DemoState::initialize( Game* game, GameTime time )
  {
    State::initialize( game, time );

    mDirector = new Director( mGame->getEngine()->getGraphics() );

    PCZSceneManager* scm = mGame->getEngine()->getGraphics()->getScene();

    PCZSceneNode* root = (PCZSceneNode*)scm->getRootSceneNode();

    PCZSceneNode* node = (PCZSceneNode*)root->createChildSceneNode();
    scm->addPCZSceneNode( node, scm->getDefaultZone() );

    Ogre::Entity* rotta = scm->createEntity( "rotta_01.mesh" );
    rotta->setVisible( true );
    node->attachObject( rotta );
    node->setPosition( 0, 0, 0 );
    node->setOrientation( Quaternion::IDENTITY );

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
    mDirector->getCamera()->applyMovement( gEngine->getActionManager()->getCameraController()->getMovement() );
    mDirector->getCamera()->update( tick );
  }

  void DemoState::shutdown( GameTime time )
  {
    SAFE_DELETE( mDirector );

    State::shutdown( time );
  }

}