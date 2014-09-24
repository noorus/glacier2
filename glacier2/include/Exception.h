#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  struct WinAPIError {
    uint32_t code;
    wstring description;
  };

  struct FMODError {
    FMOD_RESULT code;
    wstring description;
  };

  class Exception: public std::exception {
  public:
    enum Type {
      Generic = 0,
      WinAPI,
      NT,
      FMOD
    };
  private:
    Exception(): mType( Type::Generic ) {}
  protected:
    const Type mType;
    wstring mDescription;
    wstring mSource;
    mutable wstring mFullDescription;
    boost::variant<WinAPIError,FMODError> mAdditional;
    void handleAdditional();
  public:
    Exception( const wstring& description, const Type type = Type::Generic );
    Exception( const wstring& description, const wstring& source, const Type type = Type::Generic );
    Exception( const wstring& description, const wstring& source, NTSTATUS ntstatus, const Type type = Type::Generic );
    Exception( const wstring& description, const wstring& source, FMOD_RESULT result, const Type type = Type::Generic );
    virtual const std::wstring& getFullDescription() const;
    virtual const char* what() const throw();
  };

#if defined(ENGINE_EXCEPT) || defined(ENGINE_EXCEPT_WINAPI) || defined(ENGINE_EXCEPT_NT)
# error ENGINE_EXCEPT* macro already defined!
#else
# define ENGINE_EXCEPT(description) {throw Glacier::Exception(description,__FUNCTIONW__,Glacier::Exception::Generic);}
# define ENGINE_EXCEPT_WINAPI(description) {throw Glacier::Exception(description,__FUNCTIONW__,Glacier::Exception::WinAPI);}
# define ENGINE_EXCEPT_NT(r,description) {throw Glacier::Exception(description,__FUNCTIONW__,r,Glacier::Exception::NT);}
# define ENGINE_EXCEPT_FMOD(r,description) {throw Glacier::Exception(description,__FUNCTIONW__,r,Glacier::Exception::FMOD);}
// ENGINE_EXCEPT_SEH
#endif

}