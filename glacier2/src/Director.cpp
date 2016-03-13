#include "StdAfx.h"
#include "Director.h"
#include "Graphics.h"
#include "Camera.h"
#include "Engine.h"
#include "InputManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  GameCamera* Director::mCamera = nullptr;

  Director::Director( Graphics* gfx, SceneNode* target ):
  mGraphics( gfx ), mLight( nullptr ), mWorkspace( nullptr )
  {
    mCamera = new GameCamera( mGraphics->getScene(), "defaultcamera", target );

    gEngine->getInput()->getLocalController()->setCamera( mCamera );

    mWorkspace = mGraphics->createGameWorkspace( mCamera );

    // mGraphics->getScene()->setAmbientLight( ColourValue( 0.25f, 0.25f, 0.25f ) );

    Ogre::Light* w = mGraphics->getScene()->createLight();
    mGraphics->getScene()->getRootSceneNode( Ogre::SCENE_DYNAMIC )->attachObject( w );
    w->setType( Ogre::Light::LT_DIRECTIONAL );
    w->setDirection( Vector3( 0.3f, -1.0f, 0.4f ).normalisedCopy() );
    w->setCastShadows( true );
    w->setShadowFarClipDistance( 100.0f );
  }

  void Director::update( const GameTime delta )
  {
    mCamera->update( delta );
  }

  Director::~Director()
  {
    mGraphics->getScene()->destroyAllLights();

    if ( mWorkspace )
    {
      mGraphics->destroyGameWorkspace( mWorkspace );
      mWorkspace = nullptr;
    }

    gEngine->getInput()->getLocalController()->setCamera( nullptr );

    SAFE_DELETE( mCamera );
  }

}