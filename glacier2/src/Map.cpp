#include "StdAfx.h"
#include "Map.h"

namespace Glacier {

  namespace Map {

    const void Rect::getXYWH( Vector2& XY, Vector2& WH ) const
    {
      XY = topLeft;
      WH = ( bottomRight - topLeft );
    }

    void Rect::setFrom( const Point& a, const Point& b )
    {
      // ugly but probably faster than something "clever"
      if ( a.x < b.x ) {
        topLeft.x = a.x;
        bottomRight.x = b.x;
      }
      else {
        topLeft.x = b.x;
        bottomRight.x = a.x;
      }
      if ( a.y < b.y ) {
        topLeft.y = a.y;
        bottomRight.y = b.y;
      }
      else {
        topLeft.y = b.y;
        bottomRight.y = a.y;
      }
    }

    const Rect Viewport::getRect() const
    {
      auto half = dimensions_ / 2;
      return Rect( position_ - half, position_ + half );
    }

    const Point Viewport::relativeToRect( const Point& pt ) const
    {
      auto half = dimensions_ / 2;
      return ( pt + half );
    }

    const Point Viewport::cap( const Point& pt )
    {
      auto ret = pt;
      if ( ret.x < 0.0f )
        ret.x = 0.0f;
      else if ( ret.x > dimensions_.x )
        ret.x = dimensions_.x;
      if ( ret.y < 0.0f )
        ret.y = 0.0f;
      else if ( ret.y > dimensions_.y )
        ret.y = dimensions_.y;
      return ret;
    }

    Point Map::translateTo( Point& pt, Viewport& vp )
    {
      auto origo = -vp.position_;
      return ( origo + pt );
    }

    Rect Map::translateTo( Rect& r, Viewport& vp )
    {
      auto origo = -vp.position_;
      return Rect( origo + r.topLeft, origo + r.bottomRight );
    }

  }

}