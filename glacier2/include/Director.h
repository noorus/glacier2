#pragma once
#include "HDRCompositor.h"

// Glacier² Game Engine © 2014 noorus
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
    ArcballCamera* mCamera;     //!< My camera
    Ogre::Light* mLight;
  public:
    static HDRlib::HDRCompositor* mHDRCompositor; //!< My HDR compositor
    //! Constructor.
    Director( Graphics* gfx, const PCZSceneNode* target );
    //! Gets the viewport.
    Ogre::Viewport* getViewport() { return mViewport; }
    //! Gets the camera.
    ArcballCamera* getCamera() { return mCamera; }
    void update( const GameTime delta );
    //! Destructor.
    ~Director();
  };

  //! @}

}