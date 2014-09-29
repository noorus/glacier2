#pragma once
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class SettingsWindow {
  protected:
    MyGUI::Widget* mRoot;
  public:
    SettingsWindow();
    ~SettingsWindow();
    void onVerticalSyncClicked( MyGUI::Widget* sender );
    void onFullscreenClicked( MyGUI::Widget* sender );
    void onApplyClicked( MyGUI::Widget* sender );
    void onCancelClicked( MyGUI::Widget* sender );
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