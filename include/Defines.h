#pragma once

#if defined( _DEBUG ) && !defined( DEBUG )
# define DEBUG
#endif

#ifndef SAFE_DELETE
# define SAFE_DELETE(p) {if(p){delete p;(p)=NULL;}}
#endif

#ifndef SAFE_RELEASE
# define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}
#endif

#ifndef SAFE_RELEASE_PHYSX
# define SAFE_RELEASE_PHYSX(p) {if(p){(p)->release();(p)=NULL;}}
#endif