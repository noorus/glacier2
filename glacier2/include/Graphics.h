#pragma once
#include "Console.h"
#include "EngineComponent.h"
#include "GlobalStats.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_EXTERN_CONVAR( vid_screenwidth );
  ENGINE_EXTERN_CONVAR( vid_screenheight );
  ENGINE_EXTERN_CONVAR( vid_screenbpp );
  ENGINE_EXTERN_CONVAR( vid_fullscreen );
  ENGINE_EXTERN_CONVAR( vid_vsync );
  ENGINE_EXTERN_CONVAR( vid_fsaa );
  ENGINE_EXTERN_CONCMD( vid_restart );

  class Graphics: public EngineComponent {
  protected:
    struct Globals {
      GlobalStats stats;
    } mGlobals;
    Ogre::Root* mRoot;
    Ogre::RenderSystem* mRenderer;
    Ogre::RenderWindow* mWindow;
    Ogre::PCZSceneManager* mSceneManager;
    Ogre::OverlaySystem* mOverlaySystem;
    void registerResources();
    void unregisterResources();
  public:
    struct VideoMode {
      uint32_t mWidth;
      uint32_t mHeight;
      uint32_t mBits;
      bool mFullscreen;
      bool mVSync;
      uint32_t mFSAA;
      VideoMode( uint32_t width, uint32_t height, uint32_t bpp,
        bool fs, bool vsync, uint32_t fsaa );
      void setConfigurations( Ogre::RenderSystem& renderer );
      Ogre::NameValuePairList getParams();
      string getAsOptionString();
      string optBitsAsString();
      string optFSAsString();
      string optVSyncAsString();
      string optFSAAAsString();
    };
    static void callbackVideoRestart( Console* console,
      ConCmd* command, StringVector& arguments );
  public:
    Graphics( Engine* engine );
    void videoInitialize();
    void videoShutdown();
    void videoRestart();
    void screenshot();
    Ogre::Root* getRoot() { return mRoot; }
    Ogre::RenderSystem* getRenderer() { return mRenderer; }
    Ogre::RenderWindow* getWindow() { return mWindow; }
    Ogre::PCZSceneManager* getScene() { return mSceneManager; }
    virtual void componentPreUpdate( GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    virtual ~Graphics();
  };

}