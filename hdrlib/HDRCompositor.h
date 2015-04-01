#ifndef HDRLIB_HDRCOMPOSITOR_H
#define HDRLIB_HDRCOMPOSITOR_H

//
//  This source file is a part of HDRlib,
//  an addon for OGRE, the Object-oriented Graphics Rendering Engine.
//  Original author: Christian Luksch
//  Forked and modified in 2014 by noorus/sldt@github
//  Licensed under the GNU Lesser General Public License v3
//  See: https://github.com/sldt/hdrlib
//

#include <OgreResource.h>
#include <OgreCompositor.h>
#include <OgreCompositorManager.h>
#include <OgreCompositorChain.h>
#include <OgreCompositorInstance.h>
#include <OgreCompositionTechnique.h>
#include <OgreCompositionPass.h>
#include <OgreCompositionTargetPass.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>

namespace HDRlib {

  enum Tonemapper {
    Tonemapper_None = 0,
    Tonemapper_Linear,
    Tonemapper_Reinhard,
    Tonemapper_ReinhardMod,
    Tonemapper_Log,
    Tonemapper_AdaptLog,
    Tonemapper_ReinhardLocal,
    Tonemapper_Count
  };

  enum GlareType {
    Glare_None = 0,
    Glare_Blur
  };

  enum StarType {
    Star_None = 0,
    Star_Plus,
    Star_Cross,
    Star_PlusCross
  };

  class HDRCompositor: public Ogre::CompositorInstance::Listener {
  protected:
    enum MaterialID
    {
      MID_KEY = 12345,
      MID_LUMSCALE1,
      MID_LUMSCALE2,
      MID_LUMSCALE4,
      MID_LUMSCALE8,
      MID_BUILDLOCAL,
      MID_DOWN,
      MID_BRIGHT,
      MID_GAUSSBLUR,
      MID_STARH,
      MID_STARV,
      MID_FINAL,
      MID_ADAPT,
      MID_SCALE
    };
    Tonemapper mTonemapper;
    struct Glare {
      GlareType type;
      float strength;
      int passes;
      Glare();
    } mGlare;
    struct Star {
      StarType type;
      float strength;
      int passes;
      Star();
    } mStar;
    bool mAutokeying;
    float mKey;
    bool mLumAdaption;
    float mViewWidth, mViewHeight;
    bool mEnabled;
    float mFrameTime;
    float mAdaptationScale;
    float mBrightPassOffset;
    float mBrightPassThreshold;
    const int mScales;
    float mE;
    float mPhi;
    Ogre::RenderWindow* mWindow;
    Ogre::Camera* mCamera;
    Ogre::CompositorPtr mCompositor;
    Ogre::CompositionTechnique* mTechnique;
  public:
    HDRCompositor( Ogre::RenderWindow* window, Ogre::Camera* camera );
    ~HDRCompositor();
    void release();
    void enable( const bool enable_ );
    bool isEnabled() const { return mEnabled; }
    Tonemapper getTonemapper() const { return mTonemapper; }
    void setTonemapper( const Tonemapper tonemapper ) { mTonemapper = tonemapper; create(); }
    GlareType getGlareType() const { return mGlare.type; }
    void setGlareType( const GlareType glare ) { mGlare.type = glare; create(); }
    float getGlareStrength() const { return mGlare.strength; }
    void setGlareStrength( const float strength ) { mGlare.strength = strength; }
    int getGlarePasses() const { return mGlare.passes; }
    void setGlarePasses( const int passes ) { mGlare.passes = passes; create(); }
    StarType getStarType() const { return mStar.type; }
    void setStarType( const StarType star ) { mStar.type = star; create(); }
    float getStarStrength() const { return mStar.strength; }
    void setStarStrength( const float strength ) { mStar.strength = strength; }
    int getStarPasses() const { return mStar.passes; }
    void setStarPasses( const int passes ) { mStar.passes = passes; create(); }
    bool getAutokeying() const { return mAutokeying; }
    void setAutokeying( const bool autokeying ) { mAutokeying = autokeying; create(); }
    bool getLumAdaption() const { return mLumAdaption; }
    void setLumAdaption( const bool adaption ) { mLumAdaption = adaption; create(); }
    float getKey() const { return mKey; }
    void setKey( const float key ) { mKey = key; }
    float getAdaptionScale() const { return mAdaptationScale; }
    void setAdaptionScale( const float scale ) { mAdaptationScale = scale; }
    float getLocalE() const { return mE; }
    void setLocalE( const float e ) { mE = e; }
    float getLocalPhi() const { return mPhi; }
    void setLocalPhi( const float phi ) { mPhi = phi; }
    void setFrameTime( const float frameTime ) { mFrameTime = frameTime; }
    virtual void notifyMaterialSetup( Ogre::uint32 passID,
      Ogre::MaterialPtr &material );
    virtual void notifyMaterialRender( Ogre::uint32 passID,
      Ogre::MaterialPtr &material );
    virtual void notifyViewportSize( int width, int height );
  private:
    void create();
    void createTextureDefinition( const Ogre::String& name,
      const unsigned int width, const unsigned int height,
      const Ogre::PixelFormat format );
    void brightPass();
    void renderDownsample();
    void calculateLuminance();
    void buildGlare();
    void buildStar();
    void calculateKey();
    void buildScales();
    void finalRendering();
    void buildGaussGlare( float* out, float rho, float strength, float scale );
    void buildGaussStarH( float* out, float rho, float strength );
    void buildGaussStarV( float* out, float rho, float strength );
  };

}

#endif