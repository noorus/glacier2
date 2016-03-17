#pragma once
#include "Types.h"

namespace Glacier {

  class HDR {
  protected:
    Ogre::CompositorManager2* mCompositorManager;
    Ogre::MaterialManager* mMaterialManager;
  public:
    HDR( Ogre::CompositorManager2* compositorManager, Ogre::MaterialManager* materialManager );
    void setSkyColor( const ColourValue& color, Real multiplier );
    void setExposure( Real exposure, Real minAutoExposure, Real maxAutoExposure );
    void setBloomThreshold( Real minThreshold, Real fullColorThreshold );
  };

}