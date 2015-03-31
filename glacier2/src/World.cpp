#include "StdAfx.h"
#include "Entity.h"
#include "ServiceLocator.h"
#include "EntityManager.h"
#include "World.h"
#include "PhysXPhysics.h"
#include "PhysicsScene.h"
#include "Engine.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  World::World( Engine* engine ):
  mEntities( nullptr ), mPhysics( nullptr )
  {
    mEntities = new EntityManager( engine, this );
    mPhysics = engine->getPhysics()->createScene();
#ifndef GLACIER_NO_PHYSICS_DEBUG
    mPhysics->setDebugVisuals( true );
#endif
  }

  World::~World()
  {
    SAFE_DELETE( mEntities );
    gEngine->getPhysics()->destroyScene( mPhysics );
  }

}