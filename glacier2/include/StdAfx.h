#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

#include "TargetVer.h"
#include "CompilerDef.h"
#include "Defines.h"

// Platform Headers
#include <windows.h>          // Windows
#include <shellapi.h>         // ShellAPI
#include <shlobj.h>           // ShellAPI OLE Objects
#include <commctrl.h>         // Common Controls          (comctl32.lib)
#include <richedit.h>         // RichEdit
#include <time.h>             // Time
#include <psapi.h>            // Process Status API       (psapi.lib)
#include <dbghelp.h>          // Debug Help Library       (dbghelp.lib)
#include <avrt.h>             // AVRT                     (avrt.lib)

// nedmalloc Headers
#include <nedmalloc.h>
#undef WIN32_LEAN_AND_MEAN

// VC++ Runtime Headers
#ifdef _DEBUG
//# define _CRTDBG_MAP_ALLOC
# include <crtdbg.h>
#endif
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <eh.h>
#include <intrin.h>

// STL Headers
#include <exception>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <regex>
#include <stack>
#include <cstdint>

// Boost Headers
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/clamp.hpp>
#include <boost/random.hpp>
#include <boost/tokenizer.hpp>
#include <boost/variant.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

// FMOD Headers
#include <fmod.hpp>                   // FMOD API
#include <fmod_errors.h>              // FMOD API
#include <fmod_event.hpp>             // FMOD Designer API
#include <fmod_event_net.h>           // FMOD Designer Network API

// Ogre3D Headers
#include <Ogre.h>
#include <OgreCamera.h>
#include <OgreUTFString.h>
#include <OgreHardwareBufferManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreException.h>
#include <OgreRenderWindow.h>
#include <OgreStreamSerialiser.h>
#include <OgreDeflate.h>
#pragma warning( push )
#pragma warning( disable: 4005 ) // warning C4005: 'SAFE_DELETE' : macro redefinition
#include <RenderSystems/Direct3D9/OgreD3D9RenderSystem.h>
#pragma warning( pop )
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlayElementFactory.h>
#include <Overlay/OgrePanelOverlayElement.h>
#include <Overlay/OgreTextAreaOverlayElement.h>
#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>
#include <Paging/OgrePaging.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainPaging.h>
#include <Terrain/OgreTerrainPagedWorldSection.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <Plugins/PCZSceneManager/OgrePCZPlugin.h>
#include <Plugins/PCZSceneManager/OgrePCZSceneManager.h>
#include <Plugins/PCZSceneManager/OgrePCZSceneNode.h>
#include <Plugins/PCZSceneManager/OgrePCZCamera.h>

// ICU Headers
#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/locid.h>
#include <unicode/ustring.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/numfmt.h>
#include <unicode/unum.h>

// V8 Headers
#include <v8.h>
#include <libplatform/libplatform.h>

// PhysX Headers
#include <PxPhysicsAPI.h>

// Nil Headers
#include <nil.h>

// MyGUI Headers
#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

// OgreProcedural Headers
#pragma warning( push )
#pragma warning( disable: 4267 ) // warning C4267: '=' : conversion from 'size_t' to 'unsigned int', possible loss of data
#include <Procedural.h>
#pragma warning( pop )

// Recast/Detour Headers
#include <RecastAlloc.h>
#include <Recast.h>
#include <DetourAlloc.h>
#include <DetourCommon.h>
#include <DetourMath.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <DetourNode.h>

#ifndef GLACIER_NO_NAVIGATION_DEBUG
# include <DebugDraw.h>
# include <RecastDebugDraw.h>
# include <DetourDebugDraw.h>
#endif

// Glacier types
#include "Types.h"
#include "GlobalFlags.h"