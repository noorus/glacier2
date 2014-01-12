#pragma once
#include "HDRCompositor.h"

namespace Glacier {

  class Graphics;
  class Camera;

  class Director: boost::noncopyable {
  protected:
    Graphics* mGraphics;
    Ogre::Viewport* mViewport;
    Camera* mCamera;
    HDRlib::HDRCompositor* mHDRCompositor;
  public:
    Director( Graphics* gfx );
    Ogre::Viewport* getViewport() { return mViewport; }
    Camera* getCamera() { return mCamera; }
    ~Director();
  };

}