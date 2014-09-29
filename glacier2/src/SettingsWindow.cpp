#include "StdAfx.h"
#include "GUI.h"
#include "Engine.h"
#include "Console.h"
#include "Exception.h"
#include "Graphics.h"
#include "Win32.h"
#include "OgreD3D9RenderSystem.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  SettingsWindow::SettingsWindow(): mRoot( nullptr )
  {
    const MyGUI::VectorWidgetPtr& root = MyGUI::LayoutManager::getInstance().loadLayout( "Settings.layout" );
    if ( root.size() != 1 )
      ENGINE_EXCEPT( L"Settings window layout has != 1 roots" );

    mRoot = root.at( 0 );

    Ogre::ConfigOptionMap options = Locator::getGraphics().getRenderer()->getConfigOptions();

    // Video : Adapter
    MyGUI::ComboBox* adapter = mRoot->findWidget( "cbbAdapter" )->castType<MyGUI::ComboBox>();
    adapter->removeAllItems();
    for ( auto str : options["Rendering Device"].possibleValues )
      adapter->addItem( str );
    adapter->setIndexSelected( 0 );

    // Video : Video Mode
    MyGUI::ComboBox* videoMode = mRoot->findWidget( "cbbVideoMode" )->castType<MyGUI::ComboBox>();
    videoMode->removeAllItems();
    Ogre::ConfigOptionMap opts = Locator::getGraphics().getRenderer()->getConfigOptions();
    for ( auto str : options["Video Mode"].possibleValues )
      videoMode->addItem( str );
    videoMode->setIndexSelected( 0 );

    // Video : Vertical Sync
    MyGUI::Button* verticalSync = mRoot->findWidget( "cbVerticalSync" )->castType<MyGUI::Button>();
    verticalSync->eventMouseButtonClick += MyGUI::newDelegate( this, &SettingsWindow::onVerticalSyncClicked );

    // Video : Fullscreen
    MyGUI::Button* fullScreen = mRoot->findWidget( "cbFullscreen" )->castType<MyGUI::Button>();
    fullScreen->eventMouseButtonClick += MyGUI::newDelegate( this, &SettingsWindow::onFullscreenClicked );

    // Audio : Driver
    MyGUI::ComboBox* audioDevice = mRoot->findWidget( "cbbDevice" )->castType<MyGUI::ComboBox>();
    audioDevice->removeAllItems();
    for ( auto driver : Locator::getAudio().getDrivers() )
      audioDevice->addItem( driver.name );
    audioDevice->setIndexSelected( 0 );

    // Audio : Output Type
    MyGUI::ComboBox* audioOutputType = mRoot->findWidget( "cbbOutputType" )->castType<MyGUI::ComboBox>();
    audioOutputType->removeAllItems();
    for ( auto type : Locator::getAudio().getOutputTypes() )
      audioOutputType->addItem( type.name );
    audioOutputType->setIndexSelected( 0 );

    // Audio : Speaker Mode
    MyGUI::ComboBox* audioSpeakerMode = mRoot->findWidget( "cbbSpeakerMode" )->castType<MyGUI::ComboBox>();
    audioSpeakerMode->removeAllItems();
    for ( auto mode : Locator::getAudio().getSpeakerModes() )
      audioSpeakerMode->addItem( mode.name );
    audioSpeakerMode->setIndexSelected( 0 );

    // Apply button
    MyGUI::Button* apply = mRoot->findWidget( "btnApply" )->castType<MyGUI::Button>();
    apply->eventMouseButtonClick += MyGUI::newDelegate( this, &SettingsWindow::onApplyClicked );

    // Cancel button
    MyGUI::Button* cancel = mRoot->findWidget( "btnCancel" )->castType<MyGUI::Button>();
    cancel->eventMouseButtonClick += MyGUI::newDelegate( this, &SettingsWindow::onCancelClicked );
  }

  void SettingsWindow::onVerticalSyncClicked( MyGUI::Widget* sender )
  {
    MyGUI::Button* btn = sender->castType<MyGUI::Button>();
    bool selected = !btn->getStateSelected();
    btn->setStateSelected( selected );
  }

  void SettingsWindow::onFullscreenClicked( MyGUI::Widget* sender )
  {
    MyGUI::Button* btn = sender->castType<MyGUI::Button>();
    bool selected = !btn->getStateSelected();
    btn->setStateSelected( selected );
  }

  void SettingsWindow::onApplyClicked( MyGUI::Widget* sender )
  {
    //
  }

  void SettingsWindow::onCancelClicked( MyGUI::Widget* sender )
  {
    //
  }

  SettingsWindow::~SettingsWindow()
  {
    //
  }

}