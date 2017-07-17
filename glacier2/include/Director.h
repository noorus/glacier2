#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class Graphics;
  class GameCamera;
  class HDR;

  //! \class Director
  class Director: boost::noncopyable {
  protected:
    Graphics* mGraphics; //!< The graphics engine
    Ogre::Light* mLight;
    CompositorWorkspace* mWorkspace;
    HDR* mHDR;
  public:
    static GameCamera* mCamera; //!< My camera
    //! Constructor.
    Director( Graphics* gfx );
    //! Gets the camera.
    GameCamera* getCamera() { return mCamera; }
    void update( const GameTime delta );
    //! Destructor.
    ~Director();
  };

  //! @}

}