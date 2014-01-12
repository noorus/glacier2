#include "StdAfx.h"
#include "Director.h"
#include "Graphics.h"
#include "Camera.h"
#include "HDRCompositor.h"

namespace Glacier {

  Director::Director( Graphics* gfx ): mGraphics( gfx ), mViewport( nullptr ),
  mHDRCompositor( nullptr ), mCamera( nullptr )
  {
    auto zone = mGraphics->getScene()->getDefaultZone();

    mCamera = new Camera(
      mGraphics->getScene(),
      "MainCam",
      Vector3( 0.0f, 25.0f, -25.0f ),
      Radian( Math::DegreesToRadians( 90.0f ) ),
      zone );

    mViewport = mGraphics->getWindow()->addViewport( mCamera->getCamera(), 0 );
    mViewport->setClearEveryFrame( true );
    mViewport->setBackgroundColour( ColourValue( 0.62f, 0.78f, 0.88f ) );

    mHDRCompositor = new HDRlib::HDRCompositor(
      mGraphics->getWindow(), mCamera->getCamera() );

    mHDRCompositor->setTonemapper( HDRlib::HDRCompositor::Tonemapper_ReinhardLocal );
    mHDRCompositor->setGlareType( HDRlib::HDRCompositor::Glare_Blur );
    mHDRCompositor->enable( true );
  }

  Director::~Director()
  {
    SAFE_DELETE( mHDRCompositor );
    mGraphics->getWindow()->removeViewport( mViewport->getZOrder() );
    SAFE_DELETE( mCamera );
  }

}