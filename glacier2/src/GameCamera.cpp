#include "StdAfx.h"
#include "Camera.h"
#include "GlacierMath.h"
#include "GlacierCustomPass.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const Real cRotationDeceleration = 20.0f;
  const Real cZoomAcceleration = 0.25f;
  const Real cZoomDeceleration = 4.0f;
  const Radian cFOVy = Radian( Ogre::Math::DegreesToRadians( 80.0f ) );
  const Real cMaxDistance = 128.0f;
  const Real cMinDistance = 32.0f;
  const Real cSensitivity = 0.25f;

  const Radian cMinAngle = Radian( Ogre::Math::DegreesToRadians( 10.0f ) );
  const Radian cMaxAngle = Radian( Ogre::Math::DegreesToRadians( 150.0f ) );

  ENGINE_DECLARE_CONVAR( cam_pitch, L"Camera pitch angle in degrees", 30.0f );
  ENGINE_DECLARE_CONVAR( cam_yaw, L"Camera yaw angle in degrees", -135.0f );
  ENGINE_DECLARE_CONVAR( cam_window, L"Camera orthogonal window", 22.0f );

  GameCamera::GameCamera( SceneManager* scene,
    const Ogre::String& name, Gorilla::Screen* hud ):
    Camera( scene, name, Vector3::ZERO, cFOVy, hud ),
    mSensitivity( cSensitivity ),
    anchor_( nullptr ),
    rotation_( Quaternion::IDENTITY ), rotationInput_( Vector3::ZERO ),
    mZoomVelocity( 0.0f ),
    mZoom( 0.0f ), mDirection( Vector3::ZERO ), mReverseAxes( true )
  {
    anchor_ = scene->createSceneNode( Ogre::SCENE_DYNAMIC );
    anchor_->setPosition( Vector3::ZERO );
    distance_ = cMaxDistance;
    mCamera->setAutoTracking( false );
    mCamera->setProjectionType( Ogre::PT_ORTHOGRAPHIC );
    angle_ = Radian( Ogre::Math::DegreesToRadians( g_CVar_cam_pitch.getFloat() ) );
    mDirection = ( Quaternion( Ogre::Degree( g_CVar_cam_yaw.getFloat() ), Vector3::UNIT_Y ) * Quaternion( angle_, Vector3::UNIT_Z ) ) * Vector3::UNIT_X;
    lookAt( anchor_->_getDerivedPositionUpdated() );
  }

  void GameCamera::applyMovement( const Vector3& movement )
  {
    if ( mReverseAxes ) {
      rotationInput_.x += movement.x * mSensitivity;
      rotationInput_.y += movement.y * mSensitivity;
    }
    else {
      rotationInput_.x += -movement.x * mSensitivity;
      rotationInput_.y += -movement.y * mSensitivity;
    }
    rotationInput_.z += movement.z;
  }

  void GameCamera::update( const GameTime delta )
  {
    // Make up the local X axis, as it changes with rotation along world Y
    Vector3 axis = Vector3( -mDirection.z, 0, mDirection.x );
    axis.normalise();

    auto window = g_CVar_cam_window.getFloat();
    mCamera->setOrthoWindow( window, window );

    // Handle new X,Y movement
    if ( rotationInput_.x != 0.0f || rotationInput_.y != 0.0f )
    {
      // Treat the movement input as degrees, and convert to radians
      Radian radiansX = Radian( Ogre::Math::DegreesToRadians( rotationInput_.x ) );
      Radian radiansY = Radian( Ogre::Math::DegreesToRadians( rotationInput_.y ) );
      bool resetRotation = false;
      if ( radiansY > mDirection.angleBetween( Vector3::UNIT_Y ) - cMinAngle ) {
        radiansY = mDirection.angleBetween( Vector3::UNIT_Y ) - cMinAngle;
        resetRotation = true;
      }
      else if ( radiansY < mDirection.angleBetween( Vector3::UNIT_Y ) - cMaxAngle ) {
        radiansY = mDirection.angleBetween( Vector3::UNIT_Y ) - cMaxAngle;
        resetRotation = true;
      }
      // Compose the rotation delta and add to existing velocity
      Quaternion rotationX( radiansY, axis );
      Quaternion rotationY( radiansX, Vector3::UNIT_Y );
      if ( resetRotation )
        rotation_ = rotationX * rotationY;
      else
        rotation_ = rotation_ * rotationX * rotationY;
    }

    // Handle rotation velocity
    if ( rotation_ != Quaternion::IDENTITY )
    {
      rotation_ = rotation_.Slerp( cRotationDeceleration * (Real)delta, rotation_,
        Quaternion::IDENTITY, true );
      mDirection = rotation_ * mDirection;
    }

    mDirection.normalise();

    // Handle Z mouse scroll (zoom)
    if ( rotationInput_.z )
      mZoomVelocity += rotationInput_.z;
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
      distance_ = Math::interpolateLinear( cMaxDistance, cMinDistance, mZoom );
    }

    // Reset movement for next frame
    rotationInput_ = Vector3::ZERO;

    // Update camera position
    Vector3 target = anchor_->_getDerivedPositionUpdated();
    Vector3 offset = mDirection * distance_;
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
    if ( anchor_ )
    {
      anchor_->detachAllObjects();
      mScene->destroySceneNode( anchor_ );
    }
  }

}