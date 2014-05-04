#include "StdAfx.h"
#include "Camera.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Camera::Camera( PCZSceneManager* scene, const Ogre::String& name,
  const Vector3& position, const Radian& fovy, PCZone* homeZone, bool reverseAxes ):
  mCamera( nullptr ), mNode( nullptr ), mZone( homeZone ),
  mReverseAxes( reverseAxes ), mScene( scene ), mModifier( nullptr )
  {
    assert( scene && homeZone );

    // Create camera
    mCamera = (PCZCamera*)mScene->createCamera( name );
    mCamera->setPosition( Vector3::ZERO );
    mCamera->setAutoAspectRatio( true );
    mCamera->setFOVy( fovy );

    // Create node, attach the camera
    mNode = (PCZSceneNode*)mScene->getRootSceneNode()->createChildSceneNode();
    mNode->attachObject( mCamera );
    mNode->setPosition( position );
    mNode->setFixedYawAxis( true, Vector3::UNIT_Y );
    mNode->setHomeZone( mZone );

    mCamera->setNearClipDistance( 0.01f );
    mCamera->setFarClipDistance( 1024.0f );
  }

  const Radian& Camera::getFOVy()
  {
    return mCamera->getFOVy();
  }

  void Camera::setFOVy( const Radian& fovy )
  {
    mCamera->setFOVy( fovy );
  }

  void Camera::lookAt( const Vector3& position )
  {
    mCamera->lookAt( position );
  }

  void Camera::setAxisReversion( const bool reverse )
  {
    mReverseAxes = reverse;
  }

  void Camera::castViewportRay( const Vector2& position, Ray& ray )
  {
    assert( position.x >= 0.0f && position.x <= 1.0f );
    assert( position.y >= 0.0f && position.y <= 1.0f );

    mCamera->getCameraToViewportRay( position.x, position.y, &ray );
  }

  Camera::~Camera()
  {
    if ( mNode )
    {
      mNode->detachAllObjects();
      mScene->destroySceneNode( mNode->getName() );
    }
    if ( mCamera )
      mScene->destroyCamera( mCamera );
  }

  // ArcballCamera class ======================================================

  ArcballCamera::ArcballCamera( PCZSceneManager* pgeScene,
    const Ogre::String& sName, PCZSceneNode* pgeTarget_,
    const Vector3& vecOffset, const Radian& rFOVy, PCZone* pgeHomeZone,
    bool bReverseAxes, Real fSensitivity, Real fMinDistance, Real fMaxDistance,
    Real fRotationDecel, Real fZoomAccel, Real fZoomDecel ):
  Camera( pgeScene, sName, pgeTarget_->_getDerivedPosition() + vecOffset,
    rFOVy, pgeHomeZone, bReverseAxes ), mSensitivity( fSensitivity ),
    pgeTarget( pgeTarget_ ), mOffset( vecOffset ),mMinDistance( fMinDistance ),
    mMaxDistance( fMaxDistance ), mRotDeceleration( fRotationDecel ),
    mZoomAcceleration( fZoomAccel ), mZoomDeceleration( fZoomDecel ),
    mRotation( Quaternion::IDENTITY ), mMovement( Vector3::ZERO ),
    mZoomVelocity( 0.0f ), mClampTop( 0.17f ), mClampBottom( 2.0f ),
    mClampRotation( Quaternion::IDENTITY ), mClampRotating( false )
  {
    assert( mOffset.length() > mMinDistance && mOffset.length() < mMaxDistance );
    mCamera->lookAt( pgeTarget->_getDerivedPosition() );
  }

  void ArcballCamera::applyMovement( const Vector3& vecMovement )
  {
    if ( mReverseAxes ) {
      mMovement.x += vecMovement.x * mSensitivity;
      mMovement.y += vecMovement.y * mSensitivity;
    } else {
      mMovement.x += -vecMovement.x * mSensitivity;
      mMovement.y += -vecMovement.y * mSensitivity;
    }
    mMovement.z += vecMovement.z;
  }

  void ArcballCamera::update( const float fDelta )
  {
    // Calculate the local X axis, as it changes with rotation along world Y
    Vector3 vecAxis = Vector3( -mOffset.z, 0, mOffset.x );
    vecAxis.normalise();

    // Handle new X,Y movement
    if ( mMovement.x || mMovement.y ) {
      // Treat the movement input as degrees, and convert to radians
      Radian rRotX = Radian( Ogre::Math::DegreesToRadians( mMovement.x ) );
      Radian rRotY = Radian( Ogre::Math::DegreesToRadians( mMovement.y ) );
      bool bResetRotation = false;
      if ( rRotY > mOffset.angleBetween( Vector3::UNIT_Y ) - mClampTop ) {
        rRotY = mOffset.angleBetween( Vector3::UNIT_Y ) - mClampTop;
        bResetRotation = true;
      } else if ( rRotY < mOffset.angleBetween( Vector3::UNIT_Y ) - mClampBottom ) {
        rRotY = mOffset.angleBetween( Vector3::UNIT_Y ) - mClampBottom;
        bResetRotation = true;
      }
      // Compose the rotation delta and add to existing velocity
      Quaternion qtRotX( rRotY, vecAxis );
      Quaternion qtRotY( rRotX, Vector3::UNIT_Y );
      if ( bResetRotation ) {
        mRotation = qtRotX * qtRotY;
      } else {
        mRotation = mRotation * qtRotX * qtRotY;
      }
    }

    // Handle rotation if camera is outside the clamp threshold
    if ( mClampRotating ) {
      mClampRotProgress += 0.1f;
      if( mClampRotProgress > 1) {
        mClampRotating = false;
      } else {
        Quaternion qtClampRotation = Quaternion::Slerp( mClampRotProgress, 
          mClampRotation, Quaternion::IDENTITY, true );
        mOffset = qtClampRotation * mOffset;
      }
    }

    // Handle rotation velocity
    if ( mRotation != Quaternion::IDENTITY ) {
      mRotation = mRotation.Slerp( mRotDeceleration * fDelta, mRotation,
        Quaternion::IDENTITY, true );
      mOffset = mRotation * mOffset;
    }

    mDirection = mOffset.normalisedCopy();

    // Handle Z mouse scroll (zoom)
    if ( mMovement.z )
      mZoomVelocity += mMovement.z;
    if ( mZoomVelocity ) {
      mOffset += mDirection * mZoomVelocity * mZoomAcceleration * fDelta;
      Real fDistance = mOffset.length();
      // TODO:MEDIUM - this could be softened a bit
      if ( fDistance > mMaxDistance ) {
        mOffset = mDirection * mMaxDistance;
        mZoomVelocity = 0.0f;
      } else if ( fDistance < mMinDistance ) {
        mOffset = mDirection * mMinDistance;
        mZoomVelocity = 0.0f;
      } else {
        if ( mZoomVelocity > 0.0f ) {
          mZoomVelocity -= fDelta * mZoomDeceleration;
          if ( mZoomVelocity < 0.0f ) mZoomVelocity = 0.0f;
        } else {
          mZoomVelocity += fDelta * mZoomDeceleration;
          if ( mZoomVelocity > 0.0f ) mZoomVelocity = 0.0f;
        }
      }
    }

    // Reset movement for next frame
    mMovement = Vector3::ZERO;

    // Update camera position
    Vector3 vecTarget = pgeTarget->_getDerivedPosition();
    Vector3 vecPosition = vecTarget + mOffset;
    if ( mModifier )
      mModifier->updatePosition( this, vecTarget, mOffset, vecPosition );
    mNode->setPosition( vecPosition );

    // Always look at our target
    mCamera->lookAt( vecTarget );
  }

  void ArcballCamera::setSensitivity( const Real fSensitivity )
  {
    mSensitivity = fSensitivity;
  }

  void ArcballCamera::setMinDistance( const Real fMinDistance )
  {
    mMinDistance = fMinDistance;
    Real fDistance = mOffset.length();
    if ( fDistance < mMinDistance ) {
      mOffset = mOffset.normalisedCopy() * mMinDistance;
      mZoomVelocity = 0.0f;
    }
  }

  void ArcballCamera::setMaxDistance( const Real fMaxDistance )
  {
    mMaxDistance = fMaxDistance;
    Real fDistance = mOffset.length();
    if ( fDistance > mMaxDistance ) {
      mOffset = mOffset.normalisedCopy() * mMaxDistance;
      mZoomVelocity = 0.0f;
    }
  }

  void ArcballCamera::setClampTop( const Radian fClampTop )
  {
    mClampTop = fClampTop;
    if ( mOffset.angleBetween( Vector3::UNIT_Y ) < mClampTop ) {
      Radian rRotY = mOffset.angleBetween( Vector3::UNIT_Y ) - mClampTop;
      // Calculate the local X axis, as it changes with rotation along global Y
      Vector3 vecAxis = Vector3( -mOffset.z, 0, mOffset.x );
      vecAxis.normalise();
      mClampRotProgress = 0.5f;
      mClampRotation = Quaternion( rRotY, vecAxis );
      mClampRotating = true;
    }
  }

  void ArcballCamera::setClampBottom( const Radian fClampBottom )
  {
    mClampBottom = fClampBottom;
    if ( mOffset.angleBetween( Vector3::UNIT_Y ) > mClampBottom ) {
      Radian rRotY = mOffset.angleBetween( Vector3::UNIT_Y )- mClampBottom; 
      // Calculate the local X axis, as it changes with rotation along global Y
      Vector3 vecAxis = Vector3( -mOffset.z, 0, mOffset.x );
      vecAxis.normalise();
      mClampRotProgress = 0.5f;
      mClampRotation = Quaternion( rRotY, vecAxis );
      mClampRotating = true;
    }
  }

  ArcballCamera::~ArcballCamera()
  {
    // Stubb
  }

}