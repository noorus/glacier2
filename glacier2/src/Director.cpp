#include "StdAfx.h"
#include "Director.h"
#include "Graphics.h"
#include "Camera.h"
#include "HDRCompositor.h"
#include "Engine.h"
#include "GUI.h"
#include "CascadedShadowMapping.h"
#include "InputManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  GameCamera* Director::mCamera = nullptr;

  Director::Director( Graphics* gfx, const PCZSceneNode* target ):
  mGraphics( gfx ), mViewport( nullptr ), mLight( nullptr )
  {
    auto zone = mGraphics->getScene()->getDefaultZone();

    mCamera = new GameCamera( mGraphics->getScene(), "defaultcamera", target, zone );

    gEngine->getInput()->getLocalController()->setCamera( mCamera );

    mViewport = mGraphics->getWindow()->addViewport(
      mCamera->getCamera(), 0 );
    mViewport->setClearEveryFrame( true );
    mViewport->setBackgroundColour( ColourValue( 0.62f, 0.78f, 0.88f ) );

    mGraphics->getEngine()->getGUI()->initialize();

    mGraphics->getScene()->setAmbientLight( ColourValue( 0.25f, 0.25f, 0.25f ) );

    Ogre::Light* w = mGraphics->getScene()->createLight( "doopwhoop" );
    w->setType( Ogre::Light::LT_DIRECTIONAL );
    w->setDirection( Vector3( 0.3f, -1.0f, 0.4f ).normalisedCopy() );
    w->setCastShadows( true );
    w->setShadowFarClipDistance( 100.0f );

    mGraphics->getPostProcessing()->setup( mViewport );
  }

  void Director::update( const GameTime delta )
  {
    mCamera->update( delta );
  }

  Director::~Director()
  {
    mGraphics->getPostProcessing()->teardown();

    mGraphics->getScene()->destroyAllLights();

    mGraphics->getEngine()->getGUI()->shutdown();

    mGraphics->getWindow()->removeViewport(
      mViewport->getZOrder() );

    gEngine->getInput()->getLocalController()->setCamera( nullptr );

    SAFE_DELETE( mCamera );
  }

}