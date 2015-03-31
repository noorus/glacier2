#include "StdAfx.h"
#include "Navigation.h"
#include "GlacierMath.h"
#include "Exception.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

// This code is heavily based on OgreCrowd, Copyright (c) 2012 Jonas Hauquier.
// Additional contributions by mkultra333 & Paul Wilson.
// Made available under the MIT license.

namespace Glacier {

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

  void NavigationMesh::build( NavigationInputGeometry* geometry )
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

    mPolyMesh = rcAllocPolyMesh();
    if ( !mPolyMesh )
      ENGINE_EXCEPT( "Failed to allocate poly mesh" );

    if ( !rcBuildPolyMesh( mContext, *mContours, mConfig.maxVertsPerPoly, *mPolyMesh ) )
      ENGINE_EXCEPT( "Failed to triangulate contours" );

    mPolyMeshDetail = rcAllocPolyMeshDetail();
    if ( !mPolyMeshDetail )
      ENGINE_EXCEPT( "Failed to allocate detail poly mesh" );

    if ( !rcBuildPolyMeshDetail( mContext, *mPolyMesh, *mCompact,
      mConfig.detailSampleDist, mConfig.detailSampleMaxError, *mPolyMeshDetail ) )
      ENGINE_EXCEPT( "Failed to build detail poly mesh" );

    rcFreeCompactHeightfield( mCompact );
    mCompact = nullptr;

    rcFreeContourSet( mContours );
    mContours = nullptr;

    mContext->stopTimer( RC_TIMER_TOTAL );
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