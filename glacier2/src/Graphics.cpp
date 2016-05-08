#include "StdAfx.h"
#include "Graphics.h"
#include "Console.h"
#include "Exception.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "Win32.h"
#include "Camera.h"

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

  const char* cRenderSystemName    = "Direct3D11 Rendering Subsystem";
  const char* cRenderSystemDebug   = "RenderSystem_Direct3D11_d";
  const char* cRenderSystemRelease = "RenderSystem_Direct3D11";
  const char* cRenderWindowClass   = "gcr2_render";
  const char* cRenderWindowTitle   = "glacier² » renderer";
  const char* cScreenshotPrefix    = "screenshot";
  const char* cScreenshotSuffix    = ".png";
  const char* cOgreLogFile         = "ogre.log";
  const IdString cWorkspaceName    = "GcrMainWorkspace";

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
    renderer.setConfigOption( "sRGB Gamma Conversion", "Yes" );
  }

  Ogre::NameValuePairList Graphics::VideoMode::getParams()
  {
    Ogre::NameValuePairList params;
    params["title"] = cRenderWindowTitle;
    params["gamma"] = "true";
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

  Graphics::Graphics( Engine* engine, WindowHandler* windowHandler ):
  EngineComponent( engine ),
  mRoot( nullptr ), mRenderer( nullptr ), mSceneManager( nullptr ),
  mOverlaySystem( nullptr ), mWindowHandler( windowHandler ),
  mGameWorkspace( nullptr ), mUnlitMaterials( nullptr ), mPbsMaterials( nullptr )
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
    mOverlaySystem = new Ogre::v1::OverlaySystem();

#ifdef DEBUG
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

    // Refresh settings & CVARs
    mSettings.fullScreen = mWindow->isFullScreen();
    g_CVar_vid_fullscreen.forceValue( mSettings.fullScreen );
    mSettings.verticalSync = mWindow->isVSyncEnabled();
    g_CVar_vid_vsync.forceValue( mSettings.verticalSync );

    // Add engine-supplied event listener to the render window
    Ogre::WindowEventUtilities::addWindowEventListener(
      mWindow, mWindowHandler );

    // Stop the window from being deactivated when it loses focus
    mWindow->setDeactivateOnFocusChange( false );

    // Load plugins
    // mRoot->installPlugin( new Ogre::CgPlugin );
    // mRoot->installPlugin( new Ogre::PCZPlugin );

    // Create the portal-connected scene manager
    const size_t numThreads = 1;
    Ogre::InstancingThreadedCullingMethod cullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD;
    mSceneManager = mRoot->createSceneManager( Ogre::ST_GENERIC, numThreads, cullingMethod );

    // Add OverlaySystem as a global renderqueue listener
    mSceneManager->addRenderQueueListener( mOverlaySystem );

    setupCompositors();

    // Just some defaults
    mSceneManager->setShadowDirectionalLightExtrusionDistance( 500.0f );
    mSceneManager->setShadowFarDistance( 500.0f );

    // Set movables to have no scene query flags by default
    // This ensures that all kinds of random crap don't screw up our queries
    Ogre::MovableObject::setDefaultQueryFlags( 0 );

    // Set default texture filtering, anisotropy and mipmap count
    // Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(
    //   (Ogre::TextureFilterOptions)g_CVar_tex_filtering.getInt() );
    // Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(
    //   g_CVar_tex_anisotropy.getInt() );
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(
      g_CVar_tex_mipmaps.getInt() );

    registerHlms();

    // mShadows = new Shadows();
    // mPostProcessing = new PostProcessing( mSceneManager, mWindow );

    // Register & initialize resource groups
    mEngine->registerResources( ResourceGroupManager::getSingleton() );

    // Initialize globals
    mGlobals.stats.init();

    // Tell the engine to continue, in case this was a restart
    mEngine->operationContinueVideo();
  }

  void Graphics::setupCompositors()
  {
    auto cmpMgr = mRoot->getCompositorManager2();
    if ( !cmpMgr->hasWorkspaceDefinition( cWorkspaceName ) )
    {
      ColourValue bgColor( ColourValue::Black );
      cmpMgr->createBasicWorkspaceDef( cWorkspaceName, bgColor );
    }
  }

  CompositorWorkspace* Graphics::createGameWorkspace( Camera* camera )
  {
    assert( camera && !mGameWorkspace );

    auto cmpMgr = mRoot->getCompositorManager2();

    mGameWorkspace = cmpMgr->addWorkspace( mSceneManager, mWindow, camera->getCamera(), cWorkspaceName, true );
    return mGameWorkspace;
  }

  void Graphics::destroyGameWorkspace( CompositorWorkspace* ws )
  {
    assert( ws == mGameWorkspace );

    auto cmpMgr = mRoot->getCompositorManager2();

    cmpMgr->removeWorkspace( mGameWorkspace );
    mGameWorkspace = nullptr;
  }

  void Graphics::registerHlms()
  {
    mEngine->getConsole()->printf( Console::srcGfx,
      L"Registering HLMS..." );

    auto mgr = Ogre::ArchiveManager::getSingletonPtr();

    auto archiveCommon = mgr->load( "data\\tindalos\\Hlms\\Common\\HLSL", "FileSystem", true );

    Ogre::ArchiveVec library;
    library.push_back( archiveCommon );

    auto archiveUnlit = mgr->load( "data\\tindalos\\Hlms\\Unlit\\HLSL", "FileSystem", true );
    mUnlitMaterials = OGRE_NEW Ogre::HlmsUnlit( archiveUnlit, &library );
    mRoot->getHlmsManager()->registerHlms( mUnlitMaterials, false );

    auto archivePBS = mgr->load( "data\\tindalos\\Hlms\\Pbs\\HLSL", "FileSystem", true );
    mPbsMaterials = OGRE_NEW Ogre::HlmsPbs( archivePBS, &library );
    mRoot->getHlmsManager()->registerHlms( mPbsMaterials, false );
  }

  void Graphics::unregisterHlms()
  {
    mEngine->getConsole()->printf( Console::srcGfx,
      L"Unregistering HLMS..." );

    // FIXME: not deleting nets us memory leaks; deleting excepts with "datablock still in use".
    /*if ( mPbsMaterials )
    {
      mRoot->getHlmsManager()->unregisterHlms( mPbsMaterials->getType() );
      OGRE_DELETE mPbsMaterials;
      mPbsMaterials = nullptr;
    }

    if ( mUnlitMaterials )
    {
      mRoot->getHlmsManager()->unregisterHlms( mUnlitMaterials->getType() );
      OGRE_DELETE( mUnlitMaterials );
      mUnlitMaterials = nullptr;
    }*/
  }

  void Graphics::applySettings( const Settings& settings )
  {
    mEngine->getConsole()->printf( Console::srcGfx,
      L"Applying new graphics settings..." );

    bool newFullscreen = ( settings.fullScreen != mSettings.fullScreen );
    bool newVSync = ( settings.verticalSync != mSettings.verticalSync );

    if ( newFullscreen )
    {
      unsigned int width  = g_CVar_vid_screenwidth.getInt();
      unsigned int height = g_CVar_vid_screenheight.getInt();
      g_CVar_vid_fullscreen.setValue( settings.fullScreen );
      mWindow->setFullscreen( settings.fullScreen, width, height );
    }

    if ( newVSync )
    {
      g_CVar_vid_vsync.setValue( settings.verticalSync );
      mWindow->setVSyncEnabled( settings.verticalSync );
    }

    // if ( newFullscreen || newVSync )
    //   videoRestart();
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
        mSceneManager->clearScene();
        if ( mOverlaySystem )
          mSceneManager->removeRenderQueueListener( mOverlaySystem );
        mRoot->destroySceneManager( mSceneManager );
      }

      mEngine->unregisterResources( ResourceGroupManager::getSingleton() );
      mEngine->unregisterUserLocations( ResourceGroupManager::getSingleton() );

      // SAFE_DELETE( mPostProcessing );
      // SAFE_DELETE( mShadows );
      Ogre::MeshManager::getSingleton().removeAll();
      Ogre::v1::MeshManager::getSingleton().removeAll();

      //SAFE_DELETE( mOverlaySystem );
      unregisterHlms();
      mRoot->shutdown();
      //SAFE_DELETE( mRoot ); FIXME: Crash? shouldn't happen, according to docs
      mRenderer = nullptr;
    }
  }

  void Graphics::setRenderWindowTitle( const string& title )
  {
    SetWindowTextW( getRenderWindowHandle(), Utilities::utf8ToWide( title ).c_str() );
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
    manager.addResourceLocation( "data\\tindalos\\2.0\\scripts\\Compositors", "FileSystem", "Bootload", true );
    manager.addResourceLocation( "data\\tindalos\\2.0\\scripts\\materials\\Common", "FileSystem", "Bootload", true );
    manager.addResourceLocation( "data\\tindalos\\2.0\\scripts\\materials\\Common\\HLSL", "FileSystem", "Bootload", true );
    manager.addResourceLocation( "data\\tindalos\\2.0\\scripts\\materials\\HDR", "FileSystem", "Bootload", true );
    manager.addResourceLocation( "data\\tindalos\\2.0\\scripts\\materials\\HDR\\HLSL", "FileSystem", "Bootload", true );
    manager.addResourceLocation( "data\\tindalos\\2.0\\scripts\\materials\\PbsMaterials", "FileSystem", "Bootload", true );
    manager.addResourceLocation( "data\\tindalos\\models", "FileSystem", "Bootload", true );
    manager.addResourceLocation( "data\\tindalos\\materials\\textures", "FileSystem", "Bootload", true );
    manager.addResourceLocation( "data\\tindalos\\materials\\textures\\Cubemaps", "FileSystem", "Bootload", true );
    ResourceGroupManager::getSingleton().initialiseResourceGroup( "Bootload" );
  }

  void Graphics::unregisterResources( ResourceGroupManager& manager )
  {
    gEngine->getConsole()->printf( Console::srcGfx,
      L"Unregistering resources..." );

    // manager.destroyResourceGroup( "Bootload" );
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

    // Update post processing
    // mPostProcessing->update( delta );

    // Render frame
    if ( !mRoot->renderOneFrame( (Ogre::Real)delta ) )
      ENGINE_EXCEPT( "Failed to render frame" );
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

    /*if ( Ogre::MaterialManager::getSingletonPtr() )
      Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(
        method );*/

    return true;
  }

  bool Graphics::callbackCVARTextureAnisotropy(
  ConVar* variable, ConVar::Value oldValue )
  {
    /*if ( Ogre::MaterialManager::getSingletonPtr() )
      Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(
        variable->getInt() );*/

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