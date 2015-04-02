#pragma once
#include "Types.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace HDRlib {
  class HDRCompositor;
}

namespace Glacier {

  class Graphics;

  ENGINE_EXTERN_CONVAR( hdr_enabled );

  class PostProcessing {
  friend class Graphics;
  protected:
    HDRlib::HDRCompositor* mHDR;
    SceneManager* mScene;
    RenderWindow* mWindow;
    Viewport* mViewport;
    bool mHDREnabled;
    PostProcessing( SceneManager* scene, RenderWindow* window );
    ~PostProcessing();
    void checkAndUpdateState();
  public:
    static bool callbackHDREnabled( ConVar* variable, ConVar::Value oldValue );
  public:
    void setup( Viewport* viewport );
    void update( const GameTime delta );
    void setHDREnabled( const bool enabled );
    void teardown();
  };

}