#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class PhysicsScene;
  class EntityManager;
  class Scripting;

  struct PropDefinition {
    PhysicsShapeVector physical;
    MeshPtrVector meshes;
  };

  struct StaticProp {
    SceneNode* node;
    physx::PxRigidStatic* physical;
  };

  typedef std::vector<StaticProp> StaticPropVector;

  class World {
  protected:
    EntityManager* mEntities;
    PhysicsScene* mPhysics;
  public:
    World( Engine* engine );
    inline EntityManager* getEntities() const throw( ) { return mEntities; }
    inline PhysicsScene* getPhysics() const throw( ) { return mPhysics; }
    Scripting* getScripting() const throw( );
    ~World();
  };

}