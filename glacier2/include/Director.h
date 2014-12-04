#pragma once
#include "HDRCompositor.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class Graphics;
  class ArcballCamera;
  class CSMGpuConstants;
  class StableCSMShadowCameraSetup;

  //! \class Director
  //! The director is in charge of managing the viewport,
  //! camera and post-processing effects.
  class Director: boost::noncopyable {
  protected:
    Graphics* mGraphics;        //!< The graphics engine
    Ogre::Viewport* mViewport;  //!< My viewport
    Ogre::Light* mLight;
    CSMGpuConstants* mShadowConstants;
    StableCSMShadowCameraSetup* mShadowSetup;
  public:
    static HDRlib::HDRCompositor* mHDRCompositor; //!< My HDR compositor
    static ArcballCamera* mCamera; //!< My camera
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