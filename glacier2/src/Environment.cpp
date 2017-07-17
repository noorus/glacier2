#include "StdAfx.h"
#include "Types.h"
#include "Environment.h"
#include "Graphics.h"
#include "Camera.h"
#include "Engine.h"
#include "InputManager.h"
#include "HDR.h"
#include "ServiceLocator.h"

namespace Glacier {

  Environment::Environment( Graphics* gfx ): scene_( gfx->getScene() ), sunLight_( nullptr ), sunNode_( nullptr ), running_( false )
  {
    upperHemisphere_ = Ogre::ColourValue::Blue;
    lowerHemisphere_ = Ogre::ColourValue::Green;
    ambientScale_ = 1.0f;
  }

  void Environment::setUpperHemisphere( const ColourValue& value )
  {
    upperHemisphere_ = value;
  }

  void Environment::setLowerHemisphere( const ColourValue& value )
  {
    lowerHemisphere_ = value;
  }

  void Environment::setHemisphereScale( const Real scale )
  {
    ambientScale_ = scale;
  }

  void Environment::setSunPower( Real power )
  {
    sun_.power_ = power;
  }

  void Environment::setSunDiffuse( const ColourValue& color )
  {
    sun_.diffuse_ = color;
  }

  void Environment::setSunSpecular( const ColourValue& color )
  {
    sun_.specular_ = color;
  }

  void Environment::setSunDirection( const Vector3& dir )
  {
    sun_.direction_ = dir;
  }

  void Environment::setSun( Real power, const ColourValue& diffuse, const ColourValue& specular, const Vector3& direction )
  {
    sun_.power_ = power;
    sun_.diffuse_ = diffuse;
    sun_.specular_ = specular;
    sun_.direction_ = direction;
  }

  void Environment::update( const GameTime delta )
  {
    if ( !running_ )
      return;

    scene_->setAmbientLight( upperHemisphere_, lowerHemisphere_, Vector3::UNIT_Y, ambientScale_ );
    sunLight_->setPowerScale( sun_.power_ );
    sunLight_->setDiffuseColour( sun_.diffuse_ );
    sunLight_->setSpecularColour( sun_.specular_ );
    sunLight_->setDirection( sun_.direction_ );
  }

  void Environment::initialize()
  {
    sunNode_ = scene_->getRootSceneNode( Ogre::SCENE_DYNAMIC )->createChildSceneNode( Ogre::SCENE_DYNAMIC );
    sunNode_->setPosition( 0.0f, 20.0f, -10.0f );

    sunLight_ = scene_->createLight();
    sunNode_->attachObject( sunLight_ );
    sunLight_->setPowerScale( 10.0f );
    sunLight_->setType( Ogre::Light::LT_DIRECTIONAL );
    sunLight_->setCastShadows( true );
    sunLight_->setShadowFarClipDistance( 100.0f );
    sunLight_->setDirection( Vector3( -1.0f, -1.0f, -1.0f ).normalisedCopy() );
    sunLight_->setDiffuseColour( 244.0f / 255.0f * 0.2f, 245.0f / 255.0f * 0.2f, 237.0f / 255.0f * 0.2f );
    sunLight_->setSpecularColour( 244.0f / 255.0f, 245.0f / 255.0f, 237.0f / 255.0f );

    running_ = true;
  }

  void Environment::shutdown()
  {
    if ( running_ ) {
      scene_->destroySceneNode( sunNode_ );
      scene_->destroyLight( sunLight_ );
    }
    running_ = false;
  }

  void Environment::reset()
  {
  }

  Environment::~Environment()
  {
  }

}