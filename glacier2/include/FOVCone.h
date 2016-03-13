#pragma once
#include "Types.h"

// Glacier� Game Engine � 2014 noorus
// All rights reserved.

namespace Glacier {

  struct FOVCone {
    Ogre::MeshPtr mesh;
    Ogre::Item* item;
    Ogre::SceneNode* node;
    bool alert;
    FOVCone();
    ~FOVCone();
    void set( Real viewDistance, const Radian& fieldOfView );
    void setAlert( const bool alert );
  };

}