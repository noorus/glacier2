#pragma once
#include "Types.h"

namespace Glacier {

  namespace Map {

    typedef Vector2 Point;

    struct Rect {
      Point topLeft;
      Point bottomRight;
      Rect(): topLeft( 0.0f, 0.0f ), bottomRight( 0.0f, 0.0f ) {}
      Rect( const Real x, const Real y, const Real w, const Real h ): topLeft( x, y ), bottomRight( x + w, y + h ) {}
      Rect( const Point& topLeft_, const Point& bottomRight_ ): topLeft( topLeft_ ), bottomRight( bottomRight_ ) {}
      const void getXYWH( Vector2& XY, Vector2& WH ) const;
      void setFrom( const Point& a, const Point& b ); //!< Using this, doesn't matter which coord is greater
      const bool empty() const;
    };

    class Viewport {
    public:
      Point dimensions_; //!< Size in pixels
      Point position_; //!< Centered origin x,y position in map coordinates
      Viewport(): dimensions_( 0.0f, 0.0f ), position_( 0.0f, 0.0f ) {}
      Viewport( const Point& dimensions, const Point& position ): dimensions_( dimensions ), position_( position ) {}
      const Point cap( const Point& pt ); //!< Cap a coordinate inside the viewport
      const Rect getRect() const; //!< Get as a rectangle in map coordinates
      const Point relativeToRect( const Point& pt ) const;
    };

    class Map {
    public:
      Point dimensions_;
      Point translateTo( Point& pt, Viewport& vp ); //!< A point from map coordinates to relative to viewport
      Rect translateTo( Rect& r, Viewport& vp ); //!< A rectangle from map coordinates to relative to viewport
    };

  }

}