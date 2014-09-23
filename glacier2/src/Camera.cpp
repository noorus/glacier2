﻿#include "StdAfx.h"
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

  void ArcballCamera::applyMovement( const Vector3& movement )
  {
    if ( mReverseAxes ) {
      mMovement.x += movement.x * mSensitivity;
      mMovement.y += movement.y * mSensitivity;
    } else {
      mMovement.x += -movement.x * mSensitivity;
      mMovement.y += -movement.y * mSensitivity;
    }
    mMovement.z += movement.z;
  }

  void ArcballCamera::update( const float delta )
  {
    // Calculate the local X axis, as it changes with rotation along world Y
    Vector3 axis = Vector3( -mOffset.z, 0, mOffset.x );
    axis.normalise();

    // Handle new X,Y movement
    if ( mMovement.x || mMovement.y ) {
      // Treat the movement input as degrees, and convert to radians
      Radian radiansX = Radian( Ogre::Math::DegreesToRadians( mMovement.x ) );
      Radian radiansY = Radian( Ogre::Math::DegreesToRadians( mMovement.y ) );
      bool resetRotation = false;
      if ( radiansY > mOffset.angleBetween( Vector3::UNIT_Y ) - mClampTop ) {
        radiansY = mOffset.angleBetween( Vector3::UNIT_Y ) - mClampTop;
        resetRotation = true;
      } else if ( radiansY < mOffset.angleBetween( Vector3::UNIT_Y ) - mClampBottom ) {
        radiansY = mOffset.angleBetween( Vector3::UNIT_Y ) - mClampBottom;
        resetRotation = true;
      }
      // Compose the rotation delta and add to existing velocity
      Quaternion rotationX( radiansY, axis );
      Quaternion rotationY( radiansX, Vector3::UNIT_Y );
      if ( resetRotation ) {
        mRotation = rotationX * rotationY;
      } else {
        mRotation = mRotation * rotationX * rotationY;
      }
    }

    // Handle rotation if camera is outside the clamp threshold
    if ( mClampRotating ) {
      mClampRotProgress += 0.1f;
      if( mClampRotProgress > 1) {
        mClampRotating = false;
      } else {
        Quaternion clamp = Quaternion::Slerp( mClampRotProgress, 
          mClampRotation, Quaternion::IDENTITY, true );
        mOffset = clamp * mOffset;
      }
    }

    // Handle rotation velocity
    if ( mRotation != Quaternion::IDENTITY ) {
      mRotation = mRotation.Slerp( mRotDeceleration * delta, mRotation,
        Quaternion::IDENTITY, true );
      mOffset = mRotation * mOffset;
    }

    mDirection = mOffset.normalisedCopy();

    // Handle Z mouse scroll (zoom)
    if ( mMovement.z )
      mZoomVelocity += mMovement.z;
    if ( mZoomVelocity ) {
      mOffset += mDirection * mZoomVelocity * mZoomAcceleration * delta;
      Real distance = mOffset.length();
      // TODO:MEDIUM - this could be softened a bit
      if ( distance > mMaxDistance ) {
        mOffset = mDirection * mMaxDistance;
        mZoomVelocity = 0.0f;
      } else if ( distance < mMinDistance ) {
        mOffset = mDirection * mMinDistance;
        mZoomVelocity = 0.0f;
      } else {
        if ( mZoomVelocity > 0.0f ) {
          mZoomVelocity -= delta * mZoomDeceleration;
          if ( mZoomVelocity < 0.0f ) mZoomVelocity = 0.0f;
        } else {
          mZoomVelocity += delta * mZoomDeceleration;
          if ( mZoomVelocity > 0.0f ) mZoomVelocity = 0.0f;
        }
      }
    }

    // Reset movement for next frame
    mMovement = Vector3::ZERO;

    // Update camera position
    Vector3 target = pgeTarget->_getDerivedPosition();
    Vector3 vecPosition = target + mOffset;
    if ( mModifier )
      mModifier->updatePosition( this, target, mOffset, vecPosition );
    mNode->setPosition( vecPosition );

    // Always look at our target
    mCamera->lookAt( target );
  }

  void ArcballCamera::setSensitivity( const Real sensitivity )
  {
    mSensitivity = sensitivity;
  }

  void ArcballCamera::setMinDistance( const Real minimumDistance )
  {
    mMinDistance = minimumDistance;
    Real fDistance = mOffset.length();
    if ( fDistance < mMinDistance ) {
      mOffset = mOffset.normalisedCopy() * mMinDistance;
      mZoomVelocity = 0.0f;
    }
  }

  void ArcballCamera::setMaxDistance( const Real maximumDistance )
  {
    mMaxDistance = maximumDistance;
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
      Radian radiansY = mOffset.angleBetween( Vector3::UNIT_Y ) - mClampTop;
      // Calculate the local X axis, as it changes with rotation along global Y
      Vector3 axis = Vector3( -mOffset.z, 0, mOffset.x );
      axis.normalise();
      mClampRotProgress = 0.5f;
      mClampRotation = Quaternion( radiansY, axis );
      mClampRotating = true;
    }
  }

  void ArcballCamera::setClampBottom( const Radian fClampBottom )
  {
    mClampBottom = fClampBottom;
    if ( mOffset.angleBetween( Vector3::UNIT_Y ) > mClampBottom ) {
      Radian radiansY = mOffset.angleBetween( Vector3::UNIT_Y )- mClampBottom; 
      // Calculate the local X axis, as it changes with rotation along global Y
      Vector3 axis = Vector3( -mOffset.z, 0, mOffset.x );
      axis.normalise();
      mClampRotProgress = 0.5f;
      mClampRotation = Quaternion( radiansY, axis );
      mClampRotating = true;
    }
  }

  ArcballCamera::~ArcballCamera()
  {
    // Stubb
  }

}