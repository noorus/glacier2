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
#include "Player.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_DECLARE_ENTITY( player, Player );

  Player::Player( World* world ):
  Character( world, &baseData, new PlayerCharacterInputComponent( this ) ),
  mItem( nullptr )
  {
    mHeight = 1.0f;
    mRadius = 0.25f;
  }

  void Player::spawn( const Vector3& position, const Quaternion& orientation )
  {
    Character::spawn( position, orientation );

    mMesh = Procedural::CapsuleGenerator().setHeight( mHeight ).setRadius( mRadius ).realizeMesh( "playerStandCapsule" );

    mItem = Locator::getGraphics().getScene()->createItem( mMesh );
    mItem->setQueryFlags( SceneQueryFlag_Character );
    mItem->setDatablock( "Developer/Placeholder/Player" );
    mItem->setCastShadows( true );
    mNode->attachObject( mItem );

    auto skull = Locator::getGraphics().getScene()->createEntity( "skull_02.mesh" );
    auto snode = mNode->createChildSceneNode( Ogre::SCENE_DYNAMIC, Vector3( 0, 1.5f, 0 ) );
    skull->setQueryFlags( SceneQueryFlag_Camera );
    snode->attachObject( skull );
  }

  void Player::think( const GameTime delta )
  {
    Character::think( delta );
  }

  Ogre::MovableObject* Player::getMovable()
  {
    return mItem;
  }

  Player::~Player()
  {
    if ( mItem )
      Locator::getGraphics().getScene()->destroyItem( mItem );
    if ( !mMesh.isNull() )
      Ogre::MeshManager::getSingleton().remove( mMesh->getHandle() );
  }

}