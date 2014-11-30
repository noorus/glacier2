#include "StdAfx.h"
#include "FMODAudio.h"
#include "Engine.h"
#include "Console.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "FMODMusic.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

# define FMOD_FAILED(hr) (hr != FMOD_OK)

  ENGINE_DECLARE_CONVAR( dev_bgmusic,
    L"Temporary background music filename", L"" );

  FMODMusic::FMODMusic( FMODAudio* audio ): mAudio( audio ), mCurrentChannel( 0 )
  {
    memset( mChannels, 0, sizeof( mChannels ) );

    loadGroup();
  }

  void FMODMusic::beginScene()
  {
    random_shuffle( mTracks.begin(), mTracks.end() );
    mIterator = mTracks.begin();
    if ( mIterator != mTracks.end() )
      loadTrack( &( *mIterator ) );
  }

  void FMODMusic::endScene()
  {
    //
  }

  void FMODMusic::loadGroup()
  {
    mTracks.clear();

    Track track;
    track.location = g_CVar_dev_bgmusic.getString();
    mTracks.push_back( track );
  }

  void FMODMusic::loadTrack( Track* track )
  {
    FMOD_RESULT hr;

    hr = mAudio->getSystem()->createStream( (const char*)track->location.c_str(),
      FMOD_HARDWARE | FMOD_UNICODE | FMOD_2D | FMOD_LOOP_NORMAL | FMOD_NONBLOCKING,
      0, &mSound );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Creating music stream failed" );

    mLoading = true;
  }

  void FMODMusic::playLoadedTrack()
  {
    FMOD_RESULT hr;

    mLoading = false;

    unsigned int length;
    mSound->getLength( &length, FMOD_TIMEUNIT_MS );
    mSound->addSyncPoint( length - 5000, FMOD_TIMEUNIT_MS, "fadeout", &mSyncPoint );

    auto fma = (FMODAudio*)( &Locator::getAudio() );
    hr = fma->getSystem()->playSound( FMOD_CHANNEL_REUSE, mSound, false, &mChannels[getFreeChannel()] );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Playing music track failed" );

    mChannels[mCurrentChannel]->setCallback( &syncPointCallback );
    mChannels[mCurrentChannel]->setVolume( 0.0f );
    mChannels[mCurrentChannel]->setChannelGroup( fma->mMusicGroup );

    mFading = true;
  }

  void FMODMusic::update( const GameTime delta )
  {
    FMOD_RESULT hr;

    if ( mLoading )
    {
      FMOD_OPENSTATE state;
      mSound->getOpenState( &state, 0, 0, 0 );
      if ( state == FMOD_OPENSTATE_READY )
        playLoadedTrack();
    }

    if ( mFading )
    {
      Real newVolume;

      auto oc = getPlayingChannel();
      if ( mChannels[oc] != 0 )
      {
        mChannels[oc]->getVolume( &newVolume );
        newVolume -= 0.2f * delta;
        mChannels[oc]->setVolume( newVolume > 0.0f ? newVolume : 0.0f );
        if ( newVolume <= 0.0f )
          mChannels[oc]->stop();
      }

      mChannels[mCurrentChannel]->getVolume( &newVolume );
      newVolume += 0.2f * delta;
      mChannels[mCurrentChannel]->setVolume( newVolume < 1.0f ? newVolume : 1.0f );
      if ( newVolume >= 1.0f )
        mFading = false;
    }
  }

  size_t FMODMusic::getFreeChannel()
  {
    mCurrentChannel = ( mCurrentChannel == 0 ? 1 : 0 );
    return mCurrentChannel;
  }

  size_t FMODMusic::getPlayingChannel()
  {
    return ( mCurrentChannel == 0 ? 1 : 0 );
  }

  FMOD_RESULT F_CALLBACK FMODMusic::syncPointCallback(
  FMOD_CHANNEL* channel, FMOD_CHANNEL_CALLBACKTYPE type, void* data1, void* data2 )
  {
    if ( type == FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT )
    {
      auto mp = (FMODMusic*)( &Locator::getMusic() );
      if ( ++mp->mIterator == mp->mTracks.end() )
      {
        random_shuffle( mp->mTracks.begin(), mp->mTracks.end() );
        mp->mIterator = mp->mTracks.begin();
      }
      mp->loadTrack( &( *mp->mIterator ) );
    }

    return FMOD_OK;
  }

  FMODMusic::~FMODMusic()
  {
    //
  }

}