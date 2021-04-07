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
#include "USDStageActor.h"
//#include "USDStage.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		UE::FUsdStage usdStage;
} ue_PyUEFUsdStage;

//extern PyTypeObject ue_PyUEFUsdStageType;
PyObject* py_ue_load_usd_stage(ue_PyUObject*, PyObject*);
PyObject* py_ue_traverse_usd_stage(ue_PyUObject* self, PyObject* args);
PyObject* py_ue_get_variant_names_from_variant(ue_PyUObject* self, PyObject* args);
PyObject* py_ue_set_prim_variant_from_data(ue_PyUObject* self, PyObject* args);


PyObject* py_ue_new_uefusdstage(UE::FUsdStage);
PyObject* py_ue_usd_reset(ue_PyUObject*, PyObject*);
PyObject* py_ue_get_usd_prim_twin(ue_PyUObject*, PyObject*);
PyObject* py_ue_call_on_stage_changed(ue_PyUObject*, PyObject*);
PyObject* py_ue_update_stage(ue_PyUObject*, PyObject*);
