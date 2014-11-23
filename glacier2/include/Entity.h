#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class PhysicsScene;
  class EntityManager;
  struct EntityBaseData;

  class Entity {
  friend class EntityManager;
  private:
    const EntityBaseData* mBaseData;
  protected:
    string mName;
    bool mRemoval;
    explicit Entity( const EntityBaseData* baseData );
    virtual ~Entity();
    void setName( const string& name ) { mName = name; }
    void markForRemoval() { mRemoval = true; }
  public:
    inline const string& getName() const throw( ) { return mName; }
    inline const bool isRemoval() const throw( ) { return mRemoval; }
    virtual void spawn( PhysicsScene* scene, const Vector3& position ) = 0;
    virtual void think() = 0;
  };

}