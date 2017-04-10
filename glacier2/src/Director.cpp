#include "StdAfx.h"
#include "Director.h"
#include "Graphics.h"
#include "Camera.h"
#include "Engine.h"
#include "InputManager.h"
#include "HDR.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  GameCamera* Director::mCamera = nullptr;

  Director::Director( Graphics* gfx, SceneNode* target ):
  mGraphics( gfx ), mLight( nullptr ), mWorkspace( nullptr ), mHDR( nullptr )
  {
    mCamera = new GameCamera( mGraphics->getScene(), "defaultcamera", mGraphics->mGorillaScreen );

    gEngine->getInput()->getLocalController()->setCamera( mCamera );

    mWorkspace = mGraphics->createGameWorkspace( mCamera );

    mHDR = new HDR( mWorkspace->getCompositorManager(), Ogre::MaterialManager::getSingletonPtr() );

    auto scene = mGraphics->getScene();

    Ogre::Light* sun = scene->createLight();
    auto node = scene->getRootSceneNode( Ogre::SCENE_DYNAMIC )->createChildSceneNode( Ogre::SCENE_DYNAMIC );
    node->attachObject( sun );
    sun->setPowerScale( 10.0f );
    sun->setType( Ogre::Light::LT_DIRECTIONAL );
    sun->setDirection( Vector3( -1.0f, -1.0f, -1.0f ).normalisedCopy() );
    sun->setCastShadows( false );
    sun->setShadowFarClipDistance( 100.0f );
    sun->setDiffuseColour( 244.0f / 255.0f * 0.2f, 245.0f / 255.0f * 0.2f, 237.0f / 255.0f * 0.2f );
    sun->setSpecularColour( 244.0f / 255.0f, 245.0f / 255.0f, 237.0f / 255.0f );
    node->setPosition( 0.0f, 20.0f, -10.0f );

    mHDR->setSkyColor( Ogre::ColourValue( 244.0f / 255.0f, 245.0f / 255.0f, 237.0f / 255.0f )  * 6.0f, 1.0f );
    mHDR->setExposure( 0.4f, 1.0f, 2.5f );
    mHDR->setBloomThreshold( Ogre::max( 3.0f - 2.0f, 0.0f ), Ogre::max( 3.0f, 0.01f ) );

    mGraphics->getScene()->setAmbientLight(
      Ogre::ColourValue( 0.3f, 0.50f, 0.7f ) * 0.1f * 0.5f,
      Ogre::ColourValue( 0.6f, 0.45f, 0.3f ) * 0.065f * 0.5f,
      -sun->getDirection() + Ogre::Vector3::UNIT_Y * 0.2f,
      1.0f );

    auto light = scene->createLight();
    auto lightNode = scene->getRootSceneNode( Ogre::SCENE_DYNAMIC )->createChildSceneNode( Ogre::SCENE_DYNAMIC );
    lightNode->attachObject( light );
    light->setDiffuseColour( 0.8f, 0.4f, 0.2f ); //Warm
    light->setSpecularColour( 0.8f, 0.4f, 0.2f );
    light->setPowerScale( Ogre::Math::PI );
    light->setType( Ogre::Light::LT_SPOTLIGHT );
    lightNode->setPosition( -10.0f, 10.0f, 10.0f );
    light->setDirection( Ogre::Vector3( 1, -1, -1 ).normalisedCopy() );
    light->setAttenuationBasedOnRadius( 10.0f, 0.01f );

    light = scene->createLight();
    lightNode = scene->getRootSceneNode( Ogre::SCENE_DYNAMIC )->createChildSceneNode( Ogre::SCENE_DYNAMIC );
    lightNode->attachObject( light );
    light->setDiffuseColour( 0.2f, 0.4f, 0.8f ); //Cold
    light->setSpecularColour( 0.2f, 0.4f, 0.8f );
    light->setPowerScale( Ogre::Math::PI );
    light->setType( Ogre::Light::LT_SPOTLIGHT );
    lightNode->setPosition( 10.0f, 10.0f, -10.0f );
    light->setDirection( Ogre::Vector3( -1, -1, 1 ).normalisedCopy() );
    light->setAttenuationBasedOnRadius( 10.0f, 0.01f );
  }

  void Director::update( const GameTime delta )
  {
    mCamera->update( delta );
  }

  Director::~Director()
  {
    mGraphics->getScene()->destroyAllLights();

    SAFE_DELETE( mHDR );

    if ( mWorkspace )
    {
      mGraphics->destroyGameWorkspace( mWorkspace );
      mWorkspace = nullptr;
    }

    gEngine->getInput()->getLocalController()->setCamera( nullptr );

    SAFE_DELETE( mCamera );
  }

}