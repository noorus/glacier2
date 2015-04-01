#include "StdAfx.h"
#include "Navigation.h"
#include "GlacierMath.h"
#include "Exception.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  uint32_t NavigationMesh::headerChunkID =
    StreamSerialiser::makeIdentifier( "NAVM" );

  void NavigationMeshParameters::updateDerived()
  {
    derived.walkableHeight = (int)ceilf( agentHeight / cellHeight );
    derived.walkableClimb = (int)floorf( agentMaxClimb / cellHeight );
    derived.walkableRadius = (int)ceilf( agentRadius / cellSize );
    derived.maxEdgeLength = (int)( edgeMaxLength / cellSize );
    derived.minRegionArea = (int)rcSqr( regionMinSize );
    derived.mergeRegionArea = (int)rcSqr( regionMergeSize );
    derived.detailSampleDist = ( detailSampleDist < 0.9f ? 0 : cellSize * detailSampleDist );
    derived.detailSampleMaxError = ( cellHeight * detailSampleMaxError );
  }

  const NavigationMeshParameters::Derived& NavigationMeshParameters::getDerived()
  {
    return derived;
  }

  NavigationMesh::NavigationMesh( NavigationMeshParameters& parameters ):
  mContext( nullptr ), mSolid( nullptr ), mCompact( nullptr ),
  mContours( nullptr ), mPolyMesh( nullptr ), mPolyMeshDetail( nullptr )
  {
    setParameters( parameters );

    mContext = new rcContext( true );
  }

  void NavigationMesh::saveTo( StreamSerialiser& serializer )
  {
    serializer.writeChunkBegin( headerChunkID, 1 );

    // Write configuration
    uint32_t size = sizeof( rcConfig );
    serializer.writeData( &size, 4, 1 );
    serializer.writeData( &mConfig, size, 1 );

    // Write polymesh
    if ( !mPolyMesh ) {
      size = 0;
      serializer.writeData( &size, 4, 1 );
    } else {
      size = sizeof( rcPolyMesh );
      serializer.writeData( &size, 4, 1 );
      serializer.writeData( mPolyMesh, size, 1 );
      serializer.writeData( mPolyMesh->verts, sizeof( unsigned short ) * 3, mPolyMesh->nverts );
      serializer.writeData( mPolyMesh->polys, sizeof( unsigned short ) * 2 * mPolyMesh->nvp, mPolyMesh->npolys );
      serializer.writeData( mPolyMesh->regs, sizeof( unsigned short ), mPolyMesh->npolys );
      serializer.writeData( mPolyMesh->flags, sizeof( unsigned short ), mPolyMesh->npolys );
      serializer.writeData( mPolyMesh->areas, sizeof( unsigned char ), mPolyMesh->npolys );
    }

    // Write detail mesh
    if ( !mPolyMeshDetail ) {
      size = 0;
      serializer.writeData( &size, 4, 1 );
    } else {
      size = sizeof( rcPolyMeshDetail );
      serializer.writeData( &size, 4, 1 );
      serializer.writeData( mPolyMeshDetail, size, 1 );
      serializer.writeData( mPolyMeshDetail->meshes, sizeof( unsigned int ) * 4, mPolyMeshDetail->nmeshes );
      serializer.writeData( mPolyMeshDetail->verts, sizeof( float ) * 3, mPolyMeshDetail->nverts );
      serializer.writeData( mPolyMeshDetail->tris, sizeof( unsigned char ) * 4, mPolyMeshDetail->ntris );
    }

    serializer.writeChunkEnd( headerChunkID );
  }

  void NavigationMesh::saveTo( const UTFString& filename )
  {
    DataStreamPtr stream = Ogre::Root::getSingleton().createFileStream(
      filename, "User", true );
    StreamSerialiser serializer( stream,
      StreamSerialiser::ENDIAN_LITTLE, true );

    saveTo( serializer );

    stream->close();
  }

  void NavigationMesh::loadFrom( StreamSerialiser& serializer )
  {
    uint32_t size;
    serializer.readChunkBegin( headerChunkID, 1 );

    // Read configuration
    serializer.readData( &size, 4, 1 );
    if ( size != sizeof( rcConfig ) )
      ENGINE_EXCEPT( "Bad size for rcConfig blob" );
    serializer.readData( &mConfig, size, 1 );

    // Allocate fresh & read polymesh
    serializer.readData( &size, 4, 1 );
    if ( size == 0 ) {
      if ( mPolyMesh )
        rcFreePolyMesh( mPolyMesh );
      mPolyMesh = nullptr;
    } else {
      newPolyMesh();
      if ( size != sizeof( rcPolyMesh ) )
        ENGINE_EXCEPT( "Bad size for rcPolyMesh blob" );
      serializer.readData( mPolyMesh, size, 1 );
      size = ( sizeof( unsigned short ) * 3 );
      mPolyMesh->verts = (unsigned short*)rcAlloc( size * mPolyMesh->nverts, RC_ALLOC_PERM );
      serializer.readData( mPolyMesh->verts, size, mPolyMesh->nverts );
      size = ( sizeof( unsigned short ) * 2 * mPolyMesh->nvp );
      mPolyMesh->polys = (unsigned short*)rcAlloc( size * mPolyMesh->npolys, RC_ALLOC_PERM );
      serializer.readData( mPolyMesh->polys, size, mPolyMesh->npolys );
      size = sizeof( unsigned short );
      mPolyMesh->regs = (unsigned short*)rcAlloc( size * mPolyMesh->npolys, RC_ALLOC_PERM );
      serializer.readData( mPolyMesh->regs, size, mPolyMesh->npolys );
      size = sizeof( unsigned short );
      mPolyMesh->flags = (unsigned short*)rcAlloc( size * mPolyMesh->npolys, RC_ALLOC_PERM );
      serializer.readData( mPolyMesh->flags, size, mPolyMesh->npolys );
      size = sizeof( unsigned char );
      mPolyMesh->areas = (unsigned char*)rcAlloc( size * mPolyMesh->npolys, RC_ALLOC_PERM );
      serializer.readData( mPolyMesh->areas, size, mPolyMesh->npolys );
    }

    // Allocate fresh & read detail polymesh
    serializer.readData( &size, 4, 1 );
    if ( size == 0 ) {
      if ( mPolyMeshDetail )
        rcFreePolyMeshDetail( mPolyMeshDetail );
      mPolyMeshDetail = nullptr;
    } else {
      newPolyMeshDetail();
      if ( size != sizeof( rcPolyMeshDetail ) )
        ENGINE_EXCEPT( "Bad size for rcPolyMeshDetail blob" );
      serializer.readData( mPolyMeshDetail, size, 1 );
      size = ( sizeof( unsigned int ) * 4 );
      mPolyMeshDetail->meshes = (unsigned int*)rcAlloc( size * mPolyMeshDetail->nmeshes, RC_ALLOC_PERM );
      serializer.readData( mPolyMeshDetail->meshes, size, mPolyMeshDetail->nmeshes );
      size = ( sizeof( float ) * 3 );
      mPolyMeshDetail->verts = (float*)rcAlloc( size * mPolyMeshDetail->nverts, RC_ALLOC_PERM );
      serializer.readData( mPolyMeshDetail->verts, size, mPolyMeshDetail->nverts );
      size = ( sizeof( unsigned char ) * 4 );
      mPolyMeshDetail->tris = (unsigned char*)rcAlloc( size * mPolyMeshDetail->ntris, RC_ALLOC_PERM );
      serializer.readData( mPolyMeshDetail->tris, size, mPolyMeshDetail->ntris );
    }

    serializer.readChunkEnd( headerChunkID );
  }

  void NavigationMesh::loadFrom( const UTFString& filename )
  {
    DataStreamPtr stream = Ogre::Root::getSingleton().openFileStream(
      filename, "User" );
    StreamSerialiser serializer( stream,
      StreamSerialiser::ENDIAN_LITTLE, true );

    loadFrom( serializer );

    stream->close();
  }

  void NavigationMesh::setParameters( NavigationMeshParameters& parameters )
  {
    parameters.updateDerived();

    memset( &mConfig, 0, sizeof( rcConfig ) );
    mConfig.cs = parameters.cellSize;
    mConfig.ch = parameters.cellHeight;
    mConfig.walkableSlopeAngle = parameters.agentMaxSlope;
    mConfig.walkableHeight = parameters.getDerived().walkableHeight;
    mConfig.walkableClimb = parameters.getDerived().walkableClimb;
    mConfig.walkableRadius = parameters.getDerived().walkableRadius;
    mConfig.maxEdgeLen = parameters.getDerived().maxEdgeLength;
    mConfig.maxSimplificationError = parameters.edgeMaxError;
    mConfig.minRegionArea = parameters.getDerived().minRegionArea;
    mConfig.mergeRegionArea = parameters.getDerived().mergeRegionArea;
    mConfig.maxVertsPerPoly = parameters.vertsPerPoly;
    mConfig.detailSampleDist = (float)parameters.getDerived().detailSampleDist;
    mConfig.detailSampleMaxError = parameters.getDerived().detailSampleMaxError;
  }

  void NavigationMesh::buildFrom( NavigationInputGeometry* geometry )
  {
    mContext->resetTimers();
    mContext->startTimer( RC_TIMER_TOTAL );

    rcVcopy( mConfig.bmin, geometry->getMeshBoundsMin() );
    rcVcopy( mConfig.bmax, geometry->getMeshBoundsMax() );
    rcCalcGridSize( mConfig.bmin, mConfig.bmax, mConfig.cs, &mConfig.width, &mConfig.height );

    int nverts = geometry->getVertexCount();
    int ntris = geometry->getTriangleCount();

    Vector3 vmin = Math::floatArrayToOgreVec3( geometry->getMeshBoundsMin() );
    Vector3 vmax = Math::floatArrayToOgreVec3( geometry->getMeshBoundsMax() );

    mSolid = rcAllocHeightfield();
    if ( !mSolid )
      ENGINE_EXCEPT( "Failed to allocate heightfield" );

    if ( !rcCreateHeightfield( mContext, *mSolid,
      mConfig.width, mConfig.height,
      mConfig.bmin, mConfig.bmax,
      mConfig.cs, mConfig.ch ) )
      ENGINE_EXCEPT( "Failed to create heightfield" );
    
    auto triareas = (unsigned char*)Locator::getMemory().alloc( Memory::Sector_Navigation, ntris );
    memset( triareas, 0, ntris );

    rcMarkWalkableTriangles( mContext, mConfig.walkableSlopeAngle,
      geometry->getVertices(), geometry->getVertexCount(),
      geometry->getTriangles(), geometry->getTriangleCount(),
      triareas );

    rcRasterizeTriangles( mContext,
      geometry->getVertices(), geometry->getVertexCount(),
      geometry->getTriangles(), triareas, geometry->getTriangleCount(),
      *mSolid, mConfig.walkableClimb );

    Locator::getMemory().free( Memory::Sector_Navigation, triareas );

    rcFilterLowHangingWalkableObstacles( mContext, mConfig.walkableClimb, *mSolid );
    rcFilterLedgeSpans( mContext, mConfig.walkableHeight, mConfig.walkableClimb, *mSolid );
    rcFilterWalkableLowHeightSpans( mContext, mConfig.walkableHeight, *mSolid );

    mCompact = rcAllocCompactHeightfield();
    if ( !mCompact )
      ENGINE_EXCEPT( "Failed to allocate compact heightfield" );

    if ( !rcBuildCompactHeightfield( mContext,
      mConfig.walkableHeight, mConfig.walkableClimb, *mSolid, *mCompact ) )
      ENGINE_EXCEPT( "Failed to build compact heightfield" );

    rcFreeHeightField( mSolid );
    mSolid = nullptr;

    if ( !rcErodeWalkableArea( mContext, mConfig.walkableRadius, *mCompact ) )
      ENGINE_EXCEPT( "Failed to erode walkable area in heightfield" );

    if ( !rcBuildDistanceField( mContext, *mCompact ) )
      ENGINE_EXCEPT( "Failed to build distance field" );

    if ( !rcBuildRegions( mContext, *mCompact, mConfig.borderSize,
      mConfig.minRegionArea, mConfig.mergeRegionArea ) )
      ENGINE_EXCEPT( "Failed to build navigation regions" );

    mContours = rcAllocContourSet();
    if ( !mContours )
      ENGINE_EXCEPT( "Failed to allocate contour set" );

    if ( !rcBuildContours( mContext, *mCompact, mConfig.maxSimplificationError, mConfig.maxEdgeLen, *mContours ) )
      ENGINE_EXCEPT( "Failed to build contour set" );

    if ( mContours->nconts == 0 )
      ENGINE_EXCEPT( "Empty contour set" );

    newPolyMesh();

    if ( !rcBuildPolyMesh( mContext, *mContours, mConfig.maxVertsPerPoly, *mPolyMesh ) )
      ENGINE_EXCEPT( "Failed to triangulate contours" );

    newPolyMeshDetail();

    if ( !rcBuildPolyMeshDetail( mContext, *mPolyMesh, *mCompact,
      mConfig.detailSampleDist, mConfig.detailSampleMaxError, *mPolyMeshDetail ) )
      ENGINE_EXCEPT( "Failed to build detail poly mesh" );

    rcFreeCompactHeightfield( mCompact );
    mCompact = nullptr;

    rcFreeContourSet( mContours );
    mContours = nullptr;

    mContext->stopTimer( RC_TIMER_TOTAL );
  }

  void NavigationMesh::newPolyMesh()
  {
    if ( mPolyMesh )
      rcFreePolyMesh( mPolyMesh );
    mPolyMesh = rcAllocPolyMesh();
    if ( !mPolyMesh )
      ENGINE_EXCEPT( "Failed to allocate poly mesh" );
  }

  void NavigationMesh::newPolyMeshDetail()
  {
    if ( mPolyMeshDetail )
      rcFreePolyMeshDetail( mPolyMeshDetail );
    mPolyMeshDetail = rcAllocPolyMeshDetail();
    if ( !mPolyMeshDetail )
      ENGINE_EXCEPT( "Failed to allocate detail poly mesh" );
  }

  const rcPolyMesh* NavigationMesh::getPolyMesh()
  {
    return mPolyMesh;
  }

  const rcPolyMeshDetail* NavigationMesh::getPolyMeshDetail()
  {
    return mPolyMeshDetail;
  }

  NavigationMesh::~NavigationMesh()
  {
    if ( mPolyMeshDetail )
      rcFreePolyMeshDetail( mPolyMeshDetail );
    if ( mPolyMesh )
      rcFreePolyMesh( mPolyMesh );

    SAFE_DELETE( mContext );
  }

}