#include "StdAfx.h"
#include "Camera.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  bool cam_setfov( ConVar* variable, ConVar::Value oldValue )
  {
    if ( Director::mCamera )
      Director::mCamera->setFOVy( Radian( Ogre::Math::DegreesToRadians( variable->getFloat() ) ) );

    return true;
  }

  bool cam_setortho( ConVar* variable, ConVar::Value oldValue )
  {
    if ( Director::mCamera )
      Director::mCamera->setOrthographic( variable->getBool() );

    return true;
  }

  bool cam_setwindow( ConVar* variable, ConVar::Value oldValue )
  {
    if ( Director::mCamera )
      Director::mCamera->setOrthographicWindow( variable->getFloat() );

    return true;
  }

  ENGINE_DECLARE_CONVAR_WITH_CB( cam_fov, L"Camera field of view", 70.0f, cam_setfov );
  ENGINE_DECLARE_CONVAR_WITH_CB( cam_orthographic, L"Camera orthographic mode", true, cam_setortho );
  ENGINE_DECLARE_CONVAR_WITH_CB( cam_window, L"Camera orthographic window size", 24.0f, cam_setwindow );

  Camera::Camera( PCZSceneManager* scene, const Ogre::String& name,
  const Vector3& position, const Radian& fovy, PCZone* homeZone, bool reverseAxes ):
  mCamera( nullptr ), mNode( nullptr ), mZone( homeZone ),
  mReverseAxes( reverseAxes ), mScene( scene ), mModifier( nullptr )
  {
    assert( scene && homeZone );

    // Create camera
    mCamera = (PCZCamera*)mScene->createCamera( name );

    if ( g_CVar_cam_orthographic.getBool() )
    {
      mCamera->setProjectionType( Ogre::PT_ORTHOGRAPHIC );
      mCamera->setOrthoWindowWidth( g_CVar_cam_window.getFloat() );
    }

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

  void Camera::setOrthographic( const bool orthographic )
  {
    mCamera->setProjectionType( orthographic ? Ogre::PT_ORTHOGRAPHIC : Ogre::PT_PERSPECTIVE );
  }

  void Camera::setOrthographicWindow( const Real units )
  {
    mCamera->setOrthoWindowWidth( units );
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

}