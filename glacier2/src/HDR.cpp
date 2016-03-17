#include "StdAfx.h"
#include "Graphics.h"
#include "HDR.h"

namespace Glacier {

  const char* cHdrNodeName = "HdrRenderingNode";
  const char* cHdrSumMaterial = "HDR/DownScale03_SumLumEnd";
  const char* cHdrBloomMaterial = "HDR/BrightPass_Start";

  HDR::HDR( Ogre::CompositorManager2* compositorManager, Ogre::MaterialManager* materialManager ):
  mCompositorManager( compositorManager ), mMaterialManager( materialManager )
  {
    //
  }

  void HDR::setSkyColor( const ColourValue& color, Real multiplier )
  {
    auto nodeDef = mCompositorManager->getNodeDefinitionNonConst( cHdrNodeName );
    assert( nodeDef->getNumTargetPasses() >= 1 );

    auto targetDef = nodeDef->getTargetPass( 0 );
    auto& passDefs = targetDef->getCompositorPasses();
    assert( passDefs.size() >= 1 );

    auto passDef = passDefs[0];
    assert( passDef->getType() == Ogre::PASS_CLEAR && dynamic_cast<Ogre::CompositorPassClearDef*>( passDef ) );

    auto clearDef = ( Ogre::CompositorPassClearDef* )passDef;
    clearDef->mColourValue = color * multiplier;
  }

  void HDR::setExposure( Real exposure, Real minAutoExposure, Real maxAutoExposure )
  {
    assert( minAutoExposure <= maxAutoExposure );

    auto material = mMaterialManager->load( cHdrSumMaterial, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME ).staticCast<Ogre::Material>();
    auto pass = material->getTechnique( 0 )->getPass( 0 );
    auto psParams = pass->getFragmentProgramParameters();

    const Vector3 exposureParameters( 1024.0f * expf( exposure - 2.0f ),
      7.5f - maxAutoExposure, 7.5f - minAutoExposure );

    psParams->setNamedConstant( "exposure", exposureParameters );
  }

  void HDR::setBloomThreshold( Real minThreshold, Real fullColorThreshold )
  {
    assert( minThreshold < fullColorThreshold );

    auto material = mMaterialManager->load( cHdrBloomMaterial, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME ).staticCast<Ogre::Material>();
    auto pass = material->getTechnique( 0 )->getPass( 0 );
    auto psParams = pass->getFragmentProgramParameters();

    const Vector4 bloomParameters( minThreshold,
      1.0f / ( fullColorThreshold - minThreshold ),
      0.0f, 0.0f );

    psParams->setNamedConstant( "brightThreshold", bloomParameters );
  }

}