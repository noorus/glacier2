#include "StdAfx.h"
#include "Director.h"
#include "Graphics.h"
#include "Camera.h"
#include "Engine.h"
#include "InputManager.h"
#include "HDR.h"
#include "Environment.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  GameCamera* Director::mCamera = nullptr;

  Director::Director( Graphics* gfx ):
  mGraphics( gfx ), mWorkspace( nullptr ), mHDR( nullptr )
  {
    mCamera = new GameCamera( mGraphics->getScene(), "defaultcamera", mGraphics->mGorillaScreen );

    gEngine->getInput()->getLocalController()->setCamera( mCamera );

    mWorkspace = mGraphics->createGameWorkspace( mCamera );

    mHDR = new HDR( mWorkspace->getCompositorManager(), Ogre::MaterialManager::getSingletonPtr() );

    auto scene = mGraphics->getScene();

    mHDR->setSkyColor( Ogre::ColourValue( 244.0f / 255.0f, 245.0f / 255.0f, 237.0f / 255.0f )  * 6.0f, 1.0f );
    mHDR->setExposure( 0.5f, 1.0f, 2.5f );
    mHDR->setBloomThreshold( Ogre::max( 3.0f - 2.0f, 0.0f ), Ogre::max( 3.0f, 0.01f ) );

    gEngine->getEnvironment()->initialize();
  }

  void Director::update( const GameTime delta )
  {
    mCamera->update( delta );
  }

  Director::~Director()
  {
    gEngine->getEnvironment()->shutdown();

    mGraphics->getScene()->destroyAllLights();

    SAFE_DELETE( mHDR );

    if ( mWorkspace )
    {
      mGraphics->destroyGameWorkspace( mWorkspace );
      mWorkspace = nullptr;
    }

    gEngine->getInput()->getLocalController()->setCamera( nullptr );

    SAFE_DELETE( mCamera );
  }

}