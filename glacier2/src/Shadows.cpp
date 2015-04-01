#include "StdAfx.h"
#include "Shadows.h"
#include "CascadedShadowMapping.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "Graphics.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_DECLARE_CONVAR_WITH_CB( csm_enabled,
    L"Whether to enable Cascaded Shadow Mapping.", true,
    Shadows::callbackEnabled );

  const size_t cShadowCascadeCount = 4;
  const Real cShadowLambda = 0.93f;
  const Real cShadowNearSplit = 0.5f;
  const Real cShadowFarSplit = 20.0f;

  Shadows::Shadows(): mConstants( nullptr ),
  mScene( nullptr ), mEnabled( false )
  {
    mConstants = new CSMGpuConstants( cShadowCascadeCount );
    setEnabled( g_CVar_csm_enabled.getBool() );
  }

  Shadows::~Shadows()
  {
    SAFE_DELETE( mConstants );
  }

  void Shadows::setup()
  {
    mScene->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED );
    mScene->setShadowCasterRenderBackFaces( false );
    mScene->setShadowTextureCount( 4 );
    mScene->setShadowTextureCountPerLightType( Ogre::Light::LT_DIRECTIONAL, 4 );

    for ( int i = 0; i < 4; i++ )
      mScene->setShadowTextureConfig( i, 1024, 1024, Ogre::PF_FLOAT32_R );

    auto csmSetup = new StableCSMShadowCameraSetup( mConstants );
    csmSetup->calculateSplitPoints( cShadowCascadeCount, cShadowNearSplit, cShadowFarSplit, cShadowLambda );
    csmSetup->setSplitPadding( 1.0f );

    mSetup = Ogre::ShadowCameraSetupPtr( csmSetup );
    mScene->setShadowCameraSetup( mSetup );
  }

  void Shadows::teardown()
  {
    mScene->setShadowTechnique( Ogre::SHADOWTYPE_NONE );
    mScene->setShadowTextureCount( 0 );
    mScene->setShadowTextureCountPerLightType( Ogre::Light::LT_DIRECTIONAL, 0 );
    mSetup.setNull();
    mScene->setShadowCameraSetup( mSetup );
  }

  void Shadows::checkAndUpdateState()
  {
    if ( mEnabled && mScene )
      setup();
    else if ( !mEnabled && mScene )
      teardown();
  }

  void Shadows::applyTo( SceneManager* scene )
  {
    if ( mScene == scene )
      return;
    mScene = scene;
    checkAndUpdateState();
  }

  void Shadows::setEnabled( const bool enabled )
  {
    if ( mEnabled == enabled )
      return;
    mEnabled = enabled;
    checkAndUpdateState();
  }

  bool Shadows::callbackEnabled( ConVar* variable, ConVar::Value oldValue )
  {
    if ( Locator::getGraphics().getShadows() )
      Locator::getGraphics().getShadows()->setEnabled( variable->getBool() );

    return true;
  }

}