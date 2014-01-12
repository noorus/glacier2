#pragma once
#include "Types.h"
#include "Console.h"

namespace Glacier {

  class Console;
  class ConsoleWindow;
  class ConsoleWindowThread;
  class Scripting;
  class Graphics;
  class ConCmd;
  class Game;
  class Sound;
  class Physics;

  ENGINE_EXTERN_CONCMD( version );
  ENGINE_EXTERN_CONCMD( screenshot );
  ENGINE_EXTERN_CONCMD( quit );

  //! \class Engine
  //! The main engine class that makes the world go round
  class Engine {
  public:
    enum Signal {
      Signal_None = 0,
      Signal_Stop
    };
    struct Version {
      uint32_t major;
      uint32_t minor;
      uint32_t build;
      wstring compiler;
      wstring compiled;
      wstring profile;
      wstring title;
      wstring subtitle;
      Version( uint32_t major, uint32_t minor, uint32_t build );
    };
  protected:
    Version mVersion;
    // Subsystems
    Console* mConsole;
    ConsoleWindowThread* mConsoleWindow;
    Scripting* mScripting;
    Graphics* mGraphics;
    Physics* mPhysics;
    Sound* mSound;
    Game* mGame;
    // Timing
    LARGE_INTEGER mHPCFrequency;        //!< HPC frequency
    static GameTime fTime;              //!< Game time
    static GameTime fTimeDelta;         //!< Game frame delta
    static GameTime fTimeAccumulator;   //!< Game frametime accumulator
    static GameTime fLogicStep;         //!< Game logic step
    // Handles
    HANDLE mProcess;                    //!< Process handle
    HANDLE mThread;                     //!< Main thread handle
    HINSTANCE mInstance;                //!< Instance handle
    volatile Signal mSignal;            //!< Engine signal
    void adjustPrivileges();
    void fixupThreadAffinity();
  public:
    // Getters
    const Version& getVersion() { return mVersion; }
    Console* getConsole() { return mConsole; }
    Scripting* getScripting() { return mScripting; }
    Graphics* getGraphics() { return mGraphics; }
    Physics* getPhysics() { return mPhysics; }
    Sound* getSound() { return mSound; }
    Game* getGame() { return mGame; }
    inline GameTime getTime() { return fTime; }
    // Callbacks
    static void callbackVersion( Console* console,
      ConCmd* command, StringVector& arguments );
    static void callbackScreenshot( Console* console,
      ConCmd* command, StringVector& arguments );
    static void callbackQuit( Console* console,
      ConCmd* command, StringVector& arguments );
  public:
    Engine( HINSTANCE instance );
    ~Engine();
    void operationSuspendVideo();       //!< Shutdown state for gfx restart
    void operationContinueVideo();      //!< Continue state after gfx restart
    void operationSuspendAudio();       //!< Shutdown state for audio restart
    void operationContinueAudio();      //!< Continue state after audio restart
    void initialize();
    void run();
    void shutdown();
  };

}

extern Glacier::Engine* gEngine;