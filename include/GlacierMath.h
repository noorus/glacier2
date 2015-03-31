#pragma once
#include "Types.h"
#include <algorithm>
#include <math.h>

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Math {

    //! \fn pxVec3ToOgre
    //! Convert from PhysX PxVec3 to Ogre Vector3.
    inline Vector3 pxVec3ToOgre( const physx::PxVec3& v )
    {
      return Vector3( v.x, v.y, v.z );
    }

    //! \fn ogreVec3ToPx
    //! Convert from Ogre Vector3 to PhysX PxVec3.
    inline physx::PxVec3 ogreVec3ToPx( const Vector3& v )
    {
      return physx::PxVec3( v.x, v.y, v.z );
    }

    //! \fn pxExtendedVec3ToOgre
    //! Convert from PhysX PxExtendedVec3 to Ogre Vector3.
    inline Vector3 pxExtVec3ToOgre( const physx::PxExtendedVec3& v )
    {
      return Vector3( (Real)v.x, (Real)v.y, (Real)v.z );
    }

    //! \fn ogreVec3ToPxExt
    //! Convert from Ogre Vector3 to PhysX PxExtendedVec3.
    inline physx::PxExtendedVec3 ogreVec3ToPxExt( const Vector3& v )
    {
      return physx::PxExtendedVec3( v.x, v.y, v.z );
    }

    //! \fn ogreVec3ToFMOD
    //! Convert from Ogre Vector3 to FMod FMOD_VECTOR.
    inline FMOD_VECTOR ogreVec3ToFMOD( const Vector3& v )
    {
      FMOD_VECTOR fv;
      fv.x = v.x;
      fv.y = v.y;
      fv.z = v.z;
      return fv;
    }

    //! \fn floatArrayToOgreVec3
    //! Convert from float[3] to Ogre Vector3.
    inline Vector3 floatArrayToOgreVec3( const float* vec )
    {
      return Vector3( vec[0], vec[1], vec[2] );
    }

    //! \fn ogreVec3ToFloatArray
    //! Convert from Ogre Vector3 to float[3].
    inline void ogreVec3ToFloatArray( const Vector3& vec, float* out )
    {
      out[0] = vec.x;
      out[1] = vec.y;
      out[2] = vec.z;
    }

    //! \fn pxQtToOgre
    //! Convert from PhysX NxQuat to Ogre Quaternion.
    inline Quaternion pxQtToOgre( const physx::PxQuat& q )
    {
      return Quaternion( q.w, q.x, q.y, q.z );
    }

    //! \fn ogreQtToPx
    //! Convert from Ogre Quaternion to PhysX NxQuat.
    inline physx::PxQuat ogreQtToPx( const Quaternion& q )
    {
      physx::PxQuat qt;
      qt.x = q.x;
      qt.y = q.y;
      qt.z = q.z;
      qt.w = q.w;
      return qt;
    }

    //! \fn ogreMat3ToPx
    //! Convert from Ogre Matrix3 to PhysX NxMat33.
    inline physx::PxMat33 ogreMat3ToPx( const Matrix3& m )
    {
      physx::PxMat33 mat(
        ogreVec3ToPx( m.GetColumn( 0 ) ),
        ogreVec3ToPx( m.GetColumn( 1 ) ),
        ogreVec3ToPx( m.GetColumn( 2 ) ) );
      return mat;
    }

    //! \fn screenToViewport
    //! Convert from screen-space (0..w,0..h) to viewport space (-1..1,1..-1).
    inline Vector2 screenToViewport( Vector2 v, float viewportW, float viewportH )
    {
      Vector2 n;
      n.x = ( ( v.x / viewportW ) * 2.0f ) - 1.0f;
      n.y = 1.0f - ( ( ( v.y ) / viewportH ) * 2.0f );
      return n;
    }

    //! \fn viewportCenteredBox
    //! Work out the position for a fixed-size box centered in a viewport.
    inline Vector2 viewportCenteredBox( const Viewport* v, int w, int h )
    {
      auto vw = (Real)v->getActualWidth();
      auto vh = (Real)v->getActualHeight();
      Vector2 vec( ( vw / 2 ) - ( w / 2 ), ( vh / 2 ) - ( h / 2 ) );
      return vec;
    }

    //! \fn interpolateLinear
    //! Linear interpolation between v1..v2 at interp[0..1]
    template <typename T>
    inline T interpolateLinear( const T& v1, const T& v2, const float interp )
    {
      return ( v1 * ( 1.0f - interp ) + v2 * interp );
    }

    //! \fn interpolateCosine
    //! Cosine interpolation between v1..v2 at interp[0..1]
    template <typename T>
    inline T interpolateCosine( const T& v1, const T& v2, const float interp )
    {
      const float t = ( 1.0f - cosf( interp * (float)M_PI ) ) / 2.0f;
      return ( v1 * ( 1.0f - t ) + v2 * t );
    }

    //! \fn interpolateCubic
    //! Cubic spline between v1..v2 at interp[0..1]
    template <typename T>
    inline T interpolateCubic( const T& v0, const T& v1, const T& v2, const T& v3, const float interp )
    {
      const float t = interp * interp;
      T a0 = v3 - v2 - v0 + v1;
      T a1 = v0 - v1 - a0;
      T a2 = v2 - v0;
      return ( a0 * interp * t + a1 * t + a2 * interp + v1 );
    }

    //! \fn interpolateQuadraticBezier
    //! Quadratic Beziér interpolation between v1..v2 at interp[0..1]
    //! using control point control
    template <typename T>
    inline T interpolateQuadraticBezier( const T& v1, const T& v2, const T& control, const float interp )
    {
      const float t = ( 1.0f - interp );
      return t * t * v1 + 2 * t * interp * control + interp * interp * v2;
    }

    //! \fn clamp
    //! Clamp a value between minv & maxv
    template <typename T>
    inline T clamp( const T value, const T minv, const T maxv )
    {
      return std::max( std::min( value, maxv ), minv );
    }

  }

}