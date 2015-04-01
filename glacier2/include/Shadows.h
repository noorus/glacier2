#pragma once
#include "Types.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class CSMGpuConstants;
  class StableCSMShadowCameraSetup;

  class Shadows {
  protected:
    CSMGpuConstants* mConstants;
    Ogre::ShadowCameraSetupPtr mSetup;
    SceneManager* mScene;
    bool mEnabled;
    void setup();
    void teardown();
    void checkAndUpdateState();
  public:
    static bool callbackEnabled( ConVar* variable, ConVar::Value oldValue );
  public:
    Shadows();
    ~Shadows();
    void applyTo( SceneManager* scene );
    void setEnabled( const bool enabled );
  };

}