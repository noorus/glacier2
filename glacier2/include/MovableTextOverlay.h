#pragma once
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class MovableTextOverlayAttributes {
  protected:
    const Ogre::Camera* mCamera;
    Ogre::Font* mFont;
    Ogre::String mFontName;
    Ogre::String mMaterialName;
    ColourValue mColor;
    Real mHeight;
  public:
    MovableTextOverlayAttributes( const Ogre::Camera* camera,
      const Ogre::String& font, const Real height, const ColourValue& color,
      const Ogre::String& material );
    ~MovableTextOverlayAttributes();
    const Ogre::Camera* getCamera() const { return mCamera; }
    const Ogre::Font* getFont() const { return mFont; }
    const Ogre::String& getFontName() const { return mFontName; }
    const Ogre::String& getMaterialName() const { return mMaterialName; }
    const ColourValue& getColor() const { return mColor; }
    const Real getHeight() const { return mHeight; }
    void setFontName( const Ogre::String& name );
    void setMaterialName( const Ogre::String& name );
    void setColor( const ColourValue& color );
    void setHeight( const Real height );
  };

  class MovableTextOverlay {
  protected:
    const Ogre::String mName;
    const Ogre::MovableObject* mTarget;
    Ogre::String mCaption;
    Ogre::Overlay* mOverlay;
    Ogre::OverlayContainer* mContainer;
    Ogre::OverlayElement* mElement;
    MovableTextOverlayAttributes* mAttributes;
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
      MovableTextOverlayAttributes* attributes );
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