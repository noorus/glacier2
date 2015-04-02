#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class World;
  class PhysicsScene;
  class EntityManager;
  struct EntityBaseData;

  namespace JS {
    class Entity;
  }

  class Entity {
  friend class EntityManager;
  private:
    const EntityBaseData* mBaseData;
  protected:
    World* mWorld;
    string mName; //!< Entity name
    PCZSceneNode* mNode; //!< Entity root node
    Vector3 mPosition; //!< World position
    Quaternion mOrientation; //!< World orientation
    JS::Entity* mScriptable;
    bool mRemoval;
    explicit Entity( World* world, const EntityBaseData* baseData );
    virtual ~Entity();
    void setName( const string& name ) { mName = name; }
    void markForRemoval() { mRemoval = true; }
  public:
    inline const JS::Entity* getScriptable() const throw( ) { return mScriptable; }
    inline const EntityBaseData& getBaseData() const throw( ) { return *mBaseData; }
    inline const World* getWorld() const throw( ) { return mWorld; }
    inline const string& getName() const throw( ) { return mName; }
    inline const bool isRemoval() const throw( ) { return mRemoval; }
    inline const Vector3& getPosition() const throw( ) { return mPosition; }
    inline const Quaternion& getOrientation() const throw( ) { return mOrientation; }
    inline const PCZSceneNode* getNode() const throw( ) { return mNode; }
    virtual const Ogre::MovableObject* getMovable() const throw( ) = 0;
    virtual void spawn( const Vector3& position, const Quaternion& orientation );
    virtual void think( const GameTime delta ) = 0;
    void remove();
  };

}