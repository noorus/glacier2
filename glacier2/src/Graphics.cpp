#include "StdAfx.h"
#include "Graphics.h"
#include "Console.h"
#include "Exception.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "OgreD3D9Plugin.h"
#include "Plugins/CgProgramManager/OgreCgPlugin.h"
#include "Plugins/PCZSceneManager/OgrePCZPlugin.h"
#include "Win32.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  // Graphics engine CVARs ====================================================

  ENGINE_DECLARE_CONVAR( vid_screenwidth,
    L"Screen width. Changes are applied when the renderer is restarted.", 1280 );
  ENGINE_DECLARE_CONVAR( vid_screenheight,
    L"Screen height. Changes are applied when the renderer is restarted.", 720 );
  ENGINE_DECLARE_CONVAR( vid_screenbpp,
    L"Screen bit depth. Changes are applied when the renderer is restarted.", 32 );
  ENGINE_DECLARE_CONVAR( vid_fullscreen,
    L"Fullscreen toggle. Changes are applied when the renderer is restarted.", false );
  ENGINE_DECLARE_CONVAR( vid_vsync,
    L"Vertical sync toggle. Changes are applied when the renderer is restarted.", false );
  ENGINE_DECLARE_CONVAR( vid_fsaa,
    L"Full Scene Anti-Aliasing multiplier. Valid values are multiples of 2.", 8 );
  ENGINE_DECLARE_CONVAR( vid_d3d9ex,
    L"Whether to try using Direct3D9Ex when available. Windows 7+.", true );
  ENGINE_DECLARE_CONCMD( vid_restart,
    L"Restart the renderer to apply changes to display mode.",
    &Graphics::callbackVideoRestart );

  ENGINE_DECLARE_CONVAR_WITH_CB( tex_filtering,
    L"Texture filtering method. 0 = none, 1 = bilinear, 2 = trilinear, 3 = anisotropic.",
    3, Graphics::callbackCVARTextureFiltering );
  ENGINE_DECLARE_CONVAR_WITH_CB( tex_anisotropy,
    L"Level of anisotropy when using anisotropic texture filtering.",
    8, Graphics::callbackCVARTextureAnisotropy );
  ENGINE_DECLARE_CONVAR_WITH_CB( tex_mipmaps,
    L"Default number of mipmaps used when loading textures.",
    3, Graphics::callbackCVARTextureMipmaps );

  // Graphics engine constants ================================================

  const char* cRenderSystemName    = "Direct3D9 Rendering Subsystem";
  const char* cRenderSystemDebug   = "RenderSystem_Direct3D9_d";
  const char* cRenderSystemRelease = "RenderSystem_Direct3D9";
  const char* cRenderWindowClass   = "gcr2_render";
  const char* cRenderWindowTitle   = "glacier² » renderer";
  const char* cScreenshotPrefix    = "screenshot";
  const char* cScreenshotSuffix    = ".png";
  const char* cOgreLogFile         = "ogre.log";

  // Graphics::VideoMode class ================================================

  Graphics::VideoMode::VideoMode( uint32_t width, uint32_t height,
  uint32_t bpp, bool fs, bool vsync, uint32_t fsaa, bool dx9ex ):
  mWidth( width ), mHeight( height ), mBits( bpp ), mFullscreen( fs ),
  mVSync( vsync ), mFSAA( fsaa ), mDX9Ex( dx9ex )
  {
    //
  }

  void Graphics::VideoMode::setConfigurations( Ogre::RenderSystem& renderer )
  {
    renderer.setConfigOption( "Allow NVPerfHUD", "No" );
    renderer.setConfigOption( "Floating-point mode", "Fastest" );
    renderer.setConfigOption( "VSync", optVSyncAsString() );
    renderer.setConfigOption( "Full Screen", optFSAsString() );
    renderer.setConfigOption( "Video Mode", getAsOptionString() );
    renderer.setConfigOption( "sRGB Gamma Conversion", "No" );
    renderer.setConfigOption( "Allow DirectX9Ex", optDX9ExAsString() );
  }

  Ogre::NameValuePairList Graphics::VideoMode::getParams()
  {
    Ogre::NameValuePairList params;
    params["title"] = cRenderWindowTitle;
    params["colourDepth"] = optBitsAsString();
    params["vsync"] = optVSyncAsString();
    params["FSAA"] = optFSAAAsString();
    params["border"] = "fixed";
    params["outerDimensions"] = "false";
    return params;
  }

  string Graphics::VideoMode::getAsOptionString()
  {
    char options[64];
    sprintf_s( options, 64, "%u x %u @ %u-bit colour",
      mWidth, mHeight, mBits );
    return options;
  }

  string Graphics::VideoMode::optBitsAsString()
  {
    char bpp[3] = { '\0' };
    _ultoa_s( mBits, bpp, 3, 10 );
    return bpp;
  }

  string Graphics::VideoMode::optFSAsString()
  {
    return mFullscreen ? "Yes" : "No";
  }

  string Graphics::VideoMode::optVSyncAsString()
  {
    return mVSync ? "true" : "false";
  }

  string Graphics::VideoMode::optFSAAAsString()
  {
    char fsaa[3] = { '\0' };
    _ultoa_s( mFSAA, fsaa, 3, 10 );
    return fsaa;
  }

  string Graphics::VideoMode::optDX9ExAsString()
  {
    return mDX9Ex ? "Yes" : "No";
  }

  // Graphics class ===========================================================

  Graphics::Graphics( Engine* engine, WindowHandler* windowHandler ):
  EngineComponent( engine ),
  mRoot( nullptr ), mRenderer( nullptr ), mSceneManager( nullptr ),
  mOverlaySystem( nullptr ), mWindowHandler( windowHandler ),
  mShinyPlatform( nullptr ), mShinyFactory( nullptr )
  {
    preInitialize();
  }

  void Graphics::preInitialize()
  {
    // Print some version stuff
    mEngine->getConsole()->printf( Console::srcGfx,
      L"Initializing Ogre3D v%i.%i.%i (%S)",
      OGRE_VERSION_MAJOR, OGRE_VERSION_MINOR, OGRE_VERSION_PATCH,
      OGRE_VERSION_NAME );

    // Create Ogre root
    mRoot = new Ogre::Root( "", "", cOgreLogFile );

    mEngine->registerUserLocations( ResourceGroupManager::getSingleton() );
  }

  void Graphics::postInitialize()
  {
    // Create overlay system
    mOverlaySystem = new Ogre::OverlaySystem();

    // Load renderer
    mRoot->installPlugin( new Ogre::D3D9Plugin );

    // Select renderer
    mRenderer = mRoot->getRenderSystemByName( cRenderSystemName );
    mRoot->setRenderSystem( mRenderer );

    // Setup video mode
    VideoMode videoMode(
      g_CVar_vid_screenwidth.getInt(), 
      g_CVar_vid_screenheight.getInt(), 
      g_CVar_vid_screenbpp.getInt(), 
      g_CVar_vid_fullscreen.getBool(),
      g_CVar_vid_vsync.getBool(),
      g_CVar_vid_fsaa.getInt(),
      g_CVar_vid_d3d9ex.getBool() );

    mEngine->getConsole()->printf( Console::srcGfx,
      L"Video mode: %S%s%s (fsaa x%u)",
      videoMode.getAsOptionString().c_str(),
      videoMode.mFullscreen ? L" (fullscreen)" : L"",
      videoMode.mVSync ? L" (vsync)" : L"",
      videoMode.mFSAA );

    videoMode.setConfigurations( *mRenderer );
    mRoot->initialise( false );

    // Setup the render window
    mEngine->getConsole()->printf( Console::srcGfx,
      L"Creating render window..." );

    mWindow = mRoot->createRenderWindow( cRenderWindowClass,
      videoMode.mWidth, videoMode.mHeight, videoMode.mFullscreen,
      &videoMode.getParams() );

    // Add engine-supplied event listener to the render window
    Ogre::WindowEventUtilities::addWindowEventListener(
      mWindow, mWindowHandler );

    // Stop the window from being deactivated when it loses focus
    mWindow->setDeactivateOnFocusChange( false );

    // Load plugins
    mRoot->installPlugin( new Ogre::CgPlugin );
    mRoot->installPlugin( new Ogre::PCZPlugin );

    // Create the portal-connected scene manager
    mSceneManager = (Ogre::PCZSceneManager*)mRoot->createSceneManager(
      "PCZSceneManager", "PCZSceneManager" );
    mSceneManager->init( "ZoneType_Default" );

    // Add OverlaySystem as a global renderqueue listener
    mSceneManager->addRenderQueueListener( mOverlaySystem );

    // Set default texture filtering, anisotropy and mipmap count
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(
      (Ogre::TextureFilterOptions)g_CVar_tex_filtering.getInt() );
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(
      g_CVar_tex_anisotropy.getInt() );
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(
      g_CVar_tex_mipmaps.getInt() );

    // Register & initialize resource groups
    mEngine->registerResources( ResourceGroupManager::getSingleton() );

    // Initialize shiny
    // mShinyPlatform = new sh::OgrePlatform( "General", "data/shiny" );

    // mShinyFactory = new sh::Factory( mShinyPlatform );
    // mShinyFactory->setCurrentLanguage( sh::Language_CG );
    // mShinyFactory->loadAllFiles();

    // Initialize globals
    mGlobals.stats.init();

    // Tell the engine to continue, in case this was a restart
    mEngine->operationContinueVideo();
  }

  void Graphics::videoRestart()
  {
    videoShutdown();
    preInitialize();
    postInitialize();
  }

  void Graphics::videoShutdown()
  {
    // Suspend active state's video operations
    mEngine->operationSuspendVideo();

    // Shutdown globals
    mGlobals.stats.shutdown();

    SAFE_DELETE( mShinyFactory );

    // If we have a window, remove the associated event listener
    if ( mWindow )
    {
      Ogre::WindowEventUtilities::removeWindowEventListener(
        mWindow, mWindowHandler );
    }

    // Destroy Ogre
    if ( mRoot )
    {
      mEngine->getConsole()->printf( Console::srcGfx,
        L"Shutting down renderer..." );

      if ( mSceneManager )
      {
        if ( mOverlaySystem )
          mSceneManager->removeRenderQueueListener( mOverlaySystem );
        mRoot->destroySceneManager( mSceneManager );
      }

      mEngine->unregisterResources( ResourceGroupManager::getSingleton() );
      mEngine->unregisterUserLocations( ResourceGroupManager::getSingleton() );

      SAFE_DELETE( mOverlaySystem );
      mRoot->shutdown();
      SAFE_DELETE( mRoot );
      mRenderer = nullptr;
    }
  }

  void Graphics::setRenderWindowTitle( const wstring& title )
  {
    SetWindowTextW( getRenderWindowHandle(), title.c_str() );
  }

  HWND Graphics::getRenderWindowHandle()
  {
    HWND windowHandle = NULL;
    mWindow->getCustomAttribute( "WINDOW", &windowHandle );
    return windowHandle;
  }

  void Graphics::screenshot()
  {
    if ( !mWindow )
      return;

    Ogre::String filename = mWindow->writeContentsToTimestampedFile(
      cScreenshotPrefix, cScreenshotSuffix );

    if ( gEngine && gEngine->getConsole() )
      gEngine->getConsole()->printf( Console::srcGfx,
      L"Saved screenshot to %S", filename.c_str() );
  }


  void Graphics::registerResources( ResourceGroupManager& manager )
  {
    gEngine->getConsole()->printf( Console::srcGfx,
      L"Registering resources..." );

    manager.addResourceLocation( "data\\bootload", "FileSystem", "Bootload", true );
    manager.initialiseResourceGroup( "Bootload" );
  }

  void Graphics::unregisterResources( ResourceGroupManager& manager )
  {
    gEngine->getConsole()->printf( Console::srcGfx,
      L"Unregistering resources..." );

    manager.removeResourceLocation( "data\\bootload", "Bootload" );
  }

  void Graphics::componentPreUpdate( GameTime time )
  {
    HWND windowHandle = NULL;
    mWindow->getCustomAttribute( "WINDOW", &windowHandle );
    Win32::Win32::instance().handleMessagesFor( windowHandle );
  }

  void Graphics::componentPostUpdate( GameTime delta, GameTime time )
  {
    // Update globals
    mGlobals.stats.update();

    // Render frame
    if ( !mRoot->renderOneFrame( (Ogre::Real)delta ) )
      ENGINE_EXCEPT( L"Failed to render frame" );
  }

  void Graphics::callbackVideoRestart( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    if ( !gEngine || !gEngine->getGraphics() )
      return;

    gEngine->getGraphics()->videoRestart();
  }

  bool Graphics::callbackCVARTextureFiltering(
  ConVar* variable, ConVar::Value oldValue )
  {
    auto method = (Ogre::TextureFilterOptions)variable->getInt();

    if ( method < Ogre::TFO_NONE )
      method = Ogre::TFO_NONE;
    if ( method > Ogre::TFO_ANISOTROPIC )
      method = Ogre::TFO_ANISOTROPIC;

    if ( Ogre::MaterialManager::getSingletonPtr() )
      Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(
        method );

    return true;
  }

  bool Graphics::callbackCVARTextureAnisotropy(
  ConVar* variable, ConVar::Value oldValue )
  {
    if ( Ogre::MaterialManager::getSingletonPtr() )
      Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(
        variable->getInt() );

    return true;
  }

  bool Graphics::callbackCVARTextureMipmaps(
  ConVar* variable, ConVar::Value oldValue )
  {
    int value = variable->getInt();

    if ( value >= 16 )
      value = Ogre::MIP_UNLIMITED;
    else if ( value < 1 )
      value = Ogre::MIP_DEFAULT;

    if ( Ogre::TextureManager::getSingletonPtr() )
      Ogre::TextureManager::getSingleton().setDefaultNumMipmaps( value );

    variable->forceValue( value );

    return true;
  }

  Graphics::~Graphics()
  {
    videoShutdown();
  }

}