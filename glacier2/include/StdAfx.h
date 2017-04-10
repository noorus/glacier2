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
//#include <psapi.h>            // Process Status API       (psapi.lib)
//#include <dbghelp.h>          // Debug Help Library       (dbghelp.lib)
#include <avrt.h>             // AVRT                     (avrt.lib)

// nedmalloc Headers
#include <nedmalloc.h>
#undef WIN32_LEAN_AND_MEAN
#define NOMINMAX

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

#undef min
#undef max

// Ogre3D Headers
#pragma warning( push )
#pragma warning( disable: 4267 ) // warning C4267: 'return': conversion from 'size_t' to 'Ogre::uint32', possible loss of data
#pragma warning( disable: 4005 ) // warning C4005: 'SAFE_DELETE': macro redefinition
#include <Ogre.h>
#include <OgreCamera.h>
#include <OgreUTFString.h>
#include <OgreHardwareBufferManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreException.h>
#include <OgreRenderWindow.h>
#include <OgreStreamSerialiser.h>
#include <OgreDeflate.h>
#include <OgreFastArray.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlayElementFactory.h>
#include <Overlay/OgrePanelOverlayElement.h>
#include <Overlay/OgreTextAreaOverlayElement.h>
#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>
#include <OgreManualObject.h>
#include <OgreMesh2.h>
#include <OgreSubMesh2.h>
#include <OgreManualObject2.h>
#include <OgreMeshManager2.h>
#include <OgreHlms.h>
#include <OgreHlmsUnlit.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsManager.h>
#include <OgreHlmsPbsDatablock.h>
#include <OgreArchiveManager.h>
#include <Compositor/OgreCompositorManager2.h>
#include <Compositor/OgreCompositorWorkspace.h>
#include <Compositor/OgreCompositorWorkspaceListener.h>
#include <Compositor/OgreCompositorNodeDef.h>
#include <Compositor/Pass/PassClear/OgreCompositorPassClearDef.h>
#include <OgreItem.h>
#include <OgreSubItem.h>
#include <OgreEntity.h>
#include <RenderSystems/Direct3D11/OgreD3D11Plugin.h>
#include <RenderSystems/Direct3D11/OgreD3D11RenderSystem.h>
#include <OgreBitwise.h>
#include <Vao/OgreAsyncTicket.h>
#pragma warning( pop )

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
#include <libplatform/libplatform.h>
#include <v8.h>

// PhysX Headers
#include <PxPhysicsAPI.h>
#include <extensions/PxSerialization.h>
#include <extensions/PxDefaultStreams.h>
#include <extensions/PxRepXSerializer.h>

// Nil Headers
#include <nil.h>

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