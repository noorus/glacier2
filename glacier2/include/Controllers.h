#pragma once
#include "Utilities.h"
#include "Actions.h"
#include "Character.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class InputDevice;
  class GameCamera;
  class Character;

  typedef std::list<InputDevice*> InputDeviceList;

  class BaseController {
  public:
    virtual void prepare() = 0;
    virtual void apply() = 0;
  };

  class GameController: virtual public BaseController {
  protected:
    Character* mCharacter;
    ActionPacket actions_;
    Vector2 directional_;
  public:
    GameController();
    virtual void setCharacter( Character* character );
    virtual void resetActions();
    virtual void prepare();
    virtual void apply( CharacterMoveMode moveMode, const Vector3& direction );
    virtual const ActionPacket& getActions() throw( ) { return actions_; }
  };

  class CameraController: virtual public BaseController {
  protected:
    GameCamera* camera_;
    Vector3 persistentMovement_;
    Vector3 impulseMovement_;
    Vector3 movement_;
    bool zooming_;
    bool rotating_;
  public:
    CameraController();
    virtual void setCamera( GameCamera* camera );
    virtual void resetMovement();
    virtual void updateMovement();
    virtual void prepare();
    virtual void apply();
  };

  class LocalController: public GameController, public CameraController {
  public:
    enum Mode {
      Mode_KeyboardAndMouse,
      Mode_Gamepad
    };
  protected:
    InputDeviceList mDevices;
    Mode mode_;
    bool selecting_;
    void updateInputMode( InputDevice* device );
    bool shouldIgnoreInput( InputDevice* from );
  public:
    LocalController();
    virtual void beginAction( InputDevice* device, const BindAction& action );
    virtual void endAction( InputDevice* device, const BindAction& action );
    virtual void applyZoom( InputDevice* device, const Real zoom );
    virtual void directionalMovement( InputDevice* device, const Vector2& directional );
    virtual void mouseMovement( InputDevice* device, const Mouse::MousePacket& packet );
    virtual void cameraPersistentMovement( InputDevice* device, const Vector2& movement );
    virtual const Mode& getMode();
    virtual void prepare();
    virtual void apply();
  };

}