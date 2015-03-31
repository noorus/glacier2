#include "StdAfx.h"
#include "Director.h"
#include "Graphics.h"
#include "Camera.h"
#include "HDRCompositor.h"
#include "Engine.h"
#include "GUI.h"
#include "CascadedShadowMapping.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  PostProcessing::PostProcessing( SceneManager* scene, RenderWindow* window ):
  mScene( scene ), mWindow( window ), mHDR( nullptr )
  {
    //
  }

  void PostProcessing::setup( Viewport* viewport )
  {
    mHDR = new HDRlib::HDRCompositor( mWindow, viewport->getCamera() );

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

  void PostProcessing::update( const GameTime delta )
  {
    if ( mHDR )
      mHDR->setFrameTime( (const float)delta );
  }

  void PostProcessing::teardown()
  {
    SAFE_DELETE( mHDR );
  }

  PostProcessing::~PostProcessing()
  {
    teardown();
  }

}