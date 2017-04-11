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
    zoomVelocity_( 0.0f ),
    zoom_( 0.0f ), direction_( Vector3::ZERO ), mReverseAxes( true )
  {
    anchor_ = scene->createSceneNode( Ogre::SCENE_DYNAMIC );
    anchor_->setPosition( Vector3::ZERO );
    distance_ = cMaxDistance;
    mCamera->setAutoTracking( false );
    mCamera->setProjectionType( Ogre::PT_ORTHOGRAPHIC );
    angle_ = Radian( Ogre::Math::DegreesToRadians( g_CVar_cam_pitch.getFloat() ) );
    direction_ = ( Quaternion( Ogre::Degree( g_CVar_cam_yaw.getFloat() ), Vector3::UNIT_Y ) * Quaternion( angle_, Vector3::UNIT_Z ) ) * Vector3::UNIT_X;
    lookAt( anchor_->_getDerivedPositionUpdated() );
  }

  void GameCamera::applyRotation( const Vector3& rotation )
  {
    if ( mReverseAxes ) {
      rotationInput_.x += rotation.x * mSensitivity;
      rotationInput_.y += rotation.y * mSensitivity;
    }
    else {
      rotationInput_.x += -rotation.x * mSensitivity;
      rotationInput_.y += -rotation.y * mSensitivity;
    }
    rotationInput_.z += rotation.z;
  }

  void GameCamera::setEdgeScrolling( const Vector2& capped )
  {
    //
  }

  void GameCamera::update( const GameTime delta )
  {
    // Make up the local X axis, as it changes with rotation along world Y
    Vector3 axis = Vector3( -direction_.z, 0, direction_.x );
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
      if ( radiansY > direction_.angleBetween( Vector3::UNIT_Y ) - cMinAngle ) {
        radiansY = direction_.angleBetween( Vector3::UNIT_Y ) - cMinAngle;
        resetRotation = true;
      }
      else if ( radiansY < direction_.angleBetween( Vector3::UNIT_Y ) - cMaxAngle ) {
        radiansY = direction_.angleBetween( Vector3::UNIT_Y ) - cMaxAngle;
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
      direction_ = rotation_ * direction_;
    }

    direction_.normalise();

    // Handle Z mouse scroll (zoom)
    if ( rotationInput_.z )
      zoomVelocity_ += rotationInput_.z;
    if ( zoomVelocity_ != 0.0f )
    {
      zoom_ += zoomVelocity_ * cZoomAcceleration * (Real)delta;
      // TODO:MEDIUM - this could be softened a bit
      if ( zoom_ > 1.0f ) {
        zoom_ = 1.0f;
        zoomVelocity_ = 0.0f;
      }
      else if ( zoom_ < 0.0f ) {
        zoom_ = 0.0f;
        zoomVelocity_ = 0.0f;
      }
      else {
        if ( zoomVelocity_ > 0.0f ) {
          zoomVelocity_ -= (Real)delta * cZoomDeceleration;
          if ( zoomVelocity_ < 0.0f ) zoomVelocity_ = 0.0f;
        }
        else if ( zoomVelocity_ < 0.0f ) {
          zoomVelocity_ += (Real)delta * cZoomDeceleration;
          if ( zoomVelocity_ > 0.0f ) zoomVelocity_ = 0.0f;
        }
      }
      distance_ = Math::interpolateLinear( cMaxDistance, cMinDistance, zoom_ );
    }

    // Reset movement for next frame
    rotationInput_ = Vector3::ZERO;

    // Update camera position
    Vector3 target = anchor_->_getDerivedPositionUpdated();
    Vector3 offset = direction_ * distance_;
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