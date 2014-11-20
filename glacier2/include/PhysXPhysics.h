#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class PhysXPhysics: public Physics, public EngineComponent, public PxErrorCallback {
  protected:
    class Allocator: public PxAllocatorCallback {
    public:
      virtual void* allocate( size_t size, const char* typeName,
        const char* filename, int line );
      virtual void deallocate( void* ptr );
    } mAllocator;
    physx::PxFoundation* mFoundation;
    physx::PxPhysics* mPhysics;
    physx::PxCooking* mCooking;
    physx::PxCudaContextManager* mCudaContextManager;
    physx::PxControllerManager* mControllerMgr;
    virtual void reportError( PxErrorCode::Enum code,
      const char* message, const char* file, int line );
  public:
    PhysXPhysics( Engine* engine );
    virtual void initialize();
    virtual void shutdown();
    virtual void restart();
    virtual void componentPreUpdate( GameTime time );
    virtual void componentTick( GameTime tick, GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    virtual ~PhysXPhysics();
  };

}