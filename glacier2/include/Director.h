#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class Graphics;
  class GameCamera;

  //! \class Director
  class Director: boost::noncopyable {
  protected:
    Graphics* mGraphics;        //!< The graphics engine
    Ogre::Light* mLight;
    CompositorWorkspace* mWorkspace;
  public:
    static GameCamera* mCamera; //!< My camera
    //! Constructor.
    Director( Graphics* gfx, SceneNode* target );
    //! Gets the camera.
    GameCamera* getCamera() { return mCamera; }
    void update( const GameTime delta );
    //! Destructor.
    ~Director();
  };

  //! @}

}