#include "StdAfx.h"
#include "Graphics.h"

namespace Glacier {

  namespace Map {

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