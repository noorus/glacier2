#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "Actions.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "Controllers.h"
#include "InputManager.h"
#include "Character.h"
#include "Camera.h"
#include "Graphics.h"
#include "DeveloperEntities.h"
#include "EntityManager.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const Real cEdgeZone = 25.0f;

  LocalController::LocalController(): mode_( Mode_KeyboardAndMouse ), selecting_( false )
  {
    auto window = gEngine->getGraphics()->getWindow();
    viewport_.dimensions_ = Vector2( (Real)window->getWidth(), (Real)window->getHeight() );
  }

  void LocalController::updateInputMode( InputDevice* device )
  {
    Mode checkMode;

    if ( device->getType() == InputDevice::Type_Mouse
      || device->getType() == InputDevice::Type_Keyboard )
      checkMode = Mode_KeyboardAndMouse;
    else
      checkMode = Mode_Gamepad;

    if ( checkMode != mode_ )
    {
      mode_ = checkMode;
      resetActions();
      rotating_ = false;
      zooming_ = false;
      gEngine->getConsole()->printf( Console::srcInput,
        L"Switched input mode: %s",
        mode_ == Mode_KeyboardAndMouse ? L"Keyboard & Mouse" : L"Gamepad" );
    }
  }

  bool LocalController::shouldIgnoreInput( InputDevice* from )
  {
    if ( ( from->getType() == InputDevice::Type_Keyboard
      || from->getType() == InputDevice::Type_Mouse )
      && mode_ == Mode_KeyboardAndMouse )
      return false;
    else if ( from->getType() == InputDevice::Type_Gamepad && mode_ == Mode_Gamepad )
      return false;
    else
      return true;
  }

  void LocalController::beginAction( InputDevice* device,
  const BindAction& action )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    if ( action == Action_Command )
    {
      gEngine->getConsole()->printf( Console::srcEngine, L"Drop." );
      Ogre::Ray mouseRay;
      POINT pt;
      GetCursorPos( &pt );
      ScreenToClient( gEngine->getGraphics()->getRenderWindowHandle(), &pt );
      Vector2 mousePos(
        (Real)pt.x / (Real)gEngine->getGraphics()->getWindow()->getWidth(),
        (Real)pt.y / (Real)gEngine->getGraphics()->getWindow()->getHeight()
      );
      if ( mousePos.x >= 0.0f && mousePos.x <= 1.0f && mousePos.y >= 0.0f && mousePos.y <= 1.0f )
      {
        camera_->castViewportRay( mousePos, mouseRay );
        Ogre::Plane ground( Ogre::Vector3::UNIT_Y, 0 );
        auto intersect = mouseRay.intersects( ground );
        if ( intersect.first )
        {
          auto dropPos = mouseRay.getPoint( intersect.second );
          dropPos.y += 15.0f;
          auto cube = ( Entities::DevCube* )Locator::getEntities().create( "dev_cube" );
          cube->setType( Entities::DevCube::DevCube_050 );
          cube->spawn( dropPos, Quaternion::IDENTITY );
        }
      }
      return;
    }

    if ( action == Action_Select )
    {
      if ( !selecting_ )
      {
        selecting_ = true;
        if ( device->getType() == InputDevice::Type_Mouse )
        {
          auto mouse = static_cast<Mouse::Device*>( device );
          auto packet = mouse->getMousePacket();
          Locator::getHUD().beginSelection( packet );
        }
      }
      return;
    }

    switch ( action )
    {
      case Action_Move_Forward:
        if ( actions_.move == Character_Move_None )
          actions_.move = Character_Move_Forward;
        break;
      case Action_Move_Backward:
        if ( actions_.move == Character_Move_None )
          actions_.move = Character_Move_Backward;
        break;
      case Action_Sidestep_Left:
        if ( actions_.sidestep == Character_Sidestep_None )
          actions_.sidestep = Character_Sidestep_Left;
        break;
      case Action_Sidestep_Right:
        if ( actions_.sidestep == Character_Sidestep_None )
          actions_.sidestep = Character_Sidestep_Right;
        break;
      case Action_Jump:
        actions_.jump = Character_Jump_Keydown;
        break;
      case Action_Run:
        actions_.run = Character_Run_Keydown;
        break;
      case Action_Crouch:
        actions_.crouch = Character_Crouch_Keydown;
        break;
      case Action_Rotate:
        rotating_ = true;
        break;
      case Action_Zoom:
        zooming_ = true;
        break;
    }
  }

  void LocalController::endAction( InputDevice* device,
  const BindAction& action )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    if ( action == Action_Select )
    {
      if ( selecting_ )
      {
        selecting_ = false;
        if ( device->getType() == InputDevice::Type_Mouse )
        {
          auto mouse = static_cast<Mouse::Device*>( device );
          auto packet = mouse->getMousePacket();
          Locator::getHUD().endSelection( packet );
        }
      }
      return;
    }

    switch ( action )
    {
      case Action_Move_Forward:
        if ( actions_.move == Character_Move_Forward )
          actions_.move = Character_Move_None;
        break;
      case Action_Move_Backward:
        if ( actions_.move == Character_Move_Backward )
          actions_.move = Character_Move_None;
        break;
      case Action_Sidestep_Left:
        if ( actions_.sidestep == Character_Sidestep_Left )
          actions_.sidestep = Character_Sidestep_None;
        break;
      case Action_Sidestep_Right:
        if ( actions_.sidestep == Character_Sidestep_Right )
          actions_.sidestep = Character_Sidestep_None;
        break;
      case Action_Jump:
        actions_.jump = Character_Jump_Keyup;
        break;
      case Action_Run:
        actions_.run = Character_Run_Keyup;
        break;
      case Action_Crouch:
        actions_.crouch = Character_Crouch_Keyup;
        break;
      case Action_Rotate:
        rotating_ = false;
        break;
      case Action_Zoom:
        zooming_ = false;
        break;
    }
  }

  const LocalController::Mode& LocalController::getMode()
  {
    return mode_;
  }

  void LocalController::applyZoom( InputDevice* device, const Real zoom )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    impulseRotation_.z += zoom;
    updateMovement();
  }

  void LocalController::directionalMovement( InputDevice* device,
  const Vector2& directional )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    directional_ = directional;

    if ( !directional_.isZeroLength() && actions_.move == Character_Move_None )
      actions_.move = Character_Move_Forward;
    else if ( directional_.isZeroLength() && actions_.move == Character_Move_Forward )
      actions_.move = Character_Move_None;
  }

  void LocalController::mouseMovement( InputDevice* device, const Mouse::MousePacket& packet )
  {
    if ( shouldIgnoreInput( device ) )
      return;

    cameraEdgeScroll_ = 0.0f;
    if ( selecting_ )
      Locator::getHUD().updateSelection( packet );
    else {
      auto pt = viewport_.cap( packet.absolute_ );
      if ( pt.x <= cEdgeZone )
        cameraEdgeScroll_.x = -( 1.0f - ( pt.x / cEdgeZone ) );
      else if ( pt.x >= ( viewport_.dimensions_.x - cEdgeZone ) )
        cameraEdgeScroll_.x = ( 1.0f - ( ( viewport_.dimensions_.x - pt.x ) / cEdgeZone ) );
      if ( pt.y <= cEdgeZone )
        cameraEdgeScroll_.y = -( 1.0f - ( pt.y / cEdgeZone ) );
      else if ( pt.y >= ( viewport_.dimensions_.y - cEdgeZone ) )
        cameraEdgeScroll_.y = ( 1.0f - ( ( viewport_.dimensions_.y - pt.y ) / cEdgeZone ) );
    }

    /*if ( rotating_ )
    {
      impulseMovement_.x += movement.x;
      impulseMovement_.y += movement.y;
      updateMovement();
    }
    else if ( zooming_ )
    {
      impulseMovement_.z += movement.z;
      updateMovement();
    }*/
  }

  void LocalController::cameraPersistentMovement( InputDevice* device,
  const Vector2& movement )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    persistentRotation_.x = -movement.x;
    persistentRotation_.y = movement.y;
    updateMovement();
  }

  void LocalController::prepare()
  {
    GameController::prepare();
    CameraController::prepare();
  }

  void LocalController::apply()
  {
    CameraController::apply();

    CharacterMoveMode moveMode = (
      mode_ == Mode_KeyboardAndMouse
      ? CharacterMoveMode::Mode_Impulse
      : CharacterMoveMode::Mode_Directional );

    GameController::apply( moveMode, -camera_->getDirection() );
  }

}