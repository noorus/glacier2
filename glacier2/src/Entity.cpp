#include "StdAfx.h"
#include "Entity.h"
#include "ServiceLocator.h"
#include "EntityManager.h"
#include "Graphics.h"
#include "JSNatives.h"
#include "World.h"
#include "Scripting.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Entity::Entity( World* world, const EntityBaseData* baseData ):
  mBaseData( baseData ), mWorld( world ), mPosition( Vector3::ZERO ),
  mOrientation( Quaternion::IDENTITY ), mNode( nullptr ),
  mScriptable( nullptr )
  {
    auto isolate = mWorld->getScripting()->getIsolate();
    v8::HandleScope handleScope( isolate );

    auto context = v8::Local<v8::Context>::New( isolate, mWorld->getScripting()->getContext() );

    mScriptable = JS::Entity::create( this, context );
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
    SAFE_DELETE( mScriptable );
    if ( mNode )
    {
      mNode->removeAndDestroyAllChildren();
      Locator::getGraphics().getScene()->destroySceneNode( mNode );
    }
  }

}