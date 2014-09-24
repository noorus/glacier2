#include "StdAfx.h"
#include "Director.h"
#include "Graphics.h"
#include "Camera.h"
#include "HDRCompositor.h"
#include "Engine.h"
#include "GUI.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  bool cvarCallbackHDRKey( ConVar* variable, ConVar::Value oldValue )
  {
    if ( Director::mHDRCompositor )
      Director::mHDRCompositor->setKey( variable->getFloat() );
    return true;
  }

  bool cvarCallbackHDRE( ConVar* variable, ConVar::Value oldValue )
  {
    if ( Director::mHDRCompositor )
      Director::mHDRCompositor->setLocalE( variable->getFloat() );
    return true;
  }

  bool cvarCallbackHDRPhi( ConVar* variable, ConVar::Value oldValue )
  {
    if ( Director::mHDRCompositor )
      Director::mHDRCompositor->setLocalPhi( variable->getFloat() );
    return true;
  }

  bool cvarCallbackHDREnabled( ConVar* variable, ConVar::Value oldValue )
  {
    if ( Director::mHDRCompositor )
      Director::mHDRCompositor->enable( variable->getBool() );
    return true;
  }

  ENGINE_DECLARE_CONVAR_WITH_CB( hdr_key, L"HDR key",
    0.1f, cvarCallbackHDRKey );
  ENGINE_DECLARE_CONVAR_WITH_CB( hdr_e, L"HDR E",
    0.05f, cvarCallbackHDRE );
  ENGINE_DECLARE_CONVAR_WITH_CB( hdr_phi, L"HDR Phi",
    8.0f, cvarCallbackHDRPhi );
  ENGINE_DECLARE_CONVAR_WITH_CB( hdr_enabled, L"Whether to enable HDR rendering",
    false, cvarCallbackHDREnabled );

  HDRlib::HDRCompositor* Director::mHDRCompositor = nullptr;

  Director::Director( Graphics* gfx ): mGraphics( gfx ), mViewport( nullptr ),
  mCamera( nullptr ), mLight( nullptr )
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
    mViewport->setBackgroundColour( ColourValue( 0.62f, 0.78f, 0.88f ) );

    mGraphics->getEngine()->getGUI()->initialize();

    mGraphics->getScene()->setAmbientLight( ColourValue( 0.2f, 0.2f, 0.2f, 1.0f ) );

    Ogre::Light* w = mGraphics->getScene()->createLight( "doopwhoop" );
    w->setType( Ogre::Light::LT_DIRECTIONAL );
    w->setDirection( Vector3( 0.0f, -1.0f, 0.0f ) );
    w->setPowerScale( 0.5f );
    w->setDiffuseColour( ColourValue::White );
    w->setSpecularColour( ColourValue::Red );

    /*mLight = mGraphics->getScene()->createLight( "whatever" );
    mLight->setCastShadows( true );
    mLight->setType( Ogre::Light::LT_POINT );
    mLight->setDiffuseColour( ColourValue::White );
    mLight->setSpecularColour( ColourValue::White );
    mLight->setShadowFarClipDistance( 12000 );
    mLight->setAttenuation( 1.0f, 0.1f, 1.0f, 1.0f );
    mLight->setPowerScale( 0.5f );
    mLight->setVisible( true );
    mCamera->getNode()->attachObject( mLight );*/

    mHDRCompositor = new HDRlib::HDRCompositor(
      mGraphics->getWindow(), mCamera->getCamera() );

    mHDRCompositor->setTonemapper( HDRlib::Tonemapper_ReinhardMod );
    mHDRCompositor->setGlareType( HDRlib::Glare_Blur );
    mHDRCompositor->setStarType( HDRlib::Star_Plus );
    mHDRCompositor->setAutokeying( true );
    mHDRCompositor->setKey( g_CVar_hdr_key.getFloat() );
    mHDRCompositor->setAdaptionScale( 3.0f );
    mHDRCompositor->setLumAdaption( false );
    mHDRCompositor->setGlareStrength( 0.15f );
    mHDRCompositor->setStarStrength( 0.15f );
    mHDRCompositor->setLocalE( g_CVar_hdr_e.getFloat() );
    mHDRCompositor->setLocalPhi( g_CVar_hdr_phi.getFloat() );
    mHDRCompositor->enable( g_CVar_hdr_enabled.getBool() );
  }

  void Director::update( const GameTime delta )
  {
    mHDRCompositor->setFrameTime( delta );
  }

  Director::~Director()
  {
    SAFE_DELETE( mHDRCompositor );
    mGraphics->getScene()->destroyAllLights();

    mGraphics->getEngine()->getGUI()->shutdown();

    mGraphics->getWindow()->removeViewport(
      mViewport->getZOrder() );

    SAFE_DELETE( mCamera );
  }

}