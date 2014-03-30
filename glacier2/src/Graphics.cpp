#include "StdAfx.h"
#include "Graphics.h"
#include "Console.h"
#include "Exception.h"
#include "Engine.h"

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
  ENGINE_DECLARE_CONCMD( vid_restart,
    L"Restart the renderer to apply changes to display mode.",
    &Graphics::callbackVideoRestart );

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
  uint32_t bpp, bool fs, bool vsync, uint32_t fsaa ):
  mWidth( width ), mHeight( height ), mBits( bpp ), mFullscreen( fs ),
  mVSync( vsync ), mFSAA( fsaa )
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

  // Graphics class ===========================================================

  Graphics::Graphics( Engine* engine ): EngineComponent( engine ),
  mRoot( nullptr ), mRenderer( nullptr ), mSceneManager( nullptr ),
  mOverlaySystem( nullptr )
  {
    videoInitialize();
  }

  void Graphics::videoInitialize()
  {
    mEngine->getConsole()->printf( Console::srcGfx,
      L"Initializing graphics engine..." );

    // Print some version stuff
    mEngine->getConsole()->printf( Console::srcGfx,
      L"Using Ogre3D %i.%i.%i %S (%S)",
      OGRE_VERSION_MAJOR, OGRE_VERSION_MINOR, OGRE_VERSION_PATCH,
      OGRE_VERSION_SUFFIX, OGRE_VERSION_NAME );

    // Create Ogre root
    mRoot = new Ogre::Root( "", "", cOgreLogFile );

    // Create overlay system
    mOverlaySystem = new Ogre::OverlaySystem();

    // Load renderer
#if defined( _DEBUG )
    mRoot->loadPlugin( cRenderSystemDebug );
#else
    mRoot->loadPlugin( cRenderSystemRelease );
#endif

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
      g_CVar_vid_fsaa.getInt() );

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

    // Stop the window from being deactivated when it loses focus
    mWindow->setDeactivateOnFocusChange( false );

    // Load plugins
#if defined( _DEBUG )
    mRoot->loadPlugin( "Plugin_CgProgramManager_d" );
    mRoot->loadPlugin( "Plugin_PCZSceneManager_d" );
    // mRoot->loadPlugin( "Plugin_OctreeZone_d" );
#else
    mRoot->loadPlugin( "Plugin_CgProgramManager" );
    mRoot->loadPlugin( "Plugin_PCZSceneManager" );
    // mRoot->loadPlugin( "Plugin_OctreeZone" );
#endif

    // Create the portal-connected scene manager
    mSceneManager = (Ogre::PCZSceneManager*)mRoot->createSceneManager(
      "PCZSceneManager", "PCZSceneManager" );
    mSceneManager->init( "ZoneType_Default" );

    // Add OverlaySystem as a global renderqueue listener
    mSceneManager->addRenderQueueListener( mOverlaySystem );

    // Register & initialize resource groups
    registerResources();

    // Initialize globals
    mGlobals.stats.init();

    // Tell the engine to continue, in case this was a restart
    mEngine->operationContinueVideo();
  }

  void Graphics::videoRestart()
  {
    videoShutdown();
    videoInitialize();
  }

  void Graphics::videoShutdown()
  {
    // Suspend active state's video operations
    mEngine->operationSuspendVideo();

    // Shutdown globals
    mGlobals.stats.shutdown();

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

      unregisterResources();
      SAFE_DELETE( mOverlaySystem );
      mRoot->shutdown();
      SAFE_DELETE( mRoot );
      mRenderer = nullptr;
    }
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

  void Graphics::registerResources()
  {
    mEngine->getConsole()->printf( Console::srcGfx, L"Registering resources" );

    ResourceGroupManager::getSingleton().addResourceLocation(
      "data\\bootload", "FileSystem", "Bootload", true );
    ResourceGroupManager::getSingleton().initialiseResourceGroup(
      "Bootload" );
  }

  void Graphics::unregisterResources()
  {
    mEngine->getConsole()->printf( Console::srcGfx, L"Unregistering resources" );

    ResourceGroupManager::getSingleton().removeResourceLocation(
      "data\\bootload", "Bootload" );
  }

  void Graphics::componentPreUpdate( GameTime time )
  {
    Ogre::WindowEventUtilities::messagePump();
  }

  void Graphics::componentPostUpdate( GameTime delta, GameTime time )
  {
    // Update globals
    mGlobals.stats.update();

    // Render frame
    if ( !mRoot->renderOneFrame( (Ogre::Real)delta ) )
      ENGINE_EXCEPT( L"Frame failed to render" );
  }

  void Graphics::callbackVideoRestart( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    if ( !gEngine || !gEngine->getGraphics() )
      return;

    gEngine->getGraphics()->videoRestart();
  }

  Graphics::~Graphics()
  {
    videoShutdown();
  }

}