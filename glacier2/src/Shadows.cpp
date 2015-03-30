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

  const size_t cShadowCascadeCount = 4;
  const Real cShadowLambda = 0.93f;
  const Real cShadowNearSplit = 0.5f;
  const Real cShadowFarSplit = 20.0f;

  Shadows::Shadows(): mConstants( nullptr ), mSetup( nullptr )
  {
    mConstants = new CSMGpuConstants( cShadowCascadeCount );
    mSetup = new StableCSMShadowCameraSetup( mConstants );
  }

  Shadows::~Shadows()
  {
    SAFE_DELETE( mConstants );
  }

  void Shadows::applyTo( SceneManager* scene )
  {
    scene->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED );
    scene->setShadowCasterRenderBackFaces( false );
    scene->setShadowTextureCount( 4 );
    scene->setShadowTextureCountPerLightType( Ogre::Light::LT_DIRECTIONAL, 4 );

    for ( int i = 0; i < 4; i++ )
      scene->setShadowTextureConfig( i, 1024, 1024, Ogre::PF_FLOAT32_R );

    mSetup->calculateSplitPoints( cShadowCascadeCount, cShadowNearSplit, cShadowFarSplit, cShadowLambda );
    mSetup->setSplitPadding( 1.0f );

    scene->setShadowCameraSetup( Ogre::ShadowCameraSetupPtr( mSetup ) );
  }

}