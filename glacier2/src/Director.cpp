#include "StdAfx.h"
#include "Director.h"
#include "Graphics.h"
#include "Camera.h"
#include "HDRCompositor.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Director::Director( Graphics* gfx ): mGraphics( gfx ), mViewport( nullptr ),
  mHDRCompositor( nullptr ), mCamera( nullptr ), mLight( nullptr )
  {
    auto zone = mGraphics->getScene()->getDefaultZone();

    mCamera = new ArcballCamera(
      mGraphics->getScene(),
      "defaultcamera",
      (PCZSceneNode*)mGraphics->getScene()->getRootSceneNode(),
      Vector3( 0.0f, 0.08f, -0.25f ),
      Radian( Math::DegreesToRadians( 70.0f ) ),
      zone,
      true,
      0.25f, 0.1f, 2.0f, 10.0f, 2.0f, 10.0f );

    mViewport = mGraphics->getWindow()->addViewport(
      mCamera->getCamera(), 0 );
    mViewport->setClearEveryFrame( true );
    mViewport->setBackgroundColour( ColourValue::Black ); //ColourValue( 0.62f, 0.78f, 0.88f ) );

    mGraphics->getScene()->setAmbientLight( ColourValue( 0.2f, 0.2f, 0.2f, 1.0f ) );

    Ogre::Light* w = mGraphics->getScene()->createLight( "doopwhoop" );
    w->setType( Ogre::Light::LT_DIRECTIONAL );
    w->setDirection( Vector3( 0.0f, -1.0f, 0.0f ) );
    w->setPowerScale( 100.0f );
    w->setDiffuseColour( ColourValue::White );

    mLight = mGraphics->getScene()->createLight( "whatever" );
    mLight->setCastShadows( true );
    mLight->setType( Ogre::Light::LT_POINT );
    mLight->setDiffuseColour( ColourValue::White );
    mLight->setSpecularColour( ColourValue::White );
    mLight->setShadowFarClipDistance( 12000 );
    mLight->setAttenuation( 1.0f, 0.1f, 1.0f, 1.0f );
    mLight->setPowerScale( 50.0f );
    mLight->setVisible( true );
    mCamera->getNode()->attachObject( mLight );

    /*mHDRCompositor = new HDRlib::HDRCompositor(
      mGraphics->getWindow(), mCamera->getCamera() );

    mHDRCompositor->setTonemapper( HDRlib::Tonemapper_Linear );
    mHDRCompositor->setGlareType( HDRlib::Glare_Blur );
    mHDRCompositor->setStarType( HDRlib::Star_Plus );
    mHDRCompositor->setAutokeying( true );
    mHDRCompositor->setKey( 0.2f );
    mHDRCompositor->setAdaptionScale( 10.0f );
    mHDRCompositor->setLumAdaption( true );
    mHDRCompositor->setGlareStrength( 1.0f );
    mHDRCompositor->enable( true );*/
  }

  Director::~Director()
  {
    SAFE_DELETE( mHDRCompositor );
    mGraphics->getScene()->destroyAllLights();

    mGraphics->getWindow()->removeViewport(
      mViewport->getZOrder() );

    SAFE_DELETE( mCamera );
  }

}