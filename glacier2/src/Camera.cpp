﻿#include "StdAfx.h"
#include "Camera.h"

namespace Glacier {

  Camera::Camera( PCZSceneManager* scene, const Ogre::String& name,
  const Vector3& position, const Radian& fovy, PCZone* homeZone ):
  mCamera( nullptr ), mNode( nullptr ), mZone( homeZone ),
  mReverseAxes( false ), mScene( scene )
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

    mCamera->setNearClipDistance( 0.1f );
    mCamera->setFarClipDistance( 4096.0f );
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