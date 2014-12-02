#include "StdAfx.h"
#include "Graphics.h"
#include "Console.h"
#include "Exception.h"
#include "Engine.h"
#include "MovableTextOverlay.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  MovableTextOverlayAttributes::MovableTextOverlayAttributes(
    const Ogre::Camera* camera, const Ogre::String& font, const Real height,
    const ColourValue& color, const Ogre::String& material ):
    mCamera( camera ), mFont( nullptr ), mHeight( height ), mColor( color )
  {
    setFontName( font );
    setMaterialName( material );
    setColor( color );
  }

  void MovableTextOverlayAttributes::setFontName( const Ogre::String& name )
  {
    if ( mFontName == name && mFont )
      return;

    if ( mFont )
    {
      mFont->unload();
      mFont = nullptr;
    }

    mFontName = name;
    if ( !mFontName.empty() )
    {
      mFont = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName( mFontName ).getPointer();
      if ( !mFont )
        Ogre::Exception( Ogre::Exception::ERR_ITEM_NOT_FOUND, "Could not find font " + name, __FUNCTION__ );
      mFont->load();
    }
  }

  void MovableTextOverlayAttributes::setMaterialName( const Ogre::String& name )
  {
    if ( mMaterialName == name )
      return;

    if ( mMaterialName.length() )
      Ogre::MaterialManager::getSingleton().getByName( mMaterialName ).getPointer()->unload();

    mMaterialName = name;
    if ( !mMaterialName.empty() )
    {
      Ogre::Material* material = (Ogre::Material*)Ogre::MaterialManager::getSingleton().getByName( mMaterialName ).getPointer();
      if ( !material )
        Ogre::Exception( Ogre::Exception::ERR_ITEM_NOT_FOUND, "Could not find material " + name, __FUNCTION__ );
      material->load();
    }
  }

  void MovableTextOverlayAttributes::setColor( const ColourValue& color )
  {
    mColor = color;
  }

  void MovableTextOverlayAttributes::setHeight( const Real height )
  {
    mHeight = height;
  }

  MovableTextOverlayAttributes::~MovableTextOverlayAttributes()
  {
    setFontName( "" );
    setMaterialName( "" );
  }

  MovableTextOverlay::MovableTextOverlay( const Ogre::String& name,
  const Ogre::String& caption, const Ogre::MovableObject* target,
  MovableTextOverlayAttributes* attributes ):
  mName( name ), mTarget( target ), mDirty( true ), mOnScreen( false ),
  mEnabled( false ), mAttributes( attributes )
  {
    Ogre::OverlayManager& manager = Ogre::OverlayManager::getSingleton();

    mOverlay = manager.create( name );
    mOverlay->hide();

    mContainer = (Ogre::OverlayContainer*)manager.createOverlayElement( "Panel", name + "/Container" );
    mContainer->setDimensions( 0.0f, 0.0f );

    mOverlay->add2D( mContainer );

    mElement = manager.createOverlayElement( "TextArea", name + "/Container/Text" );
    mContainer->addChild( mElement );

    mElement->setMetricsMode( Ogre::GMM_RELATIVE );
    mElement->setDimensions( 1.0f, 1.0f );
    mElement->setMetricsMode( Ogre::GMM_PIXELS );
    mElement->setPosition( 0.0f, 0.0f );

    updateOverlay();
    setCaption( caption );
  }

  void MovableTextOverlay::setCaption( const Ogre::String& caption )
  {
    mCaption = caption;
    mElement->setCaption( mCaption );
    mDirty = true;
  }

  void MovableTextOverlay::enable( bool enable )
  {
    if ( mEnabled != enable )
    {
      mEnabled = enable;
      if ( mEnabled )
        mOverlay->show();
      else
        mOverlay->hide();
    }
  }

  void MovableTextOverlay::computeTextWidth()
  {
    Real spaceWidth = mAttributes->getFont()->getGlyphAspectRatio( 0x0030 );
    mTextWidth = spaceWidth;

    for ( auto character : mCaption )
      mTextWidth += ( character == 0x0020 ) ? spaceWidth : mAttributes->getFont()->getGlyphAspectRatio( character );

    mTextWidth *= mAttributes->getHeight();
    mTextWidth *= 1.1f;
  }

  void MovableTextOverlay::getMinMaxEdgesOfTopAABBIn2D( Real& minX, Real& minY, Real& maxX, Real& maxY )
  {
    minX = maxX = minY = maxY = 0.0f;

    Real x[4];
    Real y[4];

    if ( !mTarget->isInScene() )
      return;

    const Ogre::AxisAlignedBox& aabb = mTarget->getWorldBoundingBox( true );
    const Vector3 corners[4] = {
      aabb.getCorner( AxisAlignedBox::FAR_LEFT_TOP ),
      aabb.getCorner( AxisAlignedBox::FAR_RIGHT_TOP ),
      aabb.getCorner( AxisAlignedBox::NEAR_LEFT_TOP ),
      aabb.getCorner( AxisAlignedBox::NEAR_RIGHT_TOP )
    };

    Vector3 cameraPlaneNormal = mAttributes->getCamera()->getDerivedOrientation().zAxis();
    auto cameraPlane = Ogre::Plane( cameraPlaneNormal, mAttributes->getCamera()->getDerivedPosition() );

    for ( size_t i = 0; i < 4; i++ )
    {
      x[i] = 0.0f;
      y[i] = 0.0f;

      getScreenCoordinates( corners[i], x[i], y[i] );

      if ( cameraPlane.getSide( corners[i] ) == Ogre::Plane::NEGATIVE_SIDE )
      {
        if ( !i )
        {
          minX = x[i];
          minY = y[i];
          maxX = x[i];
          maxY = y[i];
        }
        else
        {
          if ( minX > x[i] )
            minX = x[i];
          if ( minY > y[i] )
            minY = y[i];
          if ( maxX < x[i] )
            maxX = x[i];
          if ( maxY < y[i] )
            maxY = y[i];
        }
      }
      else
      {
        minX = minY = maxX = maxY = 0.0f;
        break;
      }
    }
  }

  void MovableTextOverlay::getScreenCoordinates( const Vector3& position,
  Real& x, Real& y )
  {
    Vector3 projected = mAttributes->getCamera()->getProjectionMatrix() * ( mAttributes->getCamera()->getViewMatrix() * position );

    x = 1.0f - ( ( projected.x * 0.5f ) + 0.5f );
    y = ( ( projected.y * 0.5f ) + 0.5f );
  }

  void MovableTextOverlay::updateOverlay()
  {
    mContainer->setMaterialName( mAttributes->getMaterialName() );
    mElement->setColour( mAttributes->getColor() );
    mElement->setParameter( "font_name", mAttributes->getFontName() );
    mElement->setParameter( "char_height", Ogre::StringConverter::toString( mAttributes->getHeight() ) );
    mElement->setParameter( "horz_align", "center" );
    mElement->setParameter( "vert_align", "top" );
    mElement->setParameter( "alignment", "center" );
  }

  void MovableTextOverlay::update()
  {
    Ogre::OverlayManager& manager = Ogre::OverlayManager::getSingleton();

    Real minX, minY, maxX, maxY;
    getMinMaxEdgesOfTopAABBIn2D( minX, minY, maxX, maxY );

    mOnScreen = ( minX > 0.0f && maxX < 1.0f && minY > 0.0f && maxY < 1.0f );

    if ( mDirty )
    {
      computeTextWidth();
      mDirty = false;
    }

    Real textWidth = mTextWidth / manager.getViewportWidth();
    Real textHeight = ( mAttributes->getHeight() * 1.1f ) / manager.getViewportHeight();

    mContainer->setPosition(
      1.0f - ( minX + maxX + textWidth ) / 2.0f,
      1.0f - ( maxY + textHeight * 0.5f ) );
    mContainer->setDimensions( textWidth, textHeight );
  }

  MovableTextOverlay::~MovableTextOverlay()
  {
    Ogre::OverlayManager& manager = Ogre::OverlayManager::getSingleton();

    mOverlay->hide();
    mContainer->removeChild( mElement->getName() );
    mOverlay->remove2D( mContainer );

    manager.destroyOverlayElement( mElement );
    manager.destroyOverlayElement( mContainer );
    manager.destroy( mOverlay );
  }

}