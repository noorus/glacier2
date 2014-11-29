#include "StdAfx.h"
#include "Camera.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  // ArcballCamera class ======================================================

  ArcballCamera::ArcballCamera( PCZSceneManager* scene,
    const Ogre::String& name, const PCZSceneNode* target,
    const Vector3& offset, const Radian& fovy, PCZone* homeZone,
    bool reverseAxes, Real sensitivity, Real minDistance, Real maxDistance,
    Real rotationDecel, Real zoomAccel, Real zoomDecel ):
  Camera( scene, name, target->_getDerivedPosition() + offset,
    fovy, homeZone, reverseAxes ), mSensitivity( sensitivity ),
    pgeTarget( target ), mOffset( offset ),mMinDistance( minDistance ),
    mMaxDistance( maxDistance ), mRotDeceleration( rotationDecel ),
    mZoomAcceleration( zoomAccel ), mZoomDeceleration( zoomDecel ),
    mRotation( Quaternion::IDENTITY ), mMovement( Vector3::ZERO ),
    mZoomVelocity( 0.0f ), mClampTop( 0.6f ), mClampBottom( 1.0f ),
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
    Real distance = mOffset.length();
    if ( distance < mMinDistance ) {
      mOffset = mOffset.normalisedCopy() * mMinDistance;
      mZoomVelocity = 0.0f;
    }
  }

  void ArcballCamera::setMaxDistance( const Real maximumDistance )
  {
    mMaxDistance = maximumDistance;
    Real distance = mOffset.length();
    if ( distance > mMaxDistance ) {
      mOffset = mOffset.normalisedCopy() * mMaxDistance;
      mZoomVelocity = 0.0f;
    }
  }

  void ArcballCamera::setClampTop( const Radian clampTop )
  {
    mClampTop = clampTop;
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

  void ArcballCamera::setClampBottom( const Radian clampBottom )
  {
    mClampBottom = clampBottom;
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