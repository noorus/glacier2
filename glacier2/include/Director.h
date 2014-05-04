#pragma once
#include "HDRCompositor.h"

// Glacier� Game Engine � 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class Graphics;
  class ArcballCamera;

  //! \class Director
  //! The director is in charge of managing the viewport,
  //! camera and post-processing effects.
  class Director: boost::noncopyable {
  protected:
    Graphics* mGraphics;        //!< The graphics engine
    Ogre::Viewport* mViewport;  //!< My viewport
    ArcballCamera* mCamera;            //!< My camera
    HDRlib::HDRCompositor* mHDRCompositor; //!< My HDR compositor
    Ogre::Light* mLight;
  public:
    //! Constructor.
    Director( Graphics* gfx );
    //! Gets the viewport.
    Ogre::Viewport* getViewport() { return mViewport; }
    //! Gets the camera.
    ArcballCamera* getCamera() { return mCamera; }
    //! Destructor.
    ~Director();
  };

  //! @}

}