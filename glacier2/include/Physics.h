#pragma once
#include "Types.h"
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Physics: public EngineComponent {
  protected:
    physx::PxFoundation* mFoundation;
    physx::PxPhysics* mPhysics;
    physx::PxCooking* mCooking;
    physx::PxCudaContextManager* mCudaContextManager;
    physx::PxControllerManager* mControllerMgr;
  public:
    Physics( Engine* engine );
    void physicsInitialize();
    void physicsShutdown();
    void physicsRestart();
    virtual void componentPreUpdate( GameTime time );
    virtual void componentTick( GameTime tick, GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    virtual ~Physics();
  };

}