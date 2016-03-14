#include "StdAfx.h"
#include "Camera.h"
#include "GlacierMath.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const Real cRotationDeceleration = 20.0f;
  const Real cZoomAcceleration = 0.25f;
  const Real cZoomDeceleration = 4.0f;
  const Radian cFOVy = Radian( Ogre::Math::DegreesToRadians( 70.0f ) );
  const Real cMaxDistance = 15.0f;
  const Real cMinDistance = 2.0f;
  const Real cSensitivity = 0.5f;
  const Radian cClampTop = Radian( 0.6f );
  const Radian cClampBottom = Radian( 1.0f );

  const Radian cAngle = Radian( Ogre::Math::DegreesToRadians( 10.0f ) );
  const Radian cMinAngle = Radian( Ogre::Math::DegreesToRadians( 8.0f ) );
  const Radian cMaxAngle = Radian( Ogre::Math::DegreesToRadians( 80.0f ) );

  GameCamera::GameCamera( SceneManager* scene,
    const Ogre::String& name, SceneNode* target ):
    Camera( scene, name, Vector3::ZERO, cFOVy ),
    mSensitivity( cSensitivity ),
    mTarget( target ),
    mRotation( Quaternion::IDENTITY ), mMovement( Vector3::ZERO ),
    mZoomVelocity( 0.0f ),
    mZoom( 0.0f ), mDirection( Vector3::ZERO ), mReverseAxes( true )
  {
    mDistance = cMaxDistance;
    mCamera->setAutoTracking( false );
    mCamera->setProjectionType( Ogre::PT_PERSPECTIVE );
    mAngle = cAngle;
    mDirection = Quaternion( mAngle, Vector3::UNIT_Z ) * Vector3::UNIT_X;
    lookAt( mTarget->_getDerivedPositionUpdated() );
  }

  void GameCamera::applyMovement( const Vector3& movement )
  {
    if ( mReverseAxes ) {
      mMovement.x += movement.x * mSensitivity;
      mMovement.y += movement.y * mSensitivity;
    }
    else {
      mMovement.x += -movement.x * mSensitivity;
      mMovement.y += -movement.y * mSensitivity;
    }
    mMovement.z += movement.z;
  }

  void GameCamera::update( const GameTime delta )
  {
    // Make up the local X axis, as it changes with rotation along world Y
    Vector3 axis = Vector3( -mDirection.z, 0, mDirection.x );
    axis.normalise();

    // Handle new X,Y movement
    if ( mMovement.x != 0.0f || mMovement.y != 0.0f )
    {
      // Treat the movement input as degrees, and convert to radians
      Radian radiansX = Radian( Ogre::Math::DegreesToRadians( mMovement.x ) );
      Radian radiansY = Radian( Ogre::Math::DegreesToRadians( mMovement.y ) );
      bool resetRotation = false;
      if ( radiansY > mDirection.angleBetween( Vector3::UNIT_Y ) - cClampTop ) {
        radiansY = mDirection.angleBetween( Vector3::UNIT_Y ) - cClampTop;
        resetRotation = true;
      }
      else if ( radiansY < mDirection.angleBetween( Vector3::UNIT_Y ) - cClampBottom ) {
        radiansY = mDirection.angleBetween( Vector3::UNIT_Y ) - cClampBottom;
        resetRotation = true;
      }
      // Compose the rotation delta and add to existing velocity
      Quaternion rotationX( radiansY, axis );
      Quaternion rotationY( radiansX, Vector3::UNIT_Y );
      if ( resetRotation )
        mRotation = rotationX * rotationY;
      else
        mRotation = mRotation * rotationX * rotationY;
    }

    // Handle rotation velocity
    if ( mRotation != Quaternion::IDENTITY )
    {
      mRotation = mRotation.Slerp( cRotationDeceleration * (Real)delta, mRotation,
        Quaternion::IDENTITY, true );
      mDirection = mRotation * mDirection;
    }

    mDirection.normalise();

    // Handle Z mouse scroll (zoom)
    if ( mMovement.z )
      mZoomVelocity += mMovement.z;
    if ( mZoomVelocity != 0.0f )
    {
      mZoom += mZoomVelocity * cZoomAcceleration * (Real)delta;
      // TODO:MEDIUM - this could be softened a bit
      if ( mZoom > 1.0f ) {
        mZoom = 1.0f;
        mZoomVelocity = 0.0f;
      }
      else if ( mZoom < 0.0f ) {
        mZoom = 0.0f;
        mZoomVelocity = 0.0f;
      }
      else {
        if ( mZoomVelocity > 0.0f ) {
          mZoomVelocity -= (Real)delta * cZoomDeceleration;
          if ( mZoomVelocity < 0.0f ) mZoomVelocity = 0.0f;
        }
        else if ( mZoomVelocity < 0.0f ) {
          mZoomVelocity += (Real)delta * cZoomDeceleration;
          if ( mZoomVelocity > 0.0f ) mZoomVelocity = 0.0f;
        }
      }
      mDistance = Math::interpolateLinear( cMaxDistance, cMinDistance, mZoom );
    }

    // Reset movement for next frame
    mMovement = Vector3::ZERO;

    // Update camera position
    Vector3 target = mTarget->_getDerivedPositionUpdated();
    Vector3 offset = mDirection * mDistance;
    Vector3 position = target + offset;
    if ( mModifier )
      mModifier->updatePosition( this, target, offset, position );
    mNode->setPosition( position );

    // Always look at our target
    lookAt( target );
  }

  void GameCamera::setSensitivity( const Real sensitivity )
  {
    mSensitivity = sensitivity;
  }

  void GameCamera::setAxisReversion( const bool reverse )
  {
    mReverseAxes = reverse;
  }

  GameCamera::~GameCamera()
  {
    // Stubb
  }

}