#pragma once
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  struct Material {
    HlmsDatablock* datablock_;
    PxMaterial* physmat_;
    Material( HlmsDatablock* datablock, PxMaterial* physmat ): datablock_( datablock ), physmat_( physmat ) {}
  };

  typedef vector<Material> MaterialVector;

  enum Material_Index: size_t {
    Mat_Dev_Ground = 0
  };

  //! \class Materials
  //! Material service class
  class Materials {
  protected:
    CACHE_ALIGN MaterialVector materials_;
    HlmsDatablock* defaultBlock_;
    PxMaterial* defaultPhys_;
  public:
    // Callbacks
    static void callbackReload( Console* console,
      ConCmd* command, StringVector& arguments );
  public:
    Materials();
    ~Materials();
    inline const Material& get( const size_t& index ) const { return materials_[index]; }
    size_t add( const string& datablock, PxMaterial* physmat = nullptr );
    size_t add( HlmsDatablock* datablock, PxMaterial* physmat = nullptr );
    void reload( const size_t index );
  };

  void reloadHlmsResources( Ogre::Root* root, vector<string> resourceNames );

}