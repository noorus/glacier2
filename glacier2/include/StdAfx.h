#pragma once

#include "TargetVer.h"
#include "CompilerDef.h"
#include "Defines.h"
#include "Types.h"

#define WIN32_LEAN_AND_MEAN

// Platform Headers
#include <windows.h>          // Windows
#include <time.h>             // Time
#include <psapi.h>            // Process Status API       (psapi.lib)
#include <dbghelp.h>          // Debug Help Library       (dbghelp.lib)

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

// Boost Headers
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <boost/tokenizer.hpp>
#include <boost/variant.hpp>
#include <boost/noncopyable.hpp>

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
#include <RenderSystems/Direct3D9/OgreD3D9RenderSystem.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#include <Overlay/OgreOverlayManager.h>
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