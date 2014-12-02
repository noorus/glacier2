#pragma once
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class MovableTextOverlay {
  protected:
    const Ogre::String mName;
    const Ogre::MovableObject* mTarget;
    const Ogre::Camera* mCamera;
    Ogre::FontPtr mFont;
    Ogre::String mCaption;
    Ogre::Overlay* mOverlay;
    Ogre::OverlayContainer* mContainer;
    Ogre::OverlayElement* mElement;
    bool mEnabled;
    bool mDirty;
    Real mTextWidth;
    bool mOnScreen;
    void computeTextWidth();
    void updateOverlay();
    void getMinMaxEdgesOfTopAABBIn2D( Real& minX, Real& minY, Real& maxX, Real& maxY );
    void getScreenCoordinates( const Vector3& position, Real& x, Real& y );
  public:
    MovableTextOverlay( const Ogre::String& name,
      const Ogre::String& caption,
      const Ogre::MovableObject* target,
      const Ogre::Camera* camera );
    virtual ~MovableTextOverlay();
    void setCaption( const Ogre::String& caption );
    const Ogre::String& getName() const { return mName; }
    const Ogre::String& getCaption() const { return mCaption; }
    const bool isOnScreen() const { return mOnScreen; }
    const bool isEnabled() const { return mEnabled; }
    void enable( bool enable );
    void update();
  };

}