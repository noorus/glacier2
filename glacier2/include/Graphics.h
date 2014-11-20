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
  ENGINE_EXTERN_CONVAR( vid_d3d9ex );
  ENGINE_EXTERN_CONCMD( vid_restart );

  class WindowHandler;

  class Graphics: public EngineComponent {
  public:
    // Graphics settings
    struct Settings {
      bool fullScreen;
      bool verticalSync;
    };
  protected:
    struct Globals {
      GlobalStats stats;
    } mGlobals;
    Ogre::Root* mRoot;
    Ogre::RenderSystem* mRenderer;
    Ogre::RenderWindow* mWindow;
    Ogre::PCZSceneManager* mSceneManager;
    Ogre::OverlaySystem* mOverlaySystem;
    WindowHandler* mWindowHandler;
    Settings mSettings;
  public:
    struct VideoMode {
      uint32_t mWidth;
      uint32_t mHeight;
      uint32_t mBits;
      bool mFullscreen;
      bool mVSync;
      bool mDX9Ex;
      uint32_t mFSAA;
      VideoMode( uint32_t width, uint32_t height, uint32_t bpp,
        bool fs, bool vsync, uint32_t fsaa, bool dx9ex );
      void setConfigurations( Ogre::RenderSystem& renderer );
      Ogre::NameValuePairList getParams();
      string getAsOptionString();
      string optBitsAsString();
      string optFSAsString();
      string optVSyncAsString();
      string optFSAAAsString();
      string optDX9ExAsString();
    };
    static void callbackVideoRestart( Console* console,
      ConCmd* command, StringVector& arguments );
    static bool callbackCVARTextureFiltering(
      ConVar* variable, ConVar::Value oldValue );
    static bool callbackCVARTextureAnisotropy(
      ConVar* variable, ConVar::Value oldValue );
    static bool callbackCVARTextureMipmaps(
      ConVar* variable, ConVar::Value oldValue );
  public:
    Graphics( Engine* engine, WindowHandler* windowHandler );
    void preInitialize();
    void postInitialize();
    void videoShutdown();
    void videoRestart();
    void screenshot();
    void applySettings( const Settings& settings );
    const Settings& getSettings() { return mSettings; }
    void setRenderWindowTitle( const wstring& title );
    HWND getRenderWindowHandle();
    Ogre::Root* getRoot() { return mRoot; }
    Ogre::RenderSystem* getRenderer() { return mRenderer; }
    Ogre::RenderWindow* getWindow() { return mWindow; }
    Ogre::PCZSceneManager* getScene() { return mSceneManager; }
    virtual void componentPreUpdate( GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    static void registerResources( ResourceGroupManager& manager );
    static void unregisterResources( ResourceGroupManager& manager );
    virtual ~Graphics();
  };

}