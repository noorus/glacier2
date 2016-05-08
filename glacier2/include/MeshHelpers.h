#pragma once
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace MeshHelpers {

  void getMeshInformation( const Ogre::v1::MeshPtr mesh,
    size_t &vertex_count, Ogre::Vector3* &vertices,
    size_t &index_count, unsigned long* &indices,
    const Ogre::Vector3 &position = Ogre::Vector3::ZERO,
    const Ogre::Quaternion &orientation = Ogre::Quaternion::IDENTITY,
    const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE );

  void getMesh2Information( const Ogre::MeshPtr mesh,
    size_t &vertex_count, Ogre::Vector3* &vertices,
    size_t &index_count, Ogre::uint32* &indices,
    const Ogre::Vector3 &position = Ogre::Vector3::ZERO,
    const Ogre::Quaternion &orientation = Ogre::Quaternion::IDENTITY,
    const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE );

  void getManualMeshInformation( const Ogre::v1::ManualObject *manual,
    size_t &vertex_count, Ogre::Vector3* &vertices,
    size_t &index_count, unsigned long* &indices,
    const Ogre::Vector3 &position = Ogre::Vector3::ZERO,
    const Ogre::Quaternion &orientation = Ogre::Quaternion::IDENTITY,
    const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE );

}