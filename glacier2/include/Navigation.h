#pragma once
#include "Types.h"
#include "Utilities.h"
#include "Services.h"
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  struct NavigationMeshParameters {
  public:
    struct Derived {
      int walkableHeight;
      int walkableClimb;
      int walkableRadius;
      int maxEdgeLength;
      int minRegionArea;
      int mergeRegionArea;
      Real detailSampleDist;
      Real detailSampleMaxError;
    };
  private:
    Derived derived;
  public:
    Real cellSize;
    Real cellHeight;
    Real agentMaxSlope;
    Real agentHeight;
    Real agentMaxClimb;
    Real agentRadius;
    Real edgeMaxLength;
    Real edgeMaxError;
    Real regionMinSize;
    Real regionMergeSize;
    int vertsPerPoly;
    Real detailSampleDist;
    Real detailSampleMaxError;
    bool keepInterResults;
    void updateDerived();
    const Derived& getDerived();
  };

  class NavigationInputGeometry {
  protected:
    SceneNode* mReferenceNode;
    float* mVertices;
    int mVertexCount;
    int* mTriangles;
    int mTriangleCount;
    float* mNormals;
    float* mBBoxMin;
    float* mBBoxMax;
    void calculateExtents( const OgreItemVector& items );
    void convertItems( const OgreItemVector& items );
  public:
    NavigationInputGeometry( const OgreItemVector& items );
    ~NavigationInputGeometry();
    AxisAlignedBox getBoundingBox();
    float* getVertices();
    int getVertexCount();
    int* getTriangles();
    int getTriangleCount();
    float* getNormals();
    float* getMeshBoundsMin();
    float* getMeshBoundsMax();
    const bool isEmpty();
  };

  class NavigationMesh {
  protected:
    static uint32_t headerChunkID;
  protected:
    rcConfig mConfig;
    rcContext* mContext;
    rcHeightfield* mSolid;
    rcCompactHeightfield* mCompact;
    rcContourSet* mContours;
    rcPolyMesh* mPolyMesh;
    rcPolyMeshDetail* mPolyMeshDetail;
    void newPolyMesh();
    void newPolyMeshDetail();
  public:
    NavigationMesh( NavigationMeshParameters& parameters );
    ~NavigationMesh();
    void setParameters( NavigationMeshParameters& parameters );
    void buildFrom( NavigationInputGeometry* geometry );
    void loadFrom( StreamSerialiser& serializer );
    void loadFrom( const UTFString& filename );
    void saveTo( StreamSerialiser& serializer );
    void saveTo( const UTFString& filename );
    const rcPolyMesh* getPolyMesh();
    const rcPolyMeshDetail* getPolyMeshDetail();
  };

  class Navigation: public EngineComponent {
  protected:
    static void* allocator( int size, rcAllocHint hint );
    static void deallocator( void* ptr );
  public:
    Navigation( Engine* engine );
    virtual ~Navigation();
  };

#ifndef GLACIER_NO_NAVIGATION_DEBUG

  class NavigationDebugVisualizer: public duDebugDraw {
  protected:
    Engine* mEngine;
    SceneManager* mScene;
    SceneNode* mNode;
    ManualObject* mManualObject;
    ManualObject* mGrid;
    Ogre::MaterialPtr mMaterial;
    bool mDrawing;
    float mAlpha;
  public:
    explicit NavigationDebugVisualizer( Engine* engine );
    virtual void depthMask( bool state );
    virtual void texture( bool state );
    virtual void begin( duDebugDrawPrimitives prim, float size = 1.0f );
    virtual void vertex( const float* pos, unsigned int color );
    virtual void vertex( const float x, const float y, const float z, unsigned int color );
    virtual void vertex( const float* pos, unsigned int color, const float* uv );
    virtual void vertex( const float x, const float y, const float z, unsigned int color, const float u, const float v );
    virtual void end();
    ~NavigationDebugVisualizer();
  };

#endif

}