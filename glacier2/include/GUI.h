#pragma once
#include "EngineComponent.h"
#include "AudioService.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class SettingsWindow {
  protected:
    MyGUI::Widget* mRoot;
    MyGUI::ComboBox* mAudioDrivers;
    MyGUI::ComboBox* mAudioOutputTypes;
    MyGUI::ComboBox* mAudioSpeakerModes;
    MyGUI::ScrollBar* mAudioMasterVolume;
    MyGUI::ScrollBar* mAudioMusicVolume;
    MyGUI::ScrollBar* mAudioEffectVolume;
    Audio::Settings mOriginalAudio;
    Audio::Settings mCurrentAudio;
    void refreshAudioDrivers();
    void refreshAudioOutputTypes();
    void refreshAudioSpeakerModes();
  public:
    SettingsWindow();
    ~SettingsWindow();
    void onVerticalSyncClicked( MyGUI::Widget* sender );
    void onFullscreenClicked( MyGUI::Widget* sender );
    void onApplyClicked( MyGUI::Widget* sender );
    void onCancelClicked( MyGUI::Widget* sender );
    void onAudioDriverSelected( MyGUI::ComboBox* sender, size_t index );
    void onAudioOutputTypeSelected( MyGUI::ComboBox* sender, size_t index );
    void onAudioSpeakerModeSelected( MyGUI::ComboBox* sender, size_t index );
    void onAudioMasterVolumeChanged( MyGUI::ScrollBar* sender, size_t position );
    void onAudioMusicVolumeChanged( MyGUI::ScrollBar* sender, size_t position );
    void onAudioEffectVolumeChanged( MyGUI::ScrollBar* sender, size_t position );
  };

  //! \class GUI
  //! Engine component responsible for graphical user interface concerns
  //! \sa EngineComponent
  class GUI: public EngineComponent {
  protected:
    MyGUI::OgrePlatform* mPlatform;
    MyGUI::Gui* mGUI;
    Nil::Vector2i mMousePosition;
  public:
    void initialize();
    void shutdown();
  public:
    GUI( Engine* engine );
    virtual ~GUI();
    bool injectMouseMove( const Nil::MouseState& state );
    bool injectMousePress( const Nil::MouseState& state, size_t button );
    bool injectMouseRelease( const Nil::MouseState& state, size_t button );
    static void registerResources( ResourceGroupManager& manager );
    static void unregisterResources( ResourceGroupManager& manager );
  };

  //! @}

}