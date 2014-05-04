#pragma once
#include "Utilities.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  enum BindAction {
    Action_None = 0,
    Action_Move_Forward,
    Action_Move_Backward,
    Action_Sidestep_Left,
    Action_Sidestep_Right,
    Action_Jump,
    Action_Crouch,
    Action_Run,
    Action_Zoom,
    Action_Toggle_Console = 900
  };

  enum PlayerMoveAction {
    Player_Move_None = 0,
    Played_Move_Forward,
    Player_Move_Backward
  };

  enum PlayerSidestepAction {
    Player_Sidestep_None = 0,
    Player_Sidestep_Left,
    Player_Sidestep_Right
  };

  enum PlayerJumpAction {
    Player_Jump_None = 0,
    Player_Jump_Keydown,
    Player_Jump_Keyup
  };

  enum PlayerRunAction {
    Player_Run_None = 0,
    Player_Run_Keydown,
    Player_Run_Keyup
  };

  enum PlayerCrouchAction {
    Player_Crouch_None = 0,
    Player_Crouch_Keydown,
    Player_Crouch_Keyup
  };

  struct KeyBindTable {
    BindAction nKeyboard[0xFF];
    BindAction nMouse[8];
    void reset() {
      memset( nKeyboard, Action_None, 0xFF * sizeof( BindAction ) );
      memset( nMouse, Action_None, 8 * sizeof( BindAction ) );
    }
    KeyBindTable() {
      reset();
    }
  };

  struct ActionPacket {
  public:
    PlayerMoveAction nMove;
    PlayerSidestepAction nSidestep;
    PlayerJumpAction nJump;
    PlayerRunAction nRun;
    PlayerCrouchAction nCrouch;
  };

  class CameraController: public boost::noncopyable {
  protected:
    Engine* mEngine;
    Vector3 mPersistentMovement;
    Vector3 mImpulseMovement;
    Vector3 mMovement;
    bool mZooming;
    bool mRotating;
    void updateMovement();
  public:
    explicit CameraController( Engine* engine );
    ~CameraController();
    void resetMovement();
    void prepare();
    void applyMouseMovement( const Nil::MouseState& state );
    void applyMouseWheel( const Nil::MouseState& state );
    void onMouseButtonPressed( size_t button );
    void onMouseButtonReleased( size_t button );
    void setPersistentMovement( const Vector2& vecMovement );
    void setZooming( const bool zoom );
    inline const Vector3& getMovement() throw() { return mMovement; }
  };

  class ActionManager: public boost::noncopyable,
  public Nil::MouseListener, public Nil::KeyboardListener {
  friend class InputHandler;
  friend class GamepadDevice;
  friend class ThumbstickComponent;
  friend class ButtonComponent;
  friend class DirectionalComponent;
  public:
    static KeyBindTable mKeyTable;
  protected:
    Engine* mEngine;
    ActionPacket mActions;
    CameraController* mCameraController;
    Vector2 mDirectional; // hack hack hack...
    void processBindPress( const BindAction& nBind );
    void processBindRelease( const BindAction& nBind );
  public:
    explicit ActionManager( Engine* pEngine );
    ~ActionManager();
    void prepare(); //!< Prepare for new input calls (new frame)
    // Mouse handlers
    virtual void onMouseMoved(
      Nil::Mouse* mouse, const Nil::MouseState& state );
    virtual void onMouseButtonPressed(
      Nil::Mouse* mouse, const Nil::MouseState& state, size_t button );
    virtual void onMouseButtonReleased(
      Nil::Mouse* mouse, const Nil::MouseState& state, size_t button );
    virtual void onMouseWheelMoved(
      Nil::Mouse* mouse, const Nil::MouseState& state );
    // Keyboard handlers
    virtual void onKeyPressed(
      Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
    virtual void onKeyRepeat(
      Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
    virtual void onKeyReleased(
      Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
    void onKeyPressed( const Nil::VirtualKeyCode key );
    void onKeyReleased( const Nil::VirtualKeyCode key );
    void clearActions();
    void clearActivators();
    inline const ActionPacket& getActions() throw() { return mActions; }
    CameraController* getCameraController() throw() { return mCameraController; }
    // hack hack hack...
    void setDirectional( const Vector2& vecDirecitonal ) { mDirectional = vecDirecitonal; }
    inline const Vector2& getDirectional() throw() { return mDirectional; }
  };

}