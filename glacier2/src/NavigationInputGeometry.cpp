#include "StdAfx.h"
#include "Navigation.h"
#include "GlacierMath.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "MeshHelpers.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

// This code is heavily based on OgreCrowd, Copyright (c) 2012 Jonas Hauquier.
// Additional contributions by mkultra333 & Paul Wilson.
// Made available under the MIT license.

namespace Glacier {

  NavigationInputGeometry::NavigationInputGeometry( const OgreEntityVector& entities ):
  mReferenceNode( nullptr ), mVertices( nullptr ), mVertexCount( 0 ),
  mTriangles( nullptr ), mTriangleCount( 0 ), mNormals( nullptr ),
  mBBoxMin( nullptr ), mBBoxMax( nullptr )
  {
    mBBoxMin = new float[3];
    mBBoxMax = new float[3];

    if ( entities.empty() )
      return;

    mReferenceNode = entities[0]->getParentSceneNode()->getCreator()->getRootSceneNode();

    calculateExtents( entities );
    convertEntities( entities );

    // TODO Chunky triangle mesh support, tiled navmeshes
  }

  NavigationInputGeometry::~NavigationInputGeometry()
  {
    if ( mVertices )
      delete[] mVertices;
    if ( mNormals )
      delete[] mNormals;
    if ( mTriangles )
      delete[] mTriangles;
    if ( mBBoxMin )
      delete[] mBBoxMin;
    if ( mBBoxMax )
      delete[] mBBoxMax;
  }

  void NavigationInputGeometry::calculateExtents( const OgreEntityVector& entities )
  {
    auto entity = entities[0];
    auto bbox = entity->getBoundingBox();

    Matrix4 transform = mReferenceNode->_getFullTransform().inverse() * entity->getParentSceneNode()->_getFullTransform();
    bbox.transform( transform );

    Vector3 bbmin = bbox.getMinimum();
    Vector3 bbmax = bbox.getMaximum();

    for ( auto iterator : entities )
    {
      entity = iterator;
      bbox = entity->getBoundingBox();
      transform = mReferenceNode->_getFullTransform().inverse() * entity->getParentSceneNode()->_getFullTransform();
      bbox.transform( transform );

      Vector3 min2 = bbox.getMinimum();
      if ( min2.x < bbmin.x )
        bbmin.x = min2.x;
      if ( min2.y < bbmin.y )
        bbmin.y = min2.y;
      if ( min2.z < bbmin.z )
        bbmin.z = min2.z;

      Vector3 max2 = bbox.getMaximum();
      if ( max2.x > bbmax.x )
        bbmax.x = max2.x;
      if ( max2.y > bbmax.y )
        bbmax.y = max2.y;
      if ( max2.z > bbmax.z )
        bbmax.z = max2.z;
    }

    Math::ogreVec3ToFloatArray( bbmin, mBBoxMin );
    Math::ogreVec3ToFloatArray( bbmax, mBBoxMax );
  }

  void NavigationInputGeometry::convertEntities( const OgreEntityVector& entities )
  {
    const size_t count = entities.size();

    size_t* meshVertexCount = new size_t[count];
    size_t* meshIndexCount = new size_t[count];

    Vector3** meshVertices = new Vector3*[count];
    unsigned long** meshIndices = new unsigned long*[count];

    mVertexCount = 0;
    mTriangleCount = 0;

    size_t i = 0;
    for ( auto entity : entities )
    {
      MeshHelpers::getMeshInformation( entity->getMesh(),
        meshVertexCount[i], meshVertices[i],
        meshIndexCount[i], meshIndices[i] );

      mVertexCount += meshVertexCount[i];
      mTriangleCount += meshIndexCount[i];
      i++;
    }

    mVertices = new float[mVertexCount * 3];
    mTriangles = new int[mTriangleCount];
    
    mTriangleCount /= 3;

    i = 0;
    int vertexIndex = 0;
    int previousVertexCount = 0;
    int previousIndexCountTotal = 0;
    for ( auto entity : entities )
    {
      Matrix4 transform = mReferenceNode->_getFullTransform().inverse() * entity->getParentSceneNode()->_getFullTransform();
      for ( size_t j = 0; j < meshVertexCount[i]; j++ )
      {
        Vector3 vertexPosition = transform * meshVertices[i][j];
        mVertices[vertexIndex++] = vertexPosition.x;
        mVertices[vertexIndex++] = vertexPosition.y;
        mVertices[vertexIndex++] = vertexPosition.z;
      }
      for ( size_t j = 0; j < meshIndexCount[i]; j++ )
      {
        mTriangles[previousIndexCountTotal + j] = meshIndices[i][j] + previousVertexCount;
      }
      previousIndexCountTotal += meshIndexCount[i];
      previousVertexCount += meshVertexCount[i];
      i++;
    }

    delete[] meshIndices;
    delete[] meshVertices;
    delete[] meshIndexCount;
    delete[] meshVertexCount;

    // TODO Check this normals calculation, probably wrong
    mNormals = new float[mTriangleCount * 3];
    for ( size_t i = 0; i < mTriangleCount * 3; i += 3 )
    {
      const float* v0 = &mVertices[mTriangles[i] * 3];
      const float* v1 = &mVertices[mTriangles[i + 1] * 3];
      const float* v2 = &mVertices[mTriangles[i + 2] * 3];
      float e0[3], e1[3];
      for ( size_t j = 0; j < 3; j++ )
      {
        e0[j] = ( v1[j] - v0[j] );
        e1[j] = ( v2[j] - v0[j] );
      }
      float* n = &mNormals[i];
      n[0] = ( ( e0[1] * e1[2] ) - ( e0[2] * e1[1] ) );
      n[1] = ( ( e0[2] * e1[0] ) - ( e0[0] * e1[2] ) );
      n[2] = ( ( e0[0] * e1[1] ) - ( e0[1] * e1[0] ) );
      float d = sqrtf( n[0] * n[0] + n[1] * n[1] + n[2] * n[2] );
      if ( d > 0.0f )
      {
        d = 1.0f / d;
        n[0] *= d;
        n[1] *= d;
        n[2] *= d;
      }
    }
  }

  AxisAlignedBox NavigationInputGeometry::getBoundingBox()
  {
    AxisAlignedBox bbox;
    bbox.setMinimum( Math::floatArrayToOgreVec3( mBBoxMin ) );
    bbox.setMaximum( Math::floatArrayToOgreVec3( mBBoxMax ) );
    return bbox;
  }

  float* NavigationInputGeometry::getVertices()
  {
    return mVertices;
  }

  int NavigationInputGeometry::getVertexCount()
  {
    return mVertexCount;
  }

  int* NavigationInputGeometry::getTriangles()
  {
    return mTriangles;
  }

  int NavigationInputGeometry::getTriangleCount()
  {
    return mTriangleCount;
  }

  float* NavigationInputGeometry::getNormals()
  {
    return mNormals;
  }

  float* NavigationInputGeometry::getMeshBoundsMin()
  {
    return mBBoxMin;
  }

  float* NavigationInputGeometry::getMeshBoundsMax()
  {
    return mBBoxMax;
  }

  const bool NavigationInputGeometry::isEmpty()
  {
    return ( mVertexCount <= 0 || mTriangleCount <= 0 );
  }

}