#pragma once
#include "Types.h"

// Copyright 2010-2012 Matthew Paul Reid
// Modified for the Glacier2 engine

namespace Glacier {

  class CSMGpuConstants {
  private:
    GpuSharedParametersPtr mParamsScaleBias;
    GpuSharedParametersPtr mParamsShadowMatrix;
    Matrix4 mFirstCascadeViewMatrix;
    Real mFirstCascadeCamWidth;
    Real mViewRange;
    Matrix4 hack;
  public:
    CSMGpuConstants( size_t cascadeCount );
    void updateCascade( const Ogre::Camera& texCam, size_t index );
  };

  class StableCSMShadowCameraSetup: public Ogre::DefaultShadowCameraSetup {
  public:
    typedef std::vector<Ogre::Real> SplitPointList;
  private:
    CSMGpuConstants* mGpuConstants;
  protected:
    size_t mCascadeCount;
    SplitPointList mSplitPoints;
    Ogre::Real mSplitPadding;
    mutable size_t mCurrentIteration;
    void getShadowCameraForCascade( const Ogre::SceneManager *sm, const Ogre::Camera *cam,
      const Ogre::Viewport *vp, const Ogre::Light *light, Ogre::Camera *texCam, size_t iteration,
      Ogre::Real nearSplit, Ogre::Real farSplit ) const;
  public:
    StableCSMShadowCameraSetup( CSMGpuConstants* constants );
    ~StableCSMShadowCameraSetup();

    /** Calculate a new splitting scheme.
    @param cascadeCount The number of cascades to use
    @param firstSplitDist distance of the first split
    @param farDist The far plane to use for the last split
    @param lambda Value between 0 (linear splits) and 1 (logarithmic splits)
    */
    void calculateSplitPoints( size_t cascadeCount, Ogre::Real firstSplitDist, Ogre::Real farDist, Ogre::Real lambda = 0.95 );

    /** Manually configure a new splitting scheme.
    @param newSplitPoints A list which is cascadeCount + 1 entries long, containing the
    split points. The first value is the near point, the last value is the
    far point, and each value in between is both a far point of the previous
    split, and a near point for the next one.
    */
    void setSplitPoints( const SplitPointList& newSplitPoints );

    void setSplitPadding( Ogre::Real pad ) { mSplitPadding = pad; }

    /** Get the padding factor to apply to the near & far distances when matching up
    splits to one another, to avoid 'cracks'.
    */
    Ogre::Real getSplitPadding() const { return mSplitPadding; }
    /// Get the number of splits.
    size_t getCascadeCount() const { return mCascadeCount; }

    /// Returns a stable CSM shadow camera for the given iteration
    virtual void getShadowCamera( const Ogre::SceneManager *sm, const Ogre::Camera *cam,
      const Ogre::Viewport *vp, const Ogre::Light *light, Ogre::Camera *texCam, size_t iteration ) const;

    /// Returns the calculated split points.
    inline const SplitPointList& getSplitPoints() const { return mSplitPoints; }
  };

}