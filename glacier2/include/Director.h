#pragma once
#include "HDRCompositor.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class Graphics;
  class Camera;

  //! \class Director
  //! The director is in charge of managing the viewport,
  //! camera and post-processing effects.
  class Director: boost::noncopyable {
  protected:
    Graphics* mGraphics;        //!< The graphics engine
    Ogre::Viewport* mViewport;  //!< My viewport
    Camera* mCamera;            //!< My camera
    HDRlib::HDRCompositor* mHDRCompositor; //!< My HDR compositor
  public:
    //! Constructor.
    Director( Graphics* gfx );
    //! Gets the viewport.
    Ogre::Viewport* getViewport() { return mViewport; }
    //! Gets the camera.
    Camera* getCamera() { return mCamera; }
    //! Destructor.
    ~Director();
  };

  //! @}

}