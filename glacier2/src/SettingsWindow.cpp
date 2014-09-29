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

    mOriginalAudio = Locator::getAudio().getSettings();
    mCurrentAudio = mOriginalAudio;

    mAudioDrivers = mRoot->findWidget( "cbbDevice" )->castType<MyGUI::ComboBox>();
    mAudioDrivers->eventComboAccept += MyGUI::newDelegate( this, &SettingsWindow::onAudioDriverSelected );
    mAudioOutputTypes = mRoot->findWidget( "cbbOutputType" )->castType<MyGUI::ComboBox>();
    mAudioOutputTypes->eventComboAccept += MyGUI::newDelegate( this, &SettingsWindow::onAudioOutputTypeSelected );
    mAudioSpeakerModes = mRoot->findWidget( "cbbSpeakerMode" )->castType<MyGUI::ComboBox>();
    mAudioSpeakerModes->eventComboAccept += MyGUI::newDelegate( this, &SettingsWindow::onAudioSpeakerModeSelected );

    refreshAudioDrivers();
    refreshAudioOutputTypes();
    refreshAudioSpeakerModes();

    // Apply button
    MyGUI::Button* apply = mRoot->findWidget( "btnApply" )->castType<MyGUI::Button>();
    apply->eventMouseButtonClick += MyGUI::newDelegate( this, &SettingsWindow::onApplyClicked );

    // Cancel button
    MyGUI::Button* cancel = mRoot->findWidget( "btnCancel" )->castType<MyGUI::Button>();
    cancel->eventMouseButtonClick += MyGUI::newDelegate( this, &SettingsWindow::onCancelClicked );
  }

  void SettingsWindow::refreshAudioDrivers()
  {
    mAudioDrivers->removeAllItems();
    
    for ( auto driver : Locator::getAudio().getDrivers() )
    {
      mAudioDrivers->insertItemAt( driver->index, driver->name );
      if ( driver->index == mCurrentAudio.driver )
        mAudioDrivers->setIndexSelected( driver->index );
    }
  }

  void SettingsWindow::onAudioDriverSelected( MyGUI::ComboBox* sender, size_t index )
  {
    mCurrentAudio.driver = index;
    gEngine->getConsole()->printf( Console::srcGUI, L"Audio driver selected: %d", mCurrentAudio.driver );
  }

  void SettingsWindow::refreshAudioOutputTypes()
  {
    mAudioOutputTypes->removeAllItems();

    for ( auto type : Locator::getAudio().getOutputTypes() )
    {
      mAudioOutputTypes->insertItemAt( type->index, type->name );
      if ( type->index == mCurrentAudio.outputType )
        mAudioOutputTypes->setIndexSelected( type->index );
    }
  }

  void SettingsWindow::onAudioOutputTypeSelected( MyGUI::ComboBox* sender, size_t index )
  {
    mCurrentAudio.outputType = index;
    gEngine->getConsole()->printf( Console::srcGUI, L"Audio output type selected: %d", mCurrentAudio.outputType );
  }

  void SettingsWindow::refreshAudioSpeakerModes()
  {
    mAudioSpeakerModes->removeAllItems();

    for ( auto mode : Locator::getAudio().getSpeakerModes() )
    {
      mAudioSpeakerModes->insertItemAt( mode->index, mode->name );
      if ( mode->index == mCurrentAudio.speakerMode )
        mAudioSpeakerModes->setIndexSelected( mode->index );
    }
  }

  void SettingsWindow::onAudioSpeakerModeSelected( MyGUI::ComboBox* sender, size_t index )
  {
    mCurrentAudio.speakerMode = index;
    gEngine->getConsole()->printf( Console::srcGUI, L"Audio speaker mode selected: %d", mCurrentAudio.speakerMode );
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