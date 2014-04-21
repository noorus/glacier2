#pragma once

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
  protected:
    Scripting* mHost; //!< My host
    v8::Persistent<v8::Script> mScript; //!< My v8 handle
  public:
    //! Constructor.
    Script( Scripting* host );
    //! Compiles the script.
    //! \param  source Source code.
    //! \return true if it succeeds, false if it fails.
    bool compile( const Ogre::String& source );
    //! Executes the script.
    //! \return true if it succeeds, false if it fails.
    bool execute();
    //! Destructor.
    ~Script();
  };

  //! @}

  //! @}

}