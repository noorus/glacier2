#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_EXTERN_CONVAR( px_threads );
  ENGINE_EXTERN_CONVAR( px_cuda );

  class PhysicsScene;

  class PhysXPhysics: public EngineComponent, public physx::PxErrorCallback {
  protected:
    class Allocator: public physx::PxAllocatorCallback {
    public:
      virtual void* allocate( size_t size, const char* typeName,
        const char* filename, int line );
      virtual void deallocate( void* ptr );
    } mAllocator;
    physx::PxFoundation* mFoundation;
    physx::PxPhysics* mPhysics;
    physx::PxCooking* mCooking;
    physx::PxDefaultCpuDispatcher* mCPUDispatcher;
    std::list<PhysicsScene*> mScenes;
    virtual void reportError( physx::PxErrorCode::Enum code,
      const char* message, const char* file, int line );
  public:
    PhysXPhysics( Engine* engine );
    void initialize();
    void shutdown();
    void restart();
    physx::PxPhysics* getPhysics();
    PhysicsScene* createScene();
    physx::PxCooking* getCooking();
    void destroyScene( PhysicsScene* scene );
    virtual void componentPreUpdate( GameTime time );
    virtual void componentTick( GameTime tick, GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    virtual ~PhysXPhysics();
  };

}