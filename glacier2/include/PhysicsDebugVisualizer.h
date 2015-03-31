#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

#ifndef GLACIER_NO_PHYSICS_DEBUG

namespace Glacier {

  class PhysicsDebugVisualizer {
  protected:
    Engine* mEngine;
    PCZSceneManager* mScene;
    PCZSceneNode* mNode;
    ManualObject* mManualObject;
    ManualObject* mGrid;
    Ogre::MaterialPtr mMaterial;
    float mAlpha;
  public:
    explicit PhysicsDebugVisualizer( Engine* engine );
    void clearDebugScene();
    void drawDebugScene( const physx::PxRenderBuffer* renderable );
    ~PhysicsDebugVisualizer();
  };

}

#endif