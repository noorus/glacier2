#include "StdAfx.h"
#include "Graphics.h"
#include "Console.h"
#include "Exception.h"
#include "Engine.h"
#include "CascadedShadowMapping.h"

// Copyright 2010-2012 Matthew Paul Reid
// Modified for the Glacier2 engine

namespace Glacier {

  const Matrix4 cClipToImageSpaceMatrix(
    0.5f,   0.0f,   0.0f,   0.5f,
    0.0f,   -0.5f,  0.0f,   0.5f,
    0.0f,   0.0f,   1.0f,   0.0f,
    0.0f,   0.0f,   0.0f,   1.0f );

  CSMGpuConstants::CSMGpuConstants( size_t cascadeCount )
  {
    mParamsScaleBias = Ogre::GpuProgramManager::getSingletonPtr()->createSharedParameters( "params_shadowMatrixScaleBias" );

    for ( size_t i = 1; i < cascadeCount; i++ )
      mParamsScaleBias->addConstantDefinition( "texMatrixScaleBias" + StringConverter::toString( i ), Ogre::GCT_FLOAT4 );

    mParamsShadowMatrix = Ogre::GpuProgramManager::getSingletonPtr()->createSharedParameters( "params_shadowMatrix" );
    mParamsShadowMatrix->addConstantDefinition( "texMatrix0", Ogre::GCT_MATRIX_4X4 );
  }

  void CSMGpuConstants::updateCascade( const Ogre::Camera& texCam, size_t index )
  {
    if ( index == 0 )
    {
      mFirstCascadeViewMatrix = texCam.getViewMatrix();
      mFirstCascadeCamWidth = texCam.getOrthoWindowWidth();
      mViewRange = texCam.getFarClipDistance() - texCam.getNearClipDistance();

      Matrix4 texMatrix0 = cClipToImageSpaceMatrix * texCam.getProjectionMatrixWithRSDepth() * mFirstCascadeViewMatrix;
      mParamsShadowMatrix->setNamedConstant( "texMatrix0", texMatrix0 );
    }
    else
    {
      hack = cClipToImageSpaceMatrix * texCam.getProjectionMatrixWithRSDepth() * texCam.getViewMatrix();

      Matrix4 mat0 = mFirstCascadeViewMatrix;
      Matrix4 mat1 = texCam.getViewMatrix();

      Real offsetX = mat1[0][3] - mat0[0][3];
      Real offsetY = mat1[1][3] - mat0[1][3];
      Real offsetZ = mat1[2][3] - mat0[2][3];

      Real width0 = mFirstCascadeCamWidth;
      Real width1 = texCam.getOrthoWindowWidth();

      Real oneOnWidth = 1.0f / width0;
      Real offCenter = width1 / ( 2.0f * width0 ) - 0.5f;

      Ogre::RenderSystem* rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
      Real depthRange = Ogre::Math::Abs( rs->getMinimumDepthInputValue() - rs->getMaximumDepthInputValue() );

      Vector4 result;
      result.x = offsetX * oneOnWidth + offCenter;
      result.y = -offsetY * oneOnWidth + offCenter;
      result.z = -depthRange * offsetZ / mViewRange;
      result.w = width0 / width1;

      mParamsScaleBias->setNamedConstant( "texMatrixScaleBias" + StringConverter::toString( index ), result );
    }
  }

  StableCSMShadowCameraSetup::StableCSMShadowCameraSetup(
  CSMGpuConstants* constants ):
  mSplitPadding( 1.0f ), mGpuConstants( constants )
  {
    calculateSplitPoints( 3, 25.0f, 50.0f );
  }

  StableCSMShadowCameraSetup::~StableCSMShadowCameraSetup()
  {
  }

  void StableCSMShadowCameraSetup::calculateSplitPoints(
  size_t cascadeCount, Real firstSplitDist, Real farDist, Real lambda )
  {
    if ( cascadeCount < 2 )
      OGRE_EXCEPT( Ogre::Exception::ERR_INVALIDPARAMS, "Cannot specify less than 2 cascades",
      "StableCSMShadowCameraSetup::calculateSplitPoints" );

    mSplitPoints.resize( cascadeCount + 1 );
    mCascadeCount = cascadeCount;

    mSplitPoints[0] = 0;
    firstSplitDist = std::max( 0.001f, firstSplitDist );

    for ( size_t i = 1; i <= mCascadeCount; i++ )
    {
      Real fraction = (Real)( i - 1 ) / ( mCascadeCount - 1 );
      Real logDist = firstSplitDist * Ogre::Math::Pow( farDist / firstSplitDist, fraction );
      Real linDist = firstSplitDist + fraction * ( farDist - firstSplitDist );
      Real splitPoint = linDist + lambda * ( logDist - linDist );

      mSplitPoints[i] = splitPoint;
    }
  }

  void StableCSMShadowCameraSetup::setSplitPoints( const SplitPointList& newSplitPoints )
  {
    if ( newSplitPoints.size() < 3 ) // Splits + 1 point
      OGRE_EXCEPT( Ogre::Exception::ERR_INVALIDPARAMS, "Cannot specify less than 2 splits",
      "StableCSMShadowCameraSetup::setSplitPoints" );

    mCascadeCount = newSplitPoints.size() - 1;
    mSplitPoints = newSplitPoints;
  }

  void StableCSMShadowCameraSetup::getShadowCamera(
  const Ogre::SceneManager* sm, const Ogre::Camera* cam,
  const Ogre::Viewport* vp, const Ogre::Light* light,
  Ogre::Camera* texCam, size_t iteration ) const
  {
    texCam->setLodBias( 0.25f );

    // Apply the right clip distance.
    Real nearDist = mSplitPoints[iteration];
    Real farDist = mSplitPoints[iteration + 1];

    // Add a padding factor to internal distances so that the connecting split point will not have bad artifacts.
    if ( iteration > 0 )
      nearDist -= mSplitPadding;

    if ( iteration < mCascadeCount - 1 )
      farDist += mSplitPadding;

    mCurrentIteration = iteration;

    getShadowCameraForCascade( sm, cam, vp, light, texCam,
      iteration, nearDist, farDist );
  }

  void StableCSMShadowCameraSetup::getShadowCameraForCascade(
  const Ogre::SceneManager* sm, const Ogre::Camera* cam,
  const Viewport* vp, const Ogre::Light* light, Ogre::Camera* texCam,
  size_t iteration, Ogre::Real nearSplit, Ogre::Real farSplit ) const
  {
    Vector3 pos, dir;
    Quaternion q;

    // Reset custom view / projection matrix in case already set
    texCam->setCustomViewMatrix( false );
    texCam->setCustomProjectionMatrix( false );

    // Directional lights
    if ( light->getType() == Ogre::Light::LT_DIRECTIONAL )
    {
      // Set up the shadow texture
      // Set ortho projection
      texCam->setProjectionType( Ogre::PT_ORTHOGRAPHIC );

      // Set easy FOV and near dist so that texture covers far dist
      texCam->setFOVy( Degree( 90 ) );

      Real nearClip = light->getShadowNearClipDistance();
      if ( nearClip <= 0.0f )
        nearClip = cam->getNearClipDistance();

      Real farClip = light->getShadowFarClipDistance();
      if ( farClip <= 0.0f )
        farClip = cam->getFarClipDistance();

      texCam->setNearClipDistance( nearClip );
      texCam->setFarClipDistance( farClip );

      // Calculate texCam direction, which same as directional light direction
      dir = -light->getDerivedDirection(); // backwards since point down -z
      dir.normalise();

      // Get light orientation

      Vector3 up = Vector3::UNIT_Y;
      // Check it's not coincident with dir, otherwise use camera up
      if ( Ogre::Math::Abs( up.dotProduct( dir ) ) >= 1.0f )
        up = Vector3::UNIT_Z;

      // Cross twice to rederive, only direction is unaltered
      Vector3 left = dir.crossProduct( up );
      left.normalise();
      up = dir.crossProduct( left );
      up.normalise();

      // Derive quaternion from axes
      q.FromAxes( left, up, dir );

      // Find minimum enclosing sphere for view frustum
      // We do this in local space so that we don't have slight precision variation between frames
      Real vertical = Ogre::Math::Tan( cam->getFOVy() * 0.5f );
      Real horizontal = Ogre::Math::Tan( cam->getFOVy() * 0.5f * cam->getAspectRatio() );

      Vector3 topLeftFar;
      topLeftFar.x = horizontal * farSplit;
      topLeftFar.y = vertical * farSplit;
      topLeftFar.z = farSplit;

      Vector3 bottomRightNear;
      bottomRightNear.z = nearSplit;
      bottomRightNear.x = horizontal * bottomRightNear.z;
      bottomRightNear.y = vertical * bottomRightNear.z;

      Real dist = ( topLeftFar.squaredLength() - bottomRightNear.squaredLength() ) / ( 2.0f * ( topLeftFar.z - bottomRightNear.z ) );

      if ( dist > farSplit )
        dist = farSplit;

      Vector3 localPos( 0.0f, 0.0f, -dist ); // We have now found the point along frustum center which is equi-distant to the opposing corner positions

      Real diameter = 2.0f * topLeftFar.distance( -localPos );
      pos = cam->getDerivedPosition() + cam->getDerivedOrientation() * localPos;

      diameter *= 1.01f; // Allow some boundary pixels for filtering etc. TODO - make this a user constant
      pos += dir * 0.5f * ( farClip - nearClip ); // Pull light back so we can see the scene

      // Calculate window size
      texCam->setOrthoWindowWidth( diameter );

      // Round local x/y position based on a world-space texel; this helps to reduce
      // jittering caused by the projection moving with the camera
      Real worldTexelSize = ( texCam->getOrthoWindowWidth() ) / texCam->getViewport()->getActualWidth();

      // Convert world space camera position into light space
      Vector3 lightSpacePos = q.Inverse() * pos;

      // Snap to nearest texel
      lightSpacePos.x -= fmod( lightSpacePos.x, worldTexelSize );
      lightSpacePos.y -= fmod( lightSpacePos.y, worldTexelSize );

      // Convert back to world space
      pos = q * lightSpacePos;
    }

    // Finally set position
    texCam->setPosition( pos );
    texCam->setOrientation( q );

    // Update GPU shader parameters
    mGpuConstants->updateCascade( *texCam, iteration );
  }

}