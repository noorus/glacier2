#pragma once
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \class GUI
  //! Engine component responsible for graphical user interface concerns
  //! \sa EngineComponent
  class GUI: public EngineComponent {
  protected:
    MyGUI::OgrePlatform* mPlatform;
    MyGUI::Gui* mGUI;
  public:
    void initialize();
    void shutdown();
  public:
    GUI( Engine* engine );
    virtual ~GUI();
    static void registerResources( ResourceGroupManager& manager );
    static void unregisterResources( ResourceGroupManager& manager );
  };

  //! @}

}