#pragma once
#include "Types.h"

namespace MeshHelpers {

  void getMeshInformation( const Ogre::MeshPtr mesh,
    size_t &vertex_count, Ogre::Vector3* &vertices,
    size_t &index_count, unsigned long* &indices,
    const Ogre::Vector3 &position = Ogre::Vector3::ZERO,
    const Ogre::Quaternion &orientation = Ogre::Quaternion::IDENTITY,
    const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE );

  void getManualMeshInformation( const Ogre::ManualObject *manual,
    size_t &vertex_count, Ogre::Vector3* &vertices,
    size_t &index_count, unsigned long* &indices,
    const Ogre::Vector3 &position = Ogre::Vector3::ZERO,
    const Ogre::Quaternion &orientation = Ogre::Quaternion::IDENTITY,
    const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE );

}