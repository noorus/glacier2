#pragma once
#include "Types.h"

namespace Glacier {

  class ConCmdBase;
  class ConCmd;
  class ConVar;
  class Console;

  class ConVarValue {
  public:
    int i;
    float f;
    wstring str;
  };

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

  class ConCmd: public ConCmdBase {
  public:
    typedef void ( *Callback )( StringVector& arguments, Console* console );
  protected:
    Callback mCallback;
  public:
    ConCmd( const wstring& name, const wstring& description, Callback callback );
    virtual bool isCommand();
  };

  class ConVar: public ConCmdBase {
  public:
    typedef bool ( *Callback )( ConVar* variable, ConVarValue oldValue, Console* console );
  protected:
    struct Value {
      int i;
      float f;
      wstring str;
    } mValue;
    Callback mCallback;
  public:
    ConVar( const wstring& name, const wstring& description, Callback callback );
    virtual bool isCommand();
    virtual int getInt();
    virtual float getFloat();
    virtual const wstring& getString();
  };

  class Console {
  public:
    Console();
  };

}