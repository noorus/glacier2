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
      ENGINE_EXCEPT( "Settings window layout has != 1 roots" );

    mRoot = root.at( 0 );

    mOriginalGraphics = Locator::getGraphics().getSettings();
    mCurrentGraphics = mOriginalGraphics;

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

    mAudioDrivers = mRoot->findWidget( "cbbDrivers" )->castType<MyGUI::ComboBox>();
    mAudioDrivers->eventComboAccept += MyGUI::newDelegate( this, &SettingsWindow::onAudioDriverSelected );
    mAudioOutputTypes = mRoot->findWidget( "cbbOutputTypes" )->castType<MyGUI::ComboBox>();
    mAudioOutputTypes->eventComboAccept += MyGUI::newDelegate( this, &SettingsWindow::onAudioOutputTypeSelected );
    mAudioSpeakerModes = mRoot->findWidget( "cbbSpeakerModes" )->castType<MyGUI::ComboBox>();
    mAudioSpeakerModes->eventComboAccept += MyGUI::newDelegate( this, &SettingsWindow::onAudioSpeakerModeSelected );

    mAudioMasterVolume = mRoot->findWidget( "sbMasterVolume" )->castType<MyGUI::ScrollBar>();
    mAudioMasterVolume->eventScrollChangePosition += MyGUI::newDelegate(
      this, &SettingsWindow::onAudioMasterVolumeChanged );
    mAudioMasterVolume->setScrollPosition( (size_t)( g_CVar_fm_volume.getFloat() * 100.0f ) );

    mAudioMusicVolume = mRoot->findWidget( "sbMusicVolume" )->castType<MyGUI::ScrollBar>();
    mAudioMusicVolume->eventScrollChangePosition += MyGUI::newDelegate(
      this, &SettingsWindow::onAudioMusicVolumeChanged );
    mAudioMusicVolume->setScrollPosition( (size_t)( g_CVar_fm_bgvolume.getFloat() * 100.0f ) );

    mAudioEffectVolume = mRoot->findWidget( "sbEffectVolume" )->castType<MyGUI::ScrollBar>();
    mAudioEffectVolume->eventScrollChangePosition += MyGUI::newDelegate(
      this, &SettingsWindow::onAudioEffectVolumeChanged );
    mAudioEffectVolume->setScrollPosition( (size_t)( g_CVar_fm_fxvolume.getFloat() * 100.0f ) );

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
    
    const Audio::DriverVector& drivers = Locator::getAudio().getDrivers();
    for ( size_t i = 0; i < drivers.size(); i++ )
    {
      mAudioDrivers->insertItemAt( i, drivers[i]->name );
      if ( i == mCurrentAudio.driver )
        mAudioDrivers->setIndexSelected( i );
    }
  }

  void SettingsWindow::onAudioDriverSelected( MyGUI::ComboBox* sender, size_t index )
  {
    mCurrentAudio.driver = (int)index;
  }

  void SettingsWindow::refreshAudioOutputTypes()
  {
    mAudioOutputTypes->removeAllItems();

    const Audio::OutputTypeVector& types = Locator::getAudio().getOutputTypes();
    for ( size_t i = 0; i < types.size(); i++ )
    {
      mAudioOutputTypes->insertItemAt( i, types[i]->name );
      if ( i == mCurrentAudio.outputType )
        mAudioOutputTypes->setIndexSelected( i );
    }
  }

  void SettingsWindow::onAudioOutputTypeSelected( MyGUI::ComboBox* sender, size_t index )
  {
    mCurrentAudio.outputType = (int)index;
  }

  void SettingsWindow::refreshAudioSpeakerModes()
  {
    mAudioSpeakerModes->removeAllItems();

    const Audio::SpeakerModeVector& modes = Locator::getAudio().getSpeakerModes();
    for ( size_t i = 0; i < modes.size(); i++ )
    {
      mAudioSpeakerModes->insertItemAt( i, modes[i]->name );
      if ( i == mCurrentAudio.speakerMode )
        mAudioSpeakerModes->setIndexSelected( i );
    }
  }

  void SettingsWindow::onAudioSpeakerModeSelected( MyGUI::ComboBox* sender, size_t index )
  {
    mCurrentAudio.speakerMode = (int)index;
  }

  void SettingsWindow::onVerticalSyncClicked( MyGUI::Widget* sender )
  {
    MyGUI::Button* btn = sender->castType<MyGUI::Button>();
    bool selected = !btn->getStateSelected();
    mCurrentGraphics.verticalSync = selected;
    btn->setStateSelected( selected );
  }

  void SettingsWindow::onFullscreenClicked( MyGUI::Widget* sender )
  {
    MyGUI::Button* btn = sender->castType<MyGUI::Button>();
    bool selected = !btn->getStateSelected();
    mCurrentGraphics.fullScreen = selected;
    btn->setStateSelected( selected );
  }

  void SettingsWindow::onAudioMasterVolumeChanged( MyGUI::ScrollBar* sender, size_t position )
  {
    float volume = boost::algorithm::clamp( (float)position / 100.0f, 0.0f, 1.0f );
    g_CVar_fm_volume.setValue( volume );
  }

  void SettingsWindow::onAudioMusicVolumeChanged( MyGUI::ScrollBar* sender, size_t position )
  {
    float volume = boost::algorithm::clamp( (float)position / 100.0f, 0.0f, 1.0f );
    g_CVar_fm_bgvolume.setValue( volume );
  }

  void SettingsWindow::onAudioEffectVolumeChanged( MyGUI::ScrollBar* sender, size_t position )
  {
    float volume = boost::algorithm::clamp( (float)position / 100.0f, 0.0f, 1.0f );
    g_CVar_fm_fxvolume.setValue( volume );
  }

  void SettingsWindow::onApplyClicked( MyGUI::Widget* sender )
  {
    Locator::getGraphics().applySettings( mCurrentGraphics );

    mOriginalGraphics = Locator::getGraphics().getSettings();
    mCurrentGraphics = mOriginalGraphics;

    /*
    Locator::getAudio().applySettings( mCurrentAudio );

    bool newOutput   = ( mCurrentAudio.outputType != mOriginalAudio.outputType );
    bool newDriver   = ( mCurrentAudio.driver != mOriginalAudio.driver );
    bool newSpeakers = ( mCurrentAudio.speakerMode != mOriginalAudio.speakerMode );

    mOriginalAudio = Locator::getAudio().getSettings();
    mCurrentAudio = mOriginalAudio;

    if ( newOutput )
      refreshAudioOutputTypes();
    if ( newDriver )
      refreshAudioDrivers();
    if ( newSpeakers )
      refreshAudioSpeakerModes();*/
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