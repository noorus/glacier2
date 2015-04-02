#pragma once
#include "Utilities.h"
#include "ActionManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class InputDevice;
  class GameCamera;

  typedef std::list<InputDevice*> InputDeviceList;

  class Controller {
  protected:
    GameCamera* mCamera;
    ActionPacket mActions;
  public:
    Controller();
    void setCamera( GameCamera* camera );
    void resetActions();
    void prepare();
    inline const ActionPacket& getActions() throw( ) { return mActions; }
  };

  class LocalController: public Controller {
  protected:
    InputDeviceList mDevices;
  public:
    void beginAction( InputDevice* device, const BindAction& action );
    void endAction( InputDevice* device, const BindAction& action );
    void applyZoom( InputDevice* device, const Real zoom );
    void directionalMovement( InputDevice* device, const Vector2& directional );
    void cameraMovement( InputDevice* device, const Vector2& movement );
  };

}