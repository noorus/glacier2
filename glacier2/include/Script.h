#pragma once
#include <v8.h>
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Scripting
  //! @{

  class Scripting;

  //! \class Script
  //! A single JS script instance.
  class Script {
  public:
    enum Status {
      Status_Uncompiled,
      Status_Compiled,
      Status_Executing,
      Status_CompileError,
      Status_RuntimeError
    };
  protected:
    wstring mName;
    Scripting& mHost;
    Status mStatus;
    v8::Persistent<v8::Script> mScript; //!< My v8 handle
    void changeStatus( Status status );
    void reportException( const v8::TryCatch& tryCatch );
  public:
    //! Constructor.
    Script( Scripting& host, const wstring& name );
    virtual const wstring& getName();
    virtual const bool isSimple();
    //! Compiles the script.
    //! \param  source Source code.
    //! \return true if it succeeds, false if it fails.
    virtual bool compile();
    //! Executes the script.
    //! \return true if it succeeds, false if it fails.
    virtual bool execute();
    //! Destructor.
    virtual ~Script();
  };

  typedef std::list<Script*> ScriptList;

  //! @}

  //! @}

}