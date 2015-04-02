#include "StdAfx.h"
#include "PostProcessing.h"
#include "HDRCompositor.h"
#include "Console.h"
#include "Graphics.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_DECLARE_CONVAR_WITH_CB( hdr_enabled,
    L"Whether to enable High Dynamic Range postprocessing.", true,
    PostProcessing::callbackHDREnabled );

  PostProcessing::PostProcessing( SceneManager* scene, RenderWindow* window ):
  mScene( scene ), mWindow( window ), mHDR( nullptr ), mViewport( nullptr ),
  mHDREnabled( false )
  {
    setHDREnabled( g_CVar_hdr_enabled.getBool() );
  }

  void PostProcessing::setup( Viewport* viewport )
  {
    if ( mViewport == viewport )
      return;

    mViewport = viewport;
    checkAndUpdateState();
  }

  void PostProcessing::update( const GameTime delta )
  {
    if ( mHDR )
      mHDR->setFrameTime( (const float)delta );
  }

  void PostProcessing::checkAndUpdateState()
  {
    if ( mHDREnabled && mViewport )
    {
      mHDR = new HDRlib::HDRCompositor( mWindow, mViewport->getCamera() );
      mHDR->setTonemapper( HDRlib::Tonemapper_ReinhardMod );
      mHDR->setGlareType( HDRlib::Glare_Blur );
      mHDR->setStarType( HDRlib::Star_Plus );
      mHDR->setAutokeying( true );
      mHDR->setKey( 0.1f );
      mHDR->setAdaptionScale( 3.0f );
      mHDR->setLumAdaption( false );
      mHDR->setGlareStrength( 0.15f );
      mHDR->setStarStrength( 0.15f );
      mHDR->setLocalE( 0.05f );
      mHDR->setLocalPhi( 8.0f );
      mHDR->enable( true );
    }
    else if ( !mHDREnabled )
    {
      if ( mHDR )
        mHDR->enable( false );
      SAFE_DELETE( mHDR );
    }
  }

  void PostProcessing::setHDREnabled( const bool enabled )
  {
    if ( mHDREnabled == enabled )
      return;
    mHDREnabled = enabled;
    checkAndUpdateState();
  }

  void PostProcessing::teardown()
  {
    SAFE_DELETE( mHDR );
  }

  PostProcessing::~PostProcessing()
  {
    teardown();
  }

  bool PostProcessing::callbackHDREnabled( ConVar* variable, ConVar::Value oldValue )
  {
    if ( Locator::getGraphics().getPostProcessing() )
      Locator::getGraphics().getPostProcessing()->setHDREnabled( variable->getBool() );

    return true;
  }

}