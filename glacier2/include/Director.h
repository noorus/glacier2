#pragma once

namespace Glacier {

  class Graphics;

  class Director: boost::noncopyable {
  protected:
    Graphics* mGraphics;
    Ogre::Viewport mViewport;
  public:
    Director( Graphics* gfx );
    ~Director();
  };

}