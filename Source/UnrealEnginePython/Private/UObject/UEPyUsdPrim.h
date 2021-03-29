#pragma once

// -----------------------------------
// UEPyActor.h
// -----------------------------------
#include "UEPyModule.h" // Plugin Header (Plugins\UnrealEnginePython\Source\UnrealEnginePython\Private\UEPyModule.h)
#include "Wrappers/UEPyFVector.h" // Plugin Header (Plugins\UnrealEnginePython\Source\UnrealEnginePython\Private\UEPyModule.h)
#include "Wrappers/UEPyFRotator.h" // Plugin Header (Plugins\UnrealEnginePython\Source\UnrealEnginePython\Private\UEPyModule.h)
#include "UObject/UEPyObject.h" // Plugin Header (Plugins\UnrealEnginePython\Source\UnrealEnginePython\Private\UEPyModule.h)
#include "UObject/UObjectThreadContext.h" // Plugin Header (Plugins\UnrealEnginePython\Source\UnrealEnginePython\Private\UEPyModule.h)
#include "PythonComponent.h" // Plugin Header (Plugins\UnrealEnginePython\Source\UnrealEnginePython\Public\PythonComponent.h)

#include "EngineUtils.h" // Engine header (Engine\Source\Runtime\Engine\Public\EngineUtils.h)
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h" // Engine header (Engine\Source\Runtime\LevelSequence\Public\LevelSequence.h)
#include "Runtime/LevelSequence/Public/LevelSequence.h" // Engine header (Engine\Source\Runtime\LevelSequence\Public\LevelSequence.h)
#include "Kismet/GameplayStatics.h" // Engine header (Engine\Source\Runtime\Engine\Classes\Kismet\GameplayStatics.h)


// -------------------------------------
#include "UsdWrappers/UsdStage.h"
#include "UsdWrappers/UsdAttribute.h"
#include "UsdWrappers/SdfLayer.h"
#include "UsdWrappers/SdfPath.h"
#include "UsdWrappers/UsdPrim.h"
#include "UnrealUSDWrapper.h"

#include "USDPrimTwin.h"
#include "USDMemory.h"



// ---------- pxr --------------
#include "USDIncludesStart.h"

#include "pxr/base/gf/rotation.h"
#include "pxr/base/plug/plugin.h"
#include "pxr/base/plug/registry.h"
#include "pxr/base/tf/diagnosticMgr.h"
#include "pxr/base/tf/errorMark.h"
#include "pxr/base/tf/getenv.h"
#include "pxr/base/tf/setenv.h"
#include "pxr/usd/ar/defaultResolver.h"
#include "pxr/usd/ar/defineResolver.h"
#include "pxr/usd/kind/registry.h"
#include "pxr/usd/sdf/fileFormat.h"
#include "pxr/usd/sdf/schema.h"
#include "pxr/usd/usd/attribute.h"
#include "pxr/usd/usd/common.h"
#include "pxr/usd/usd/debugCodes.h"
#include "pxr/usd/usd/modelAPI.h"
#include "pxr/usd/usd/references.h"
#include "pxr/usd/usd/relationship.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/stageCacheContext.h"
#include "pxr/usd/usd/usdFileFormat.h"
#include "pxr/usd/usd/variantSets.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdGeom/metrics.h"
#include "pxr/usd/usdGeom/modelAPI.h"
#include "pxr/usd/usdGeom/tokens.h"
#include "pxr/usd/usdGeom/xformCommonAPI.h"
#include "pxr/usd/usdLux/light.h"
#include "pxr/usd/usdShade/materialBindingAPI.h"
#include "pxr/usd/usdShade/tokens.h"
#include "pxr/usd/usdUtils/stageCache.h"


#include "USDIncludesEnd.h"


typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		UE::FUsdPrim *usdPrim;
		std::string sdfPathString;

} ue_PyFUsdPrim;


void ue_python_init_usd_prim(PyObject*); // ue_module

PyObject* py_ue_new_usd_prim(const UE::FUsdPrim &); // usdPrim