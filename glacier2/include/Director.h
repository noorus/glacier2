#pragma once
#include "HDRCompositor.h"

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
    Ogre::Viewport* mViewport;  //!< My viewport
    Ogre::Light* mLight;
  public:
    static GameCamera* mCamera; //!< My camera
    //! Constructor.
    Director( Graphics* gfx, const PCZSceneNode* target );
    //! Gets the viewport.
    Ogre::Viewport* getViewport() { return mViewport; }
    //! Gets the camera.
    GameCamera* getCamera() { return mCamera; }
    void update( const GameTime delta );
    //! Destructor.
    ~Director();
  };

  //! @}

}