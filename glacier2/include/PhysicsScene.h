#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class PhysXPhysics;

  class PhysicsScene {
  friend class PhysXPhysics;
  protected:
    PhysXPhysics* mPhysics;
    physx::PxScene* mScene;
    physx::PxMaterial* mDefaultMaterial;
    Ogre::Vector3 mGravity;
    physx::PxCpuDispatcher* mCPUDispatcher;
    physx::PxGpuDispatcher* mGPUDispatcher;
    physx::PxSimulationStatistics mStatistics;
    PhysicsScene( PhysXPhysics* physics,
      physx::PxCpuDispatcher* cpuDispatcher, physx::PxGpuDispatcher* gpuDispatcher,
      const float gravity, const float restitution,
      const float staticFriction, const float dynamicFriction );
    void simulationStep( const GameTime delta, const GameTime time );
    void simulationFetchResults();
    void debugFetchVisualization();
  public:
    void setDebugVisuals( const bool visuals );
    const physx::PxRenderBuffer& fetchDebugVisuals();
    inline physx::PxScene* getScene() { return mScene; }
    float setGravity( const float gravity );
    physx::PxMaterial* getDefaultMaterial() const { return mDefaultMaterial; }
    const float getGravity() const { return -mGravity.y; }
    const Vector3& getGravityVector() const { return mGravity; }
    float setRestitution( const float restitution );
    float setStaticFriction( const float staticFriction );
    float setDynamicFriction( const float dynamicFriction );
    ~PhysicsScene();
  };

}