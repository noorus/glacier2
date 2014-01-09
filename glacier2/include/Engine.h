#pragma once
#include "Types.h"

namespace Glacier {

  class Console;
  class ConsoleWindow;
  class ConsoleWindowThread;
  class Scripting;
  class Graphics;
  class ConCmd;

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
    static void callbackVersion( Console* console, ConCmd* command, StringVector& arguments );
  public:
    Engine( HINSTANCE instance );
    ~Engine();
    void operationSuspendVideo(); //!< shutdown state for gfx restart
    void operationContinueVideo(); //!< continues state after gfx restart
    void operationSuspendAudio(); //!< shutdown state for audio restart
    void operationContinueAudio(); //!< continues state after audio restart
    void initialize();
    void run();
    void shutdown();
    const Version& getVersion() { return mVersion; }
    Console* getConsole() { return mConsole; }
    Scripting* getScripting() { return mScripting; }
    Graphics* getGraphics() { return mGraphics; }
  };

}

extern Glacier::Engine* gEngine;