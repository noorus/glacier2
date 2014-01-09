#pragma once
#include "Types.h"

namespace Glacier {

  class Console;
  class ConsoleWindow;
  class ConsoleWindowThread;
  class Scripting;
  class Graphics;

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
    LARGE_INTEGER qwiTimerFrequency;      //!< HPC frequency
    LARGE_INTEGER qwiTimeNew;             //!< Newest time
    LARGE_INTEGER qwiTimeCurrent;         //!< Current time
    LARGE_INTEGER qwiTickDelta;           //!< Time delta
    static GameTime  fTime;               //!< Game time
    static LocalTime fTimeDelta;          //!< Game frame delta
    static GameTime  fTimeAccumulator;    //!< Game frametime accumulator
    static LocalTime fLogicStep;          //!< Game logic step
    // Handles
    HANDLE mProcess; //!< Process handle
    HANDLE mThread; //!< Main thread handle
    HINSTANCE mInstance; //!< Instance handle
    volatile Signal mSignal; //!< Engine signal
    void adjustPrivileges();
    void fixupThreadAffinity();
  public:
    Engine( HINSTANCE instance );
    ~Engine();
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