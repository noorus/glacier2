#pragma once

namespace Glacier {

  class Graphics;
  class Camera;

  class Director: boost::noncopyable {
  protected:
    Graphics* mGraphics;
    Ogre::Viewport* mViewport;
    Camera* mCamera;
  public:
    Director( Graphics* gfx );
    ~Director();
  };

}