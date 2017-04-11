#include "StdAfx.h"
#include "Graphics.h"
#include "Gorilla.h"
#include "EngineComponent.h"
#include "Engine.h"

namespace Glacier {

  // TODO move these to video start/shutdown called from GFX

  HUD::HUD( Engine* engine, Gorilla::Screen* screen ):
  EngineComponent( engine ), gorillaScreen_( screen ), gorillaLayer_( nullptr )
  {
    gorillaLayer_ = gorillaScreen_->createLayer();
  }

  void HUD::beginSelection( const Mouse::MousePacket& from )
  {
    if ( selection_.vis_ )
      gorillaLayer_->destroyRectangle( selection_.vis_ );
    Vector2 xy, wh;
    selection_.rect_.getXYWH( xy, wh );
    selection_.vis_ = gorillaLayer_->createRectangle( xy, wh );

    selection_.from_ = from.absolute_;
    updateSelection( from );

    ColourValue bgColor;
    bgColor.setAsRGBA( 0x59d60064 );
    ColourValue borderColor;
    borderColor.setAsRGBA( 0x90ff42dd );
    selection_.vis_->background_colour( bgColor );
    selection_.vis_->border_colour( borderColor );
    selection_.vis_->border_width( 1.0f );
  }

  void HUD::updateSelection( const Mouse::MousePacket& to )
  {
    selection_.to_ = to.absolute_;
    selection_.rect_.setFrom( selection_.from_, selection_.to_ );
    if ( selection_.vis_ )
      selection_.vis_->coords( selection_.rect_.topLeft, selection_.rect_.bottomRight );
  }

  void HUD::endSelection( const Mouse::MousePacket& to )
  {
    if ( selection_.vis_ )
    {
      gorillaLayer_->destroyRectangle( selection_.vis_ );
      selection_.vis_ = nullptr;
    }
  }

  void HUD::componentPreUpdate( GameTime time )
  {
    //
  }

  void HUD::componentPostUpdate( GameTime delta, GameTime time )
  {
    //
  }

  HUD::~HUD()
  {
    gorillaScreen_->destroy( gorillaLayer_ );
  }

}