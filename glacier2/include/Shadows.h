#pragma once
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class CSMGpuConstants;
  class StableCSMShadowCameraSetup;

  class Shadows {
  protected:
    CSMGpuConstants* mConstants;
    StableCSMShadowCameraSetup* mSetup;
  public:
    Shadows();
    ~Shadows();
    void applyTo( SceneManager* scene );
  };

}