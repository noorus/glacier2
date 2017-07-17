#pragma once
#include "Types.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
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
  class PhysXPhysics;
  class WindowHandler;
  class InputManager;
  class EntityManager;
  class World;
  class Navigation;
  class HUD;
  class Environment;

  ENGINE_EXTERN_CONCMD( version );
  ENGINE_EXTERN_CONCMD( memstat );
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
    //! Possible fatal errors
    enum FatalError {
      Fatal_Generic, //!< Non-specific fatal error case
      Fatal_MemoryAllocation //!< Memory allocation failure
    };
    //! Engine version structure
    struct Version {
      uint32_t major;   //!< The major version
      uint32_t minor;   //!< The minor version
      uint32_t build;   //!< The build version
      wstring compiler; //!< Compiler name
      wstring compiled; //!< Compilation date
      wstring profile;  //!< Compilation profile
      wstring title;    //!< Engine build title
      wstring subtitle; //!< Engine build subtitle
      Version( uint32_t major, uint32_t minor, uint32_t build );
    };
    //! Engine startup options
    struct Options {
      bool noAudio; //!< Run with a null audio service
      StringList additionalExecs; //!< List of additional configuration files to execute
      Options();
    };
  protected:
    Version mVersion;
    // Subsystems
    Console* mConsole; //!< Console subsystem
    ConsoleWindowThread* mConsoleWindow; //!< Console window - TODO move elsewhere
    Scripting* mScripting; //!< Scripting subsystem (JS/v8)
    Graphics* mGraphics; //!< Graphics subsystem (OGRE)
    PhysXPhysics* mPhysics; //!< Physics subsystem (PhysX)
    FMODAudio* mAudio; //!< Audio subsystem (FMOD)
    Game* mGame; //!< Game subsystem
    InputManager* mInput; //!< Input subsystem (NIL)
    WindowHandler* mWindowHandler; //!< Window events handler - TODO move elsewhere
    EntityManager* mEntities; //!< Entity manager subsystem
    World* mWorld;
    Navigation* mNavigation; //!< Navigation subsystem (Recast/Detour)
    HUD* mHUD; //!< HUD subsystem
    Environment* mEnvironment; //!< Environment subsystem
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
    void fixupThreadAffinity();
  public:
    // Getters
    //! \copydoc Engine::Version
    const Version& getVersion() { return mVersion; }
    //! \copydoc Engine::mConsole
    Console* getConsole() { return mConsole; }
    //! \copydoc Engine::mScripting
    Scripting* getScripting() { return mScripting; }
    //! \copydoc Engine::mGraphics
    Graphics* getGraphics() { return mGraphics; }
    //! \copydoc Engine::mPhysics
    PhysXPhysics* getPhysics() { return mPhysics; }
    //! \copydoc Engine::mAudio
    FMODAudio* getSound() { return mAudio; }
    //! \copydoc Engine::mGame
    Game* getGame() { return mGame; }
    //! \copydoc Engine::mInput
    InputManager* getInput() { return mInput; }
    //! \copydoc Engine::mEntities
    EntityManager* getEntities() { return mEntities; }
    //! \copydoc Engine::mWorld
    World* getWorld() { return mWorld; }
    //! \copydoc Engine::mNavigation
    Navigation* getNavigation() { return mNavigation; }
    //! \copydoc Engine::mEnvironment
    Environment* getEnvironment() { return mEnvironment; }
    inline GameTime getTime() { return fTime; }
    // Callbacks
    static void callbackVersion( Console* console,
      ConCmd* command, StringVector& arguments );
    static void callbackMemstat( Console* console,
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
    //! Triggers a "graceful" quit in case of a fatal error.
    void triggerFatalError( FatalError error );
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
    void registerUserLocations( ResourceGroupManager& manager );
    void unregisterUserLocations( ResourceGroupManager& manager );
    void registerResources( ResourceGroupManager& manager );
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