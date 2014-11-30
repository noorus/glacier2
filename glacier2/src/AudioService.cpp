#include "StdAfx.h"
#include "Engine.h"
#include "Console.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "AudioService.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_DECLARE_CONVAR_WITH_CB( fm_volume, L"Master volume.",
    1.0f, Audio::callbackMasterVolume );
  ENGINE_DECLARE_CONVAR_WITH_CB( fm_bgvolume, L"Background music volume.",
    0.6f, Audio::callbackMusicVolume );
  ENGINE_DECLARE_CONVAR_WITH_CB( fm_fxvolume, L"Sound effect volume.",
    0.8f, Audio::callbackEffectVolume );

  bool Audio::callbackMasterVolume( ConVar* variable, ConVar::Value oldValue )
  {
    float volume = boost::algorithm::clamp( variable->getFloat(), 0.0f, 1.0f );
    Locator::getAudio().setMasterVolume( volume );
    return true;
  }

  bool Audio::callbackMusicVolume( ConVar* variable, ConVar::Value oldValue )
  {
    float volume = boost::algorithm::clamp( variable->getFloat(), 0.0f, 1.0f );
    Locator::getAudio().setMusicVolume( volume );
    return true;
  }

  bool Audio::callbackEffectVolume( ConVar* variable, ConVar::Value oldValue )
  {
    float volume = boost::algorithm::clamp( variable->getFloat(), 0.0f, 1.0f );
    Locator::getAudio().setEffectVolume( volume );
    return true;
  }

}