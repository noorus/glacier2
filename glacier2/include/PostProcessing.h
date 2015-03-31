#pragma once
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace HDRlib {
  class HDRCompositor;
}

namespace Glacier {

  class Graphics;

  class PostProcessing {
  friend class Graphics;
  protected:
    HDRlib::HDRCompositor* mHDR;
    SceneManager* mScene;
    RenderWindow* mWindow;
    PostProcessing( SceneManager* scene, RenderWindow* window );
    ~PostProcessing();
  public:
    void setup( Viewport* viewport );
    void update( const GameTime delta );
    void teardown();
  };

}