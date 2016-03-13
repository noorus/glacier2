#include "StdAfx.h"
#include "Engine.h"
#include "Exception.h"
#include "EntityManager.h"
#include "EntityRegistry.h"
#include "Entity.h"
#include "World.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  EntityManager::EntityManager( Engine* engine, World* world ):
  EngineComponent( engine ),
  mNamingCounter( 0 ), mWorld( world )
  {
    //
  }

  string EntityManager::nextEntityName()
  {
    char name[64];
    sprintf_s( name, 64, "entity_%I64u", mNamingCounter );
    mNamingCounter++;
    return name;
  }

  Entity* EntityManager::create( const string& className, const string& name )
  {
    auto record = EntityRegistry::instance().lookup( className );
    if ( !record )
      ENGINE_EXCEPT( "Cannot create entity, unknown class" );

    if ( findByName( name ) )
      ENGINE_EXCEPT( "Cannot create entity, name is already in use" );

    auto entity = record->factory( mWorld );
    entity->setName( name );
    mEntities.push_back( entity );

    addThinker( entity );

    return entity;
  }

  Entity* EntityManager::create( const string& className )
  {
    return create( className, nextEntityName() );
  }

  void EntityManager::componentPreUpdate( GameTime time )
  {
    //
  }

  void EntityManager::componentTick( GameTime tick, GameTime time )
  {
    // Remove entities that have been marked for removal
    removeMarked();
    // Run entity think functions
    for ( auto entity : mThinkers )
      entity->think( tick );
  }

  void EntityManager::componentPostUpdate( GameTime delta, GameTime time )
  {
    //
  }

  Entity* EntityManager::findByName( const string& name )
  {
    for ( auto entity : mEntities )
      if ( entity->getName() == name )
        return entity;

    return nullptr;
  }

  void EntityManager::addThinker( Entity* entity )
  {
    mThinkers.push_back( entity );
  }

  void EntityManager::removeThinker( Entity* entity )
  {
    mThinkers.remove( entity );
  }

  void EntityManager::remove( Entity* entity )
  {
    removeThinker( entity );
    mEntities.remove( entity );
    delete entity;
  }

  void EntityManager::remove( const string& name )
  {
    auto entity = findByName( name );
    if ( entity )
      remove( entity );
  }

  void EntityManager::markForRemoval( Entity* entity )
  {
    entity->markForRemoval();
    mRemovals.push_back( entity );
  }

  void EntityManager::removeMarked()
  {
    for ( auto entity : mRemovals )
      remove( entity );
    mRemovals.clear();
  }

  void EntityManager::clear()
  {
    removeMarked();
    for ( auto entity : mEntities )
    {
      removeThinker( entity );
      delete entity;
    }
    mEntities.clear();
    mNamingCounter = 0;
  }

  EntityManager::~EntityManager()
  {
    clear();
  }

}