#pragma once
#include "Console.h"
#include "EngineComponent.h"
#include "GlobalStats.h"
#include "GlacierCustomPass.h"
#include "Mouse.h"
#include "Map.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Gorilla {
  class Silverback;
  class Screen;
  class Layer;
  class GorillaPassProvider;
  class Rectangle;
}

namespace Glacier {

  ENGINE_EXTERN_CONVAR( vid_screenwidth );
  ENGINE_EXTERN_CONVAR( vid_screenheight );
  ENGINE_EXTERN_CONVAR( vid_screenbpp );
  ENGINE_EXTERN_CONVAR( vid_fullscreen );
  ENGINE_EXTERN_CONVAR( vid_vsync );
  ENGINE_EXTERN_CONVAR( vid_fsaa );
  ENGINE_EXTERN_CONCMD( vid_restart );

  class WindowHandler;
  class Camera;
  class HUD;

  class HUD: public EngineComponent {
  protected:
    Gorilla::Screen* gorillaScreen_;
    Gorilla::Layer* gorillaLayer_;
    struct {
      Map::Rect rect_;
      Map::Point from_;
      Map::Point to_;
      Gorilla::Rectangle* vis_;
    } selection_;
  public:
    HUD( Engine* engine, Gorilla::Screen* screen );
    void beginSelection( const Mouse::MousePacket& from );
    void updateSelection( const Mouse::MousePacket& to );
    void endSelection( const Mouse::MousePacket& to );
    virtual void componentPreUpdate( GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    virtual ~HUD();
  };

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
    SceneManager* mSceneManager;
    CompositorWorkspace* mGameWorkspace;
    Ogre::v1::OverlaySystem* mOverlaySystem;
    WindowHandler* mWindowHandler;
    Settings mSettings;
    HlmsUnlit* mUnlitMaterials;
    HlmsPbs* mPbsMaterials;
    Gorilla::Silverback* mSilverback;
  public:
    Gorilla::Screen* mGorillaScreen;
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
    static bool callbackCVARTextureFiltering(
      ConVar* variable, ConVar::Value oldValue );
    static bool callbackCVARTextureAnisotropy(
      ConVar* variable, ConVar::Value oldValue );
    static bool callbackCVARTextureMipmaps(
      ConVar* variable, ConVar::Value oldValue );
  public:
    Graphics( Engine* engine, WindowHandler* windowHandler );
    void setupCompositors();
    CompositorWorkspace* createGameWorkspace( Camera* camera );
    void destroyGameWorkspace( CompositorWorkspace* ws );
    void registerHlms();
    void unregisterHlms();
    void preInitialize();
    void postInitialize();
    void videoShutdown();
    void videoRestart();
    void screenshot();
    void applySettings( const Settings& settings );
    const Settings& getSettings() throw() { return mSettings; }
    void setRenderWindowTitle( const string& title );
    HWND getRenderWindowHandle();
    Ogre::Root* getRoot() { return mRoot; }
    Ogre::RenderSystem* getRenderer() { return mRenderer; }
    Ogre::RenderWindow* getWindow() { return mWindow; }
    Ogre::SceneManager* getScene() { return mSceneManager; }
    HlmsUnlit* getUnlit() { return mUnlitMaterials; }
    HlmsPbs* getPbs() { return mPbsMaterials; }
    virtual void componentPreUpdate( GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    static void registerResources( ResourceGroupManager& manager );
    static void unregisterResources( ResourceGroupManager& manager );
    virtual ~Graphics();
  };

}