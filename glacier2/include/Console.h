#pragma once
#include "Types.h"
#include "EngineComponent.h"

namespace Glacier {

  class Console;
  class TextFile;

# define ENGINE_DECLARE_CONCMD(name,desc,cb)\
  Glacier::ConCmd g_CVar_##name( L#name, desc, cb )
# define ENGINE_DECLARE_CONVAR(name,desc,defval)\
  Glacier::ConVar g_CVar_##name( L#name, desc, defval )
# define ENGINE_DECLARE_CONVAR_WITH_CB(name,desc,defval,cb)\
  Glacier::ConVar g_CVar_##name( L#name, desc, defval, cb )
# define ENGINE_EXTERN_CONCMD(name)\
  extern Glacier::ConCmd g_CVar_##name
# define ENGINE_EXTERN_CONVAR(name)\
  extern Glacier::ConVar g_CVar_##name

  struct ConsoleSource
  {
    wstring name;
    COLORREF color;
  };

  //! \class ConBase
  //! Base class for console commands & variables.
  class ConBase {
  friend class Console;
  protected:
    wstring mName; //!< Name of the command/variable
    wstring mDescription; //!< Short description for the command/variable
    bool mRegistered; //!< Is the command/variable registered
    ConBase( const wstring& name, const wstring& description );
    virtual void onRegister();
  public:
    virtual bool isCommand() = 0;
    virtual const wstring& getName();
    virtual const wstring& getDescription();
    virtual const bool isRegistered();
  };

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
    Value mValue; //!< Value
    Value mDefaultValue; //!< Default value
    Callback mCallback; //!< Callback function on value change
  public:
    ConVar( const wstring& name, const wstring& description,
      int defaultValue, Callback callback = nullptr );
    ConVar( const wstring& name, const wstring& description,
      float defaultValue, Callback callback = nullptr );
    ConVar( const wstring& name, const wstring& description,
      const wstring& defaultValue, Callback callback = nullptr );
    virtual bool isCommand();
    virtual int getInt();
    virtual float getFloat();
    virtual const wstring& getString();
    virtual bool getBool() { return ( getInt() > 0 ); }
    virtual void setValue( int value );
    virtual void setValue( float value );
    virtual void setValue( const wstring& value );
    virtual void forceValue( int value );
    virtual void forceValue( float value );
    virtual void forceValue( const wstring& value );
  };

  class ConsoleListener {
  public:
    virtual void onAddLine( COLORREF color, const wstring& line ) = 0;
  };

  typedef std::list<ConsoleListener*> ConsoleListenerList;

  //! \class Console
  class Console: public EngineComponent {
  friend class ConBase;
  public:
    enum Source: unsigned long {
      srcEngine = 0,
      srcGfx,
      srcSound,
      srcPhysics,
      srcScripting,
      srcInput,
      srcGame
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
    ConCmd* mCmdList; //!< "list" command
    ConCmd* mCmdFind; //!< "find" command
    ConCmd* mCmdExec; //!< "exec" command
    ConCmd* mCmdHelp; //!< "help" command
    static bool cmpSortCmds( ConBase* a, ConBase* b );
    static StringVector tokenize( const wstring& commandLine );
  protected:
    // Core command callbacks
    static void callbackList( Console* console, ConCmd* command, StringVector& arguments );
    static void callbackFind( Console* console, ConCmd* command, StringVector& arguments );
    static void callbackExec( Console* console, ConCmd* command, StringVector& arguments );
    static void callbackHelp( Console* console, ConCmd* command, StringVector& arguments );
  public:
    Console( Engine* engine );
    virtual void componentPreUpdate( GameTime time );
    virtual ~Console();
    Source registerSource( const wstring& name, COLORREF color );
    void processBuffered();
    void addListener( ConsoleListener* listener );
    void removeListener( ConsoleListener* listener );
    void unregisterSource( Source source );
    void registerVariable( ConBase* var );
    void printf( Source source, const wchar_t* line, ... );
    void errorPrintf( const wchar_t* line, ... );
    void autoComplete( const wstring& line, ConBaseList& matches );
    void execute( wstring commandLine );
    void executeBuffered( const wstring& commandLine );
    void executeFile( const wstring& filename );
    void describe( ConBase* base );
  };

}