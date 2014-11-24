#include "StdAfx.h"
#include "Entity.h"
#include "ServiceLocator.h"
#include "EntityManager.h"
#include "Graphics.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Entity::Entity( World* world, const EntityBaseData* baseData ):
  mBaseData( baseData ), mWorld( world ), mPosition( Vector3::ZERO ),
  mOrientation( Quaternion::IDENTITY ), mNode( nullptr )
  {
    //
  }

  void Entity::spawn( const Vector3& position, const Quaternion& orientation )
  {
    mPosition = position;
    mOrientation = orientation;

    auto scm = Locator::getGraphics().getScene();
    mNode = (PCZSceneNode*)scm->getRootSceneNode()->createChildSceneNode( mPosition, mOrientation );
    scm->addPCZSceneNode( mNode, scm->getDefaultZone() );
  }

  void Entity::remove()
  {
    Locator::getEntities().markForRemoval( this );
  }

  Entity::~Entity()
  {
    if ( mNode )
      Locator::getGraphics().getScene()->destroySceneNode( mNode );
  }

}