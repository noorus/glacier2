#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "PhysicsScene.h"
#include "PhysicsDebugVisualizer.h"
#include "Graphics.h"
#include "GlacierMath.h"
#include "Character.h"
#include "World.h"
#include "Entity.h"
#include "Actions.h"
#include "InputManager.h"
#include "Dummy.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_DECLARE_ENTITY( dev_dummy, Dummy );

  Dummy::Dummy( World* world ): Character( world, &baseData ),
  mEntity( nullptr )
  {
    mHeight = 0.8f;
    mRadius = 0.2f;
  }

  void Dummy::spawn( const Vector3& position, const Quaternion& orientation )
  {
    Character::spawn( position, orientation );

    mMesh = Procedural::CapsuleGenerator().setHeight( mHeight ).setRadius( mRadius ).realizeMesh( "dummyStandCapsule" );

    mEntity = Locator::getGraphics().getScene()->createEntity( mMesh );
    mEntity->setMaterialName( "Developer/Placeholder/Dummy" );
    mEntity->setCastShadows( true );
    mNode->attachObject( mEntity );
  }

  void Dummy::think( const GameTime delta )
  {
    Character::think( delta );
  }

  const Ogre::MovableObject* Dummy::getMovable() const
  {
    return mEntity;
  }

  Dummy::~Dummy()
  {
    if ( mEntity )
      Locator::getGraphics().getScene()->destroyEntity( mEntity );
  }

}