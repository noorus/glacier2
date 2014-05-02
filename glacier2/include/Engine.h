#pragma once
#include "Types.h"
#include "Console.h"

// Glacier� Game Engine � 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Engine
  //! @{

  class Console;
  class ConsoleWindow;
  class ConsoleWindowThread;
  class Scripting;
  class Graphics;
  class ConCmd;
  class Game;
  class FMODAudio;
  class Physics;
  class WindowHandler;
  class Input;

  ENGINE_EXTERN_CONCMD( version );
  ENGINE_EXTERN_CONCMD( screenshot );
  ENGINE_EXTERN_CONCMD( quit );

  //! \class Engine
  //! The main engine class that makes the world go round
  class Engine {
  public:
    //! Possible signal values interpreted by the engine's gameloop
    enum Signal {
      Signal_None = 0,  //!< No signal
      Signal_Stop       //!< Engine stop signal
    };
    //! Engine version structure
    struct Version {
      uint32_t major;   //!< The major version
      uint32_t minor;   //!< The minor version
      uint32_t build;   //!< The build version
      wstring compiler; //!< Compiler name
      wstring compiled; //!< Compilation date
      wstring profile;  //!< Compilation profile
      wstring title;
      wstring subtitle;
      Version( uint32_t major, uint32_t minor, uint32_t build );
    };
    //! Engine options structure
    struct Options {
      bool noAudio;
      Options();
    };
  protected:
    Version mVersion;
    // Subsystems
    Console* mConsole;
    ConsoleWindowThread* mConsoleWindow;
    Scripting* mScripting;
    Graphics* mGraphics;
    Physics* mPhysics;
    FMODAudio* mAudio;
    Game* mGame;
    Input* mInput;
    WindowHandler* mWindowHandler;
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
    FMODAudio* getSound() { return mAudio; }
    Game* getGame() { return mGame; }
    Input* getInput() { return mInput; }
    inline GameTime getTime() { return fTime; }
    // Callbacks
    static void callbackVersion( Console* console,
      ConCmd* command, StringVector& arguments );
    static void callbackScreenshot( Console* console,
      ConCmd* command, StringVector& arguments );
    static void callbackQuit( Console* console,
      ConCmd* command, StringVector& arguments );
  public:
    //! Constructor.
    //! \param  instance The owning application's instance handle.
    Engine( HINSTANCE instance );
    //! Destructor.
    ~Engine();
    //! Raises a stop signal on the next cycle.
    void signalStop();
    //! Shutdown state for gfx restart.
    void operationSuspendVideo();
    //! Continue state after gfx restart.
    void operationContinueVideo();
    //! Shutdown state for audio restart.
    void operationSuspendAudio();
    //! Continue state after audio restart.
    void operationContinueAudio();
    //! Shutdown state for physics restart.
    void operationSuspendPhysics();
    //! Continue state after physics restart.
    void operationContinuePhysics();
    //! Registers the resources described by manager.
    //! \param [in,out] manager The resource group manager.
    void registerResources( ResourceGroupManager& manager );
    //! Unregisters the resources described by manager.
    //! \param [in,out] manager The resource group manager.
    void unregisterResources( ResourceGroupManager& manager );
    //! Initializes the Engine.
    void initialize( const Options& options );
    //! Runs the Engine.
    void run();
    //! Shuts down the Engine and frees any resources it is using.
    void shutdown();
  };

  //! @}

  //! @}

}

extern Glacier::Engine* gEngine; //!< Global engine instance pointer