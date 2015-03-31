#pragma once
#include "Types.h"
#include "Utilities.h"
#include "Services.h"

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

  typedef std::vector<Ogre::Entity*> OgreEntityVector;

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
    void calculateExtents( const OgreEntityVector& entities );
    void convertEntities( const OgreEntityVector& entities );
  public:
    NavigationInputGeometry( const OgreEntityVector& entities );
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
    rcConfig mConfig;
    rcContext* mContext;
    rcHeightfield* mSolid;
    rcCompactHeightfield* mCompact;
    rcContourSet* mContours;
    rcPolyMesh* mPolyMesh;
    rcPolyMeshDetail* mPolyMeshDetail;
  public:
    NavigationMesh( NavigationMeshParameters& parameters );
    ~NavigationMesh();
    void setParameters( NavigationMeshParameters& parameters );
    void build( NavigationInputGeometry* geometry );
  };

}