#pragma once
#include "Entity.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class World;

  typedef std::list<Entity*> EntityList;

  class EntityManager: public EngineComponent {
  protected:
    World* mWorld;
    uint64_t mNamingCounter;
    EntityList mEntities;
    EntityList mThinkers;
    EntityList mRemovals;
    string nextEntityName();
    void addThinker( Entity* entity );
    void removeThinker( Entity* entity );
    void remove( Entity* entity );
    void remove( const string& name );
  public:
    EntityManager( Engine* engine, World* world );
    Entity* create( const string& className, const string& name );
    Entity* create( const string& className );
    void markForRemoval( Entity* entity );
    void removeMarked();
    void clear();
    Entity* findByName( const string& name );
    virtual void componentPreUpdate( GameTime time );
    virtual void componentTick( GameTime tick, GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    virtual ~EntityManager();
  };

}