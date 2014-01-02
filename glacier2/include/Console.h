#pragma once
#include "Types.h"

namespace Glacier {

  class Console;

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

  //! \class ConCmdBase
  //! Base class for console commands & variables.
  class ConCmdBase {
  friend class Console;
  protected:
    wstring mName; //!< Name of the command/variable
    wstring mDescription; //!< Short description for the command/variable
    bool mRegistered; //!< Is the command/variable registered
    ConCmdBase( const wstring& name, const wstring& description );
    virtual void onRegister();
  public:
    virtual bool isCommand() = 0;
    virtual const wstring& getName();
    virtual const wstring& getDescription();
    virtual const bool isRegistered();
  };

  typedef std::list<ConCmdBase*> ConCmdBaseList;

  //! \class ConCmd
  //! A console command.
  class ConCmd: public ConCmdBase {
  public:
    typedef void ( *Callback )( ConCmd* command, StringVector& arguments );
  protected:
    Callback mCallback; //!< Callback function on execution
  public:
    ConCmd( const wstring& name, const wstring& description, Callback callback );
    virtual bool isCommand();
  };

  //! \class ConVar
  //! A console variable.
  class ConVar: public ConCmdBase {
  public:
    struct Value {
      int i; //!< Integer representation
      double f; //!< Floating point representation
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
      double defaultValue, Callback callback = nullptr );
    ConVar( const wstring& name, const wstring& description,
      const wstring& defaultValue, Callback callback = nullptr );
    virtual bool isCommand();
    virtual int getInt();
    virtual double getFloat();
    virtual const wstring& getString();
    virtual void setValue( int value );
    virtual void setValue( double value );
    virtual void setValue( const wstring& value );
  };

  //! \class Console
  class Console {
  friend class ConCmdBase;
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
    SRWLOCK mLock; //!< Our execution lock
    ConCmdBaseList mCommands; //!< Registered commands & variables
    static ConCmdBaseList mPrecreated; //!< Pre-created commands & variables
    StringList mLines; //!< Line buffer
    StringQueue mCommandBuffer; //!< Command buffer for next execution
    std::map<Source,ConsoleSource> mSources;
    static bool cmpSortCmds( ConCmdBase* x, ConCmdBase* y );
  public:
    Console();
    ~Console();
    Source registerSource( const wstring& name, COLORREF color );
    void unregisterSource( Source source );
    void registerVariable( ConCmdBase* var );
  };

}