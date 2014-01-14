#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

#if defined( _DEBUG ) && !defined( DEBUG )
# define DEBUG
#endif

#ifndef SAFE_DELETE
# define SAFE_DELETE(p) {if(p){delete p;(p)=NULL;}}
#endif

#ifndef SAFE_RELEASE
# define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}
#endif

#ifndef SAFE_RELEASE_FMOD
# define SAFE_RELEASE_FMOD(p) {if(p){(p)->release();(p)=NULL;}}
#endif

#ifndef SAFE_RELEASE_PHYSX
# define SAFE_RELEASE_PHYSX(p) {if(p){(p)->release();(p)=NULL;}}
#endif

#ifndef SAFE_CLOSE_HANDLE
# define SAFE_CLOSE_HANDLE(p) {if(p){CloseHandle(p);(p)=NULL;}}
#endif