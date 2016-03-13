#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class GlobalStats {
  protected:
    bool mEnabled;
    Ogre::v1::Overlay* mOverlay;
    Ogre::v1::OverlayElement* mNamesText;
    Ogre::v1::OverlayElement* mValuesText;
  public:
    void init();
    void shutdown();
    void update();
    void setEnabled( bool b );
    GlobalStats(): mOverlay( nullptr ) {}
  };

}