#pragma once
#include "Console.h"
#include "EngineComponent.h"
#include "Services.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  struct Track {
  public:
    std::wstring location;
  };

  class FMODMusic: public Music {
  protected:
    FMODAudio* mAudio;
    FMOD::Channel* mChannels[3];
    FMOD::Sound* mSound;
    FMOD_SYNCPOINT* mSyncPoint;
    std::vector<Track> mTracks;
    std::vector<Track>::iterator mIterator;
    size_t mCurrentChannel;
    bool mLoading;
    bool mFading;
  public:
    FMODMusic( FMODAudio* audio );
    virtual void beginScene();
    virtual void endScene();
    void loadGroup();
    void loadTrack( Track* track );
    void playLoadedTrack();
    void update( const GameTime delta );
    size_t getFreeChannel();
    size_t getPlayingChannel();
    ~FMODMusic();
    static FMOD_RESULT F_CALLBACK syncPointCallback( FMOD_CHANNEL* channel,
      FMOD_CHANNEL_CALLBACKTYPE type, void* data1, void* data2 );
  };

}