#include "StdAfx.h"
#include "Graphics.h"
#include "Gorilla.h"
#include "EngineComponent.h"

namespace Glacier {

  // TODO move these to video start/shutdown called from GFX

  HUD::HUD( Engine* engine, Gorilla::Screen* screen ):
  EngineComponent( engine ), gorillaScreen_( screen ), gorillaLayer_( nullptr )
  {
    gorillaLayer_ = gorillaScreen_->createLayer();
    auto rect = gorillaLayer_->createRectangle( Vector2( -50.0f, 100.0f ), Vector2( 100.0f, 100.0f ) );
    ColourValue bgColor;
    bgColor.setAsRGBA( 0x59d60064 );
    ColourValue borderColor;
    borderColor.setAsRGBA( 0x90ff42dd );
    rect->background_colour( bgColor );
    rect->border_colour( borderColor );
    rect->border_width( 1.0f );
  }

  void HUD::beginSelection()
  {
    //
  }

  void HUD::endSelection()
  {
    //
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
    //
  }

}