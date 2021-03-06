#include "StdAfx.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Colors::Colors()
  {
    mColorTable[Color_Text_Plain] = ColourValue::White;
    mColorTable[Color_Text_Warning] = ColourValue( 1.0f, 1.0f, 0.0f, 1.0f );
    mColorTable[Color_Text_Error] = ColourValue::Red;
    mColorTable[Color_Text_Success] = ColourValue::Green;
  }

  const ColourValue& Colors::base( const Value index )
  {
    assert( index >= 0 && index < Color_MAX );

    return mColorTable[index];
  }

  void Colors::set( const Value index, uint32_t value )
  {
    assert( index >= 0 && index < Color_MAX );

    value = ( value << 8 ) | 0x000000FF;
    mColorTable[index].setAsRGBA( value );
  }

}