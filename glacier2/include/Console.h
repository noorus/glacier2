#pragma once
#include "Types.h"

namespace Glacier {

  class Console;

  //! \class ConCmdBase
  //! Base class for console commands & variables.
  class ConCmdBase {
  protected:
    wstring mName; //!< Name of the command/variable
    wstring mDescription; //! Short description for the command/variable
    ConCmdBase( const wstring& name, const wstring& description );
  public:
    virtual bool isCommand() = 0;
    virtual const wstring& getName();
    virtual const wstring& getDescription();
  };

  typedef std::list<ConCmdBase*> ConCmdBaseList;

  //! \class ConCmd
  //! A console command.
  class ConCmd: public ConCmdBase {
  public:
    typedef void ( *Callback )( StringVector& arguments, Console* console );
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
      float f; //!< Floating point representation
      wstring str; //!< String representation
    };
    typedef bool ( *Callback )( ConVar* variable, Value oldValue, Console* console );
  protected:
    Value mValue; //!< Value
    Value mDefaultValue; //!< Default value
    Callback mCallback; //!< Callback function on value change
  public:
    ConVar( const wstring& name, const wstring& description, Callback callback );
    virtual bool isCommand();
    virtual int getInt();
    virtual float getFloat();
    virtual const wstring& getString();
  };

  //! \class Console
  class Console {
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
    ConCmdBaseList mCommands; //!< Registered commands & variables
    static ConCmdBaseList mPrecreated; //!< Pre-created commands & variables
    StringList mLines; //!< Line buffer
    StringQueue mCommandBuffer; //!< Command buffer for next execution
  public:
    Console();
  };

}