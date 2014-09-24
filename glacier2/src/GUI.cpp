#include "StdAfx.h"
#include "GUI.h"
#include "Engine.h"
#include "Console.h"
#include "Exception.h"
#include "Graphics.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const std::string cGUIResourceGroup = "GUI";
  const std::string cGUILogFile = "MyGUI.log";
  const std::string cGUICoreFile = "MyGUI_Core.xml";

  GUI::GUI( Engine* engine ): EngineComponent( engine ),
  mPlatform( nullptr ), mGUI( nullptr )
  {
    mPlatform = new MyGUI::OgrePlatform();
    mGUI = new MyGUI::Gui();
  }

  void GUI::initialize()
  {
    mEngine->getConsole()->printf( Console::srcGUI,
      L"Initializing MyGUI v%d.%d.%d", MYGUI_VERSION_MAJOR,
      MYGUI_VERSION_MINOR, MYGUI_VERSION_PATCH );

    mPlatform->initialise(
      mEngine->getGraphics()->getWindow(),
      mEngine->getGraphics()->getScene(),
      cGUIResourceGroup,
      cGUILogFile );

    mGUI->initialise( cGUICoreFile );
  }

  void GUI::shutdown()
  {
    mEngine->getConsole()->printf( Console::srcGUI,
      L"Shutting down MyGUI" );

    mGUI->shutdown();

    mPlatform->shutdown();
  }

  void GUI::registerResources( ResourceGroupManager& manager )
  {
    gEngine->getConsole()->printf( Console::srcGUI,
      L"Registering resources..." );

    manager.addResourceLocation( "data\\gui", "FileSystem", cGUIResourceGroup );
  }

  void GUI::unregisterResources( ResourceGroupManager& manager )
  {
    gEngine->getConsole()->printf( Console::srcGUI,
      L"Unregistering resources..." );

    manager.removeResourceLocation( "data\\gui", cGUIResourceGroup );
  }

  GUI::~GUI()
  {
    SAFE_DELETE( mGUI );
    SAFE_DELETE( mPlatform );
  }

}