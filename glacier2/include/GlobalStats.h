#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class GlobalStats {
  protected:
    bool mEnabled;
    Ogre::Overlay* mOverlay;
    Ogre::OverlayElement* mNamesText;
    Ogre::OverlayElement* mValuesText;
  public:
    void init();
    void shutdown();
    void update();
    void setEnabled( bool b );
    GlobalStats(): mOverlay( nullptr ) {}
  };

}