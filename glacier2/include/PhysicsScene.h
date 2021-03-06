#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class PhysXPhysics;
  class PhysicsDebugVisualizer;

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
    physx::PxControllerManager* mControllerMgr;
    PhysicsScene( PhysXPhysics* physics,
      physx::PxCpuDispatcher* cpuDispatcher, physx::PxGpuDispatcher* gpuDispatcher,
      const float gravity, const float restitution,
      const float staticFriction, const float dynamicFriction );
    void simulationStep( const GameTime delta, const GameTime time );
    void simulationFetchResults();
    void post();
#ifndef GLACIER_NO_PHYSICS_DEBUG
    PhysicsDebugVisualizer* mVisualizer;
    void debugFetchVisualization();
#else
    void* mVisualizer;
#endif
  public:
    inline physx::PxScene* getScene() const throw() { return mScene; }
    inline physx::PxControllerManager* getControllerManager() const throw() { return mControllerMgr; }
    float setGravity( const float gravity );
    physx::PxMaterial* getDefaultMaterial() const { return mDefaultMaterial; }
    const float getGravity() const throw() { return -mGravity.y; }
    const Vector3& getGravityVector() const throw() { return mGravity; }
    float setRestitution( const float restitution );
    float setStaticFriction( const float staticFriction );
    float setDynamicFriction( const float dynamicFriction );
    ~PhysicsScene();
#ifndef GLACIER_NO_PHYSICS_DEBUG
    void setDebugVisuals( const bool visuals );
    const physx::PxRenderBuffer& fetchDebugVisuals();
#endif
  };

}