#include "StdAfx.h"
#include "Graphics.h"
#include "GlobalStats.h"
#include "Console.h"
#include "Exception.h"
#include "Engine.h"
#include <OgreFrameStats.h>

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  void GlobalStats::init()
  {
    mOverlay = Ogre::v1::OverlayManager::getSingleton().getByName(
      "Ingame/Debug/Stats" );
    mNamesText = Ogre::v1::OverlayManager::getSingleton().getOverlayElement(
      "Ingame/Debug/Stats/Names" );
    mValuesText = Ogre::v1::OverlayManager::getSingleton().getOverlayElement(
      "Ingame/Debug/Stats/Values" );
    setEnabled( true );
  }

  void GlobalStats::setEnabled( bool b )
  {
    if ( !mOverlay ) {
      mEnabled = b;
      return;
    }
    if ( mEnabled != b ) {
      if ( b )
        mOverlay->show();
      else
        mOverlay->hide();
    }
    mEnabled = b;
  }

  void GlobalStats::update()
  {
    if ( !mEnabled )
      return;

    mNamesText->setCaption( "AvgFPS:\r\nAvgTime:" );

    auto stats = gEngine->getGraphics()->getRoot()->getFrameStats();

    static wchar_t values[64];
    swprintf_s( values, 64,
      L"%0.2f\r\n%0.2f",
      stats->getAvgFps(), stats->getAvgTime() );

    mValuesText->setCaption( Ogre::UTFString( values ) );
  }

  void GlobalStats::shutdown()
  {
    mOverlay = nullptr;
  }

}