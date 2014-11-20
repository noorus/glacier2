#pragma once
#include "Types.h"
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Console
  //! @{

  class Console;
  class TextFile;

  //! \def ENGINE_DECLARE_CONCMD(name,desc,cb)
  //! Declares a console command with a callback.
# define ENGINE_DECLARE_CONCMD(name,desc,cb)\
  Glacier::ConCmd g_CVar_##name( L#name, desc, cb )
  //! \def ENGINE_DECLARE_CONVAR(name,desc,defval)
  //! Declares a console variable with a default value.
# define ENGINE_DECLARE_CONVAR(name,desc,defval)\
  Glacier::ConVar g_CVar_##name( L#name, desc, defval )
  //! \def ENGINE_DECLARE_CONVAR_WITH_CB(name,desc,defval,cb)
  //! Declares a console variable with a default value and a callback function.
# define ENGINE_DECLARE_CONVAR_WITH_CB(name,desc,defval,cb)\
  Glacier::ConVar g_CVar_##name( L#name, desc, defval, cb )
  //! \def ENGINE_EXTERN_CONCMD(name)
  //! Externs the declaration of a console command.
# define ENGINE_EXTERN_CONCMD(name)\
  extern Glacier::ConCmd g_CVar_##name
  //! \def ENGINE_EXTERN_CONVAR(name)
  //! Externs the declaration of a console variable.
# define ENGINE_EXTERN_CONVAR(name)\
  extern Glacier::ConVar g_CVar_##name

  //! A console source.
  struct ConsoleSource
  {
    wstring name;
    COLORREF color;
  };

  //! \class ConBase
  //! Base class for console commands & variables.
  //! \sa Console
  class ConBase {
  friend class Console;
  protected:
    wstring mName;        //!< Name of the command/variable
    wstring mDescription; //!< Short description for the command/variable
    bool mRegistered;     //!< Is the command/variable registered
    ConBase( const wstring& name, const wstring& description );
    //! Called when this object is registered in a Console.
    virtual void onRegister();
  public:
    //! Query if this ConBase is a command.
    virtual bool isCommand() = 0;
    //! Gets the name.
    virtual const wstring& getName();
    //! Gets the description.
    virtual const wstring& getDescription();
    //! Query whether this ConBase is registered.
    virtual const bool isRegistered();
  };

  //! A list of console variables and commands.
  typedef std::list<ConBase*> ConBaseList;

  //! \class ConCmd
  //! A console command.
  class ConCmd: public ConBase {
  public:
    typedef void ( *Callback )( Console* console, ConCmd* command, StringVector& arguments );
  protected:
    Callback mCallback; //!< Callback function on execution
  public:
    ConCmd( const wstring& name, const wstring& description, Callback callback );
    virtual void call( Console* console, StringVector& arguments );
    virtual bool isCommand();
  };

  //! \class ConVar
  //! A console variable.
  class ConVar: public ConBase {
  public:
    struct Value {
      int i; //!< Integer representation
      float f; //!< Floating point representation
      wstring str; //!< String representation
    };
    typedef bool ( *Callback )( ConVar* variable, Value oldValue );
  protected:
    Value mValue; //!< Current value
    Value mDefaultValue; //!< Default value
    Callback mCallback; //!< Callback function on value change
  public:
    //! Constructor with an integer value.
    ConVar( const wstring& name, const wstring& description,
      int defaultValue, Callback callback = nullptr );
    //! Constructor with a float value.
    ConVar( const wstring& name, const wstring& description,
      float defaultValue, Callback callback = nullptr );
    //! Constructor with a string value.
    ConVar( const wstring& name, const wstring& description,
      const wstring& defaultValue, Callback callback = nullptr );
    virtual bool isCommand();
    //! Gets the value as integer.
    virtual int getInt();
    //! Gets the value as float.
    virtual float getFloat();
    //! Gets the value as string.
    virtual const wstring& getString();
    //! Gets the value as boolean.
    virtual bool getBool() { return ( getInt() > 0 ); }
    //! Sets the value as integer.
    virtual void setValue( int value );
    //! Sets the value as float.
    virtual void setValue( float value );
    //! Sets the value as string.
    virtual void setValue( const wstring& value );
    //! Force-sets the value as integer.
    virtual void forceValue( int value );
    //! Force-sets the value as float.
    virtual void forceValue( float value );
    //! Force-sets the value as string.
    virtual void forceValue( const wstring& value );
  };

  //! A console listener's virtual base class.
  class ConsoleListener {
  public:
    //! Called when a line is added to the console.
    virtual void onAddLine( COLORREF color, const wstring& line ) = 0;
  };

  //! A list of console listeners.
  typedef std::list<ConsoleListener*> ConsoleListenerList;

  //! \class Console
  //! Allows output of messages and execution of commands and variables.
  //! \sa EngineComponent
  class Console: public EngineComponent {
  friend class ConBase;
  public:
    //! Message source type.
    enum Source: unsigned long {
      srcEngine = 0,  //!< Message from the engine
      srcGfx,         //!< Message from the graphics subsystem
      srcSound,       //!< Message from the sound subsystem
      srcPhysics,     //!< Message from the physics subsystem
      srcScripting,   //!< Message from the scripting subsystem
      srcInput,       //!< Message from the input subsystem
      srcGame,        //!< Message from the game logic
      srcGUI          //!< Message from the gui subsystem
    };
  protected:
    SRWLOCK mLock; //!< Execution lock
    SRWLOCK mBufferLock; //!< Command buffer lock
    ConBaseList mCommands; //!< Registered commands & variables
    static ConBaseList mPrecreated; //!< Pre-created commands & variables
    StringList mLines; //!< Line buffer
    StringQueue mCommandBuffer; //!< Command buffer for next execution
    std::map<Source,ConsoleSource> mSources;
    TextFile* mOutFile; //!< Output log file
    ConsoleListenerList mListeners; //!< Console event listeners
    ConCmd* mCmdList; //!< Internal "list" command
    ConCmd* mCmdFind; //!< Internal "find" command
    ConCmd* mCmdExec; //!< Internal "exec" command
    ConCmd* mCmdHelp; //!< Internal "help" command
    static bool cmpSortCmds( ConBase* a, ConBase* b );
    static StringVector tokenize( const wstring& commandLine );
  protected:
    // Core command callbacks
    static void callbackList( Console* console, ConCmd* command, StringVector& arguments );
    static void callbackFind( Console* console, ConCmd* command, StringVector& arguments );
    static void callbackExec( Console* console, ConCmd* command, StringVector& arguments );
    static void callbackHelp( Console* console, ConCmd* command, StringVector& arguments );
  public:
    //! Constructor.
    Console( Engine* engine );
    //! \copydoc EngineComponent::componentPreUpdate()
    virtual void componentPreUpdate( GameTime time );
    //! Destructor.
    virtual ~Console();
    ConVar* getVariable( const wstring& name );
    ConCmd* getCommand( const wstring& name );
    //! Registers a message source.
    Source registerSource( const wstring& name, COLORREF color );
    //! Processes commands currently in the command buffer.
    void processBuffered();
    //! Adds a listener.
    void addListener( ConsoleListener* listener );
    //! Removes a listener.
    void removeListener( ConsoleListener* listener );
    //! Unregisters a message source.
    void unregisterSource( Source source );
    //! Registers a console variable or command.
    void registerVariable( ConBase* var );
    //! Prints a message.
    void printf( Source source, const wchar_t* line, ... );
    //! Prints an error message.
    void errorPrintf( Source source, const wchar_t* line, ... );
    //! Automatic completion search for given command line.
    void autoComplete( const wstring& line, ConBaseList& matches );
    //! Executes a command line.
    void execute( wstring commandLine, const bool echo = true );
    //! Queues a command for execution on next update call.
    void executeBuffered( const wstring& commandLine );
    //! Executes a file.
    void executeFile( const wstring& filename );
    //! Describes the given console command or variable.
    void describe( ConBase* base );
  };

  //! @}

  //! @}

}