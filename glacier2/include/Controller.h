#pragma once
#include "Utilities.h"
#include "ActionManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class InputDevice;
  class GameCamera;
  class Character;

  typedef std::list<InputDevice*> InputDeviceList;

  class Controller {
  protected:
    Character* mCharacter;
    GameCamera* mCamera;
    ActionPacket mActions;
  public:
    Controller();
    void setCharacter( Character* character );
    void setCamera( GameCamera* camera );
    void resetActions();
    void prepare();
    void apply();
    inline const ActionPacket& getActions() throw() { return mActions; }
  };

  class LocalController: public Controller {
  public:
    enum Mode {
      Mode_KeyboardAndMouse,
      Mode_Gamepad
    };
  protected:
    InputDeviceList mDevices;
    Mode mMode;
    void updateInputMode( InputDevice* device );
    bool shouldIgnoreInput( InputDevice* from );
  public:
    LocalController();
    void beginAction( InputDevice* device, const BindAction& action );
    void endAction( InputDevice* device, const BindAction& action );
    void applyZoom( InputDevice* device, const Real zoom );
    void directionalMovement( InputDevice* device, const Vector2& directional );
    void cameraMovement( InputDevice* device, const Vector2& movement );
    const Mode& getMode();
  };

}