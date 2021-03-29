// ==================================
#include "UEPyUsdStageActor.h"
#include "UEPyUsdPrim.h"

// ---
#include "UsdWrappers\UsdStage.h"


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

//PXR_NAMESPACE_USING_DIRECTIVE
//PXR_NAMESPACE_OPEN_SCOPE

//// IMPORT THIRD-PARTY LIBRARIES
//#include <pxr/usd/usd/stage.h>
//#include <pxr/usd/usd/editContext.h>
//#include <pxr/usd/usd/variantSets.h>
//#include <pxr/usd/usdGeom/sphere.h>




//using std::vector;
//using std::string;

//using namespace pxr;


#if WITH_EDITOR
#include "Editor.h"
#include "Editor/UnrealEd/Public/ComponentTypeRegistry.h"
#endif


// ---------------------------------------------------
static PyMethodDef ue_PyUEFUsdStage_methods[] = {
	{ NULL }  /* Sentinel */
};


// ---------------------------------------------------
static PyTypeObject ue_PyUEFUsdStageType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.UsdStageActor", /* tp_name */
	sizeof(ue_PyUEFUsdStage),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,   /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine UsdStageActor", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyUEFUsdStage_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};



// ------------------
PyObject* py_ue_new_uefusdstage(UE::FUsdStage usdStage)
{
	//ue_py_check(self);
	ue_PyUEFUsdStage* ret = (ue_PyUEFUsdStage*)PyObject_New(ue_PyUEFUsdStage, &ue_PyUEFUsdStageType);
	ret->usdStage = usdStage;
	return (PyObject*)ret;
}

// ------------------
PyObject* py_ue_usd_reset(ue_PyUObject* self, PyObject* args)
{

	ue_py_check(self);

	AUsdStageActor* pUsdStageActor = ue_py_check_type<AUsdStageActor>(self);
	if (!pUsdStageActor)
		return PyErr_Format(PyExc_Exception, "uobject is not a UsdStageActor");

	Py_BEGIN_ALLOW_THREADS;
	pUsdStageActor->Reset();
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;
}


// ------------------
PyObject* py_ue_call_on_stage_changed(ue_PyUObject* self, PyObject* args)
{

	ue_py_check(self);

	AUsdStageActor* pUsdStageActor = ue_py_check_type<AUsdStageActor>(self);
	if (!pUsdStageActor)
		return PyErr_Format(PyExc_Exception, "uobject is not a UsdStageActor");

	Py_BEGIN_ALLOW_THREADS;
	pUsdStageActor->OnStageChanged.Broadcast();
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;
}

// ------------------------------------------------
void fusd_prim_get_children_recursive(UE::FUsdPrim &usdPrim, bool exhaustiveDebug, PyObject* pyList) {

	//UE_LOG(LogPython, Warning, TEXT("[print_fusd_prim_get_children_recursive][start]---------------")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

	// std::string -------------------
	std::string msg = "";

	// Get UE:: items ------------
	//UE::FUsdStage stage = usdPrim.GetStage();
	//UE::FUsdPrim parentPrim = usdPrim.GetParent();
	UE::FSdfPath primPath = usdPrim.GetPrimPath();

	FName primName = usdPrim.GetName();
	FName primTypeName = usdPrim.GetTypeName();

	FString fsPrimName = primName.ToString();
	FString fsPrimTypeName = primTypeName.ToString();
	FString fsPrimPath = primPath.GetString();
	
	
	std::string sPrimName = TCHAR_TO_ANSI(*fsPrimName);
	std::string sPrimTypeName = TCHAR_TO_ANSI(*fsPrimTypeName);
	std::string sPrimPath = TCHAR_TO_ANSI(*fsPrimPath);
	

	msg += "[START]==============================================\n";
	msg += "sPrimName:     " + sPrimName + ", \n";
	msg += "sPrimTypeName: " + sPrimTypeName + ", \n";
	msg += "sPrimPath:     " + sPrimPath + ", \n";

	//UE_LOG(LogUsd, Log, TEXT("%s"), ANSI_TO_TCHAR(msg.c_str()));


	

	
	/*
	* //msg += "sPrimTypeName: , \n" + sPrimTypeName;
	msg += "primTypeName: " + primTypeName.ToString();

	const FString& Path
	TCHAR_TO_ANSI(*Path)

	std::string sPath = TCHAR_TO_ANSI(*Path)*/

	/*
	* ---------------------------------------
	FName name;
	FString >> FName.ToString()
	FString >> parent->ue_object->GetName()
	TCHAR >> *parent->ue_object->GetName()
	TCHAR >> *FString

	

	UTF8 >> sParentName.c_str()
	UTF8 >> char* cstr

	std::string sParentName = "Hello"
	UTF8_TO_TCHAR(sParentName.c_str())
	---------------------------------------
	*/
	//Msg += ": ";
	//Msg += Status.GetCommentary();

	//UE_LOG(LogUsd, Log, TEXT("%s"), ANSI_TO_TCHAR(Msg.c_str()));
	
	
	//UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive][ANSI_TO_TCHAR]  %s"), 
	// ANSI_TO_TCHAR(msg.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
	//UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive][UTF8_TO_TCHAR] %s"), 
	// UTF8_TO_TCHAR(msg.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
	
	

	//
	
	
	bool isValid = usdPrim.IsValid();
	bool isPseudoRoot = usdPrim.IsPseudoRoot();
	bool isModel = usdPrim.IsModel();
	bool isGroup = usdPrim.IsGroup();

	bool hasVariantSets = usdPrim.HasVariantSets();
	bool hasAuthoredReferences = usdPrim.HasAuthoredReferences();
	bool hasPayload = usdPrim.HasPayload();
	bool isLoaded = usdPrim.IsLoaded();


	

	//std::string s_isValid = std::to_string(isValid);
	std::string s_isValid = "false";
	std::string s_isPseudoRoot = "false";
	std::string s_isModel = "false";
	std::string s_isGroup = "false";
	std::string s_hasVariantSets = "false";
	std::string s_hasAuthoredReferences = "false";
	std::string s_hasPayload = "false";
	std::string s_isLoaded = "false";

	if (isValid) s_isValid = "true";
	if (isPseudoRoot) s_isPseudoRoot = "true";
	if (isModel) s_isModel = "true";
	if (isGroup) s_isGroup = "true";
	if (hasVariantSets) s_hasVariantSets = "true";
	if (hasAuthoredReferences) s_hasAuthoredReferences = "true";
	if (hasPayload) s_hasPayload = "true";
	if (isLoaded) s_isLoaded = "true";


	//PyObject* PyDict_GetItem(PyObject * p, PyObject * key)
	//PyObject* variantEntry = PyDict_GetItemString(py_hud_module_dict, TCHAR_TO_UTF8(*PythonClass));

	/*
	std::string s_isModel = std::to_string(isModel);
	std::string s_isGroup = std::to_string(isGroup);
	std::string s_hasVariantSets = std::to_string(hasVariantSets);
	std::string s_hasAuthoredReferences = std::to_string(hasAuthoredReferences);
	std::string s_hasPayload = std::to_string(hasPayload);
	std::string s_isLoaded = std::to_string(isLoaded);
	*/

	// bool value = std::stoi(someString.c_str());
	//std::string s_isValid = std::format("{:6}", isValid);  // value of s6 is "true  "
	
	// If this prim has variant sets then print
	if (hasVariantSets) {

		
		// Create a dict for this object
		PyObject* local_prim_data = PyDict_New();

		//PyDict_SetItemString(local_prim_data, "absolute_filepath", PyUnicode_FromString(TCHAR_TO_UTF8(*import_data->ResolveImportFilename(import_info->SourceFiles[i].RelativeFilename, NULL))));
		PyDict_SetItemString(local_prim_data, "prim_name", PyUnicode_FromString(sPrimName.c_str()));
		PyDict_SetItemString(local_prim_data, "prim_type_name", PyUnicode_FromString(sPrimTypeName.c_str()));
		PyDict_SetItemString(local_prim_data, "prim_path", PyUnicode_FromString(sPrimPath.c_str()));

		// Create a ue_PyFUsdPrim to house the UE::FUsdPrim*
		PyObject* usdPrimPyObj = py_ue_new_usd_prim(usdPrim);
		

		// Set this as the prim_object
		PyDict_SetItemString(local_prim_data, "prim_object", usdPrimPyObj);

		//PyDict_GetItem("prim_object")

		//ue_PyUObject* py_obj = ue_get_python_uobject(*Itr);

		//FUsdPrim::FUsdPrim(const pxr::UsdPrim & InUsdPrim)
		//UE::FUsdPrim()
		
		// PyList -----------------
		PyList_Append(pyList, local_prim_data);

		// ------------------------------
		// PXR_NAMESPACE_OPEN_SCOPE 
		// PXR_NAMESPACE_CLOSE_SCOPE
		// ------------------------------
		if (exhaustiveDebug) {
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] ---------------------------")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] primName:      %s"), *primName.ToString()); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] primTypeName:  %s"), *primTypeName.ToString()); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] fsPrimPath:    %s"), *fsPrimPath); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] isValid:       %s"), UTF8_TO_TCHAR(s_isValid.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] isPseudoRoot:  %s"), UTF8_TO_TCHAR(s_isPseudoRoot.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] isModel:       %s"), UTF8_TO_TCHAR(s_isModel.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] isGroup:       %s"), UTF8_TO_TCHAR(s_isGroup.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] s_hasVariantSets:        %s"), UTF8_TO_TCHAR(s_hasVariantSets.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] s_hasAuthoredReferences: %s"), UTF8_TO_TCHAR(s_hasAuthoredReferences.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] s_hasPayload:            %s"), UTF8_TO_TCHAR(s_hasPayload.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] s_isLoaded:              %s"), UTF8_TO_TCHAR(s_isLoaded.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

		}
			
			


		// This works -------------------
		//const pxr::UsdPrim& pxrUsdPrim = usdPrim;
		//bool pxrUsdPrimHasVarSets = pxrUsdPrim.HasVariantSets();
		//pxr::UsdVariantSets VariantSets = pxrUsdPrim.GetVariantSets();

		// This does not work -----------
		//std::vector<std::string> SetNames2 = VariantSets.GetNames();
		bool getVariantData = false;
		if (getVariantData) {

			// Get the variant sets
			pxr::UsdVariantSets UsdVariantSets = pxr::UsdPrim(usdPrim).GetVariantSets();

			if (exhaustiveDebug) {
				UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive][begin variant sets---------------")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			}
			
			//Log(string(Concat + "\tVariant Sets:\n").c_str());

			// Get the Variant Sets
			std::vector<std::string> SetNames = UsdVariantSets.GetNames();

			// Get the Variant Set Names
			//SetNames = VariantSets.GetNames();

			// --------------------------
			int num_sets = 0;
			for (std::string SetName : SetNames)
			{
				UE_LOG(LogPython, Warning, TEXT("\t\t SetName:  %s"), UTF8_TO_TCHAR(SetName.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));	

			}//endof for ----------------

			if (exhaustiveDebug) {
				UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive][end variant sets---------------")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			}

		}

		

		// -------------------------------
		//if (caseToPxrUsdPrim) {
		//
		//	//pxr::UsdPrim& pxrUsdPrim = usdPrim;
		//	if (pxrUsdPrim) {
		//
		//		if (pxrUsdPrim.HasVariantSets())
		//		{
		//			UE_LOG(LogPython, Warning, TEXT("[print_fusd_prim_get_children_recursive][begin variant sets---------------")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		//			//Log(string(Concat + "\tVariant Sets:\n").c_str());
		//
		//			// Get the Variant Sets
		//			//VariantSets = pxrUsdPrim.GetVariantSets();
		//			std::vector<std::string> SetNames = VariantSets.GetNames();
		//
		//			// Get the Variant Set Names
		//			//SetNames = VariantSets.GetNames();
		//			// --------------------------
		//			int num_sets = 0;
		//			for (std::string SetName : SetNames)
		//			{
		//				UE_LOG(LogPython, Warning, TEXT("\t\t SetName:  %s"), UTF8_TO_TCHAR(SetName.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		//				//UE_LOG(LogPython, Warning, TEXT("\t\t:  %s"), UTF8_TO_TCHAR(SetName.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		//				//Log(string(Concat + "\t\t%s:\n").c_str(), SetName.c_str());
		//			}//endof for ----------------
		//			UE_LOG(LogPython, Warning, TEXT("[print_fusd_prim_get_children_recursive][end variant sets---------------")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		//		}
		//	}

			

		//}//endof try
		////catch (std::exception& e) {
		////catch (const char* msg) {
		//catch (std::exception& e) {
		//	(void)e; // Cast to void to mark this object as "used"
		//	//UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		//	//UTF8_TO_TCHAR(e.what())
		//	UE_LOG(LogPython, Warning, TEXT("Exception thrown...")); 
		//}


		//// -------------------------------------------------
		////TArray< FUsdAttribute > GetAttributes() const;
		////FUsdAttribute GetAttribute(const TCHAR * AttrName) const; */
		////TArray< FUsdPrim > GetChildren() const;
		////TArray< FUsdPrim > GetFilteredChildren(bool bTraverseInstanceProxies) const;
		//TArray< UE::FUsdAttribute > usdAttributesArray = usdPrim.GetAttributes();
		//UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive][begin-attrs] --------------------")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		//for (UE::FUsdAttribute primUsdAttr : usdAttributesArray)
		//{
		//	FName attrFName = primUsdAttr.GetName();
		//	FName attrFBaseName = primUsdAttr.GetBaseName();
		//	FName attrFTypeName = primUsdAttr.GetTypeName();
		//
		//	FString fsAttrFName = attrFName.ToString();
		//	UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive] fsAttrFName:              %s"), *fsAttrFName); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		//
		//}
		//UE_LOG(LogPython, Warning, TEXT("[fusd_prim_get_children_recursive][end-attrs] --------------------")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		//
		/*
		FName GetName() const;
		FName GetBaseName() const;
		FName GetTypeName() const;

		bool ValueMightBeTimeVarying() const;

		FSdfPath GetPath() const;
		FUsdPrim GetPrim() const;
		*/

	}//endif hasVariants ----------------
	
	
	//TArray<FName> names;
	TArray< UE::FUsdPrim > childrenArray = usdPrim.GetChildren();
	for (UE::FUsdPrim childPrim : childrenArray)
	{
		fusd_prim_get_children_recursive(childPrim, exhaustiveDebug, pyList);// , pyDict);
		
	}

}





//==============================================================================
// UE::FUsdPrim
/*
PyObject* __LogPrimTreeHelper(const std::string& Concat, const UsdPrim& Prim)
{
	std::string TypeName = Prim.GetTypeName().GetString();
	bool bIsModel = Prim.IsModel();
	bool bIsAbstract = Prim.IsAbstract();
	bool bIsGroup = Prim.IsGroup();
	bool bIsInstance = Prim.IsInstance();
	bool bIsActive = Prim.IsActive();
	bool bInMaster = Prim.IsInMaster();
	bool bIsMaster = Prim.IsMaster();

	UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage] __LogPrimTreeHelper:      %s"), *pseudoRootName.ToString()); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

	std::string currentLog = string(Concat + "Prim: [%s] %s Model:%d Abstract:%d Group:%d Instance:%d(Master:%s) Active:%d InMaster:%d IsMaster:%d\n").c_str(),
		TypeName.c_str(),
		Prim.GetName().GetText(),
		bIsModel,
		bIsAbstract,
		bIsGroup,
		bIsInstance,
		bIsInstance ? Prim.GetMaster().GetName().GetString().c_str() : "",
		bIsActive,
		bInMaster,
		bIsMaster;

	

	{
		UsdMetadataValueMap Metadata = Prim.GetAllMetadata();
		if (Metadata.size())
		{
			Log(string(Concat + "\tMetaData:\n").c_str());
			for (auto KeyValue : Metadata)
			{
				Log(string(Concat + "\t\t[%s] %s\n").c_str(), KeyValue.second.GetTypeName().c_str(), KeyValue.first.GetText());
			}
		}

		vector<UsdRelationship> Relationships = Prim.GetRelationships();
		if (Relationships.size())
		{
			Log(string(Concat + "\tRelationships:\n").c_str());
			for (const UsdRelationship& Relationship : Relationships)
			{
				SdfPathVector Targets;
				Relationship.GetTargets(&Targets);

				for (SdfPath& Path : Targets)
				{
					Log(string(Concat + "\t\t%s\n").c_str(), Path.GetString().c_str());
				}
			}
		}


		vector<UsdAttribute> Attributes = Prim.GetAttributes();
		if (Attributes.size())
		{
			Log(string(Concat + "\tAttributes:\n").c_str());
			for (const UsdAttribute& Attribute : Attributes)
			{
				if (Attribute.IsAuthored())
				{
					Log(string(Concat + "\t\t[%s] %s %s\n").c_str(), Attribute.GetTypeName().GetAsToken().GetText(), Attribute.GetBaseName().GetText(), Attribute.GetDisplayName().c_str());
				}
			}
		}

		if (Prim.HasVariantSets())
		{
			Log(string(Concat + "\tVariant Sets:\n").c_str());
			UsdVariantSets VariantSets = Prim.GetVariantSets();
			vector<string> SetNames = VariantSets.GetNames();
			for (const string& SetName : SetNames)
			{
				Log(string(Concat + "\t\t%s:\n").c_str(), SetName.c_str());

				UsdVariantSet Set = Prim.GetVariantSet(SetName);

				vector<string> VariantNames = Set.GetVariantNames();
				for (const string& VariantName : VariantNames)
				{
					char ActiveChar = ' ';
					if (Set.GetVariantSelection() == VariantName)
					{
						ActiveChar = '*';
					}
					Log(string(Concat + "\t\t\t%s%c\n").c_str(), VariantName.c_str(), ActiveChar);
				}
			}
		}
	}


	for (const UsdPrim& Child : Prim.GetChildren())
	{
		__LogPrimTreeHelper(Concat + "\t", Child);
	}

	//Log("\n");
	// ------------
	Py_RETURN_NONE;
}

*/
// =======================================================================
// ------------------
PyObject* py_ue_get_variant_names_from_variant(ue_PyUObject* self, PyObject* args)
{
	char* sPrimPath;
	char* sPrimVarSetName;

	// Get the params
	if (!PyArg_ParseTuple(args, "ss", &sPrimPath, &sPrimVarSetName))
		return NULL;


	UE_LOG(LogPython, Warning, TEXT("[py_ue_get_variant_names_from_variant] sPrimPath :       %s"), UTF8_TO_TCHAR(sPrimPath)); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
	UE_LOG(LogPython, Warning, TEXT("[py_ue_get_variant_names_from_variant] sPrimVarSetName : %s"), UTF8_TO_TCHAR(sPrimVarSetName)); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
	

	// ---------------------
	ue_py_check(self);

	AUsdStageActor* pUsdStageActor = ue_py_check_type<AUsdStageActor>(self);
	if (!pUsdStageActor)
		return PyErr_Format(PyExc_Exception, "uobject is not a UsdStageActor");


	// Pointer to FUsdStage
	UE::FUsdStage& pUsdStage = pUsdStageActor->GetUsdStage();

	//UEPyUnicode_AsUTF8(prim_type_name)
	UE::FUsdPrim UsdPrim = pUsdStage.GetPrimAtPath(UE::FSdfPath(UTF8_TO_TCHAR(sPrimPath)));

	PyObject* variant_names = PyList_New(0);

	// If UsdPrim exists and has variant sets
	if (UsdPrim && UsdPrim.HasVariantSets()) {

		//pxr::UsdPrim pxrUsdPrim = pxr::UsdPrim(UsdPrim);
		UE_LOG(LogPython, Warning, TEXT("[py_ue_get_variant_names_from_variant] UsdPrim && UsdPrim.HasVariantSets()..."));

		// Create New List for the Variant Set Variant Names
		//PyObject* variant_set_name_variants_pylist = PyList_New(0);
	
		//std::vector< std::string > UsdVariantNames = pxr::UsdPrim(UsdPrim).GetVariantSet(sPrimVarSetName).GetVariantNames();

		//pxrUsdPrim.GetVariantSet(sPrimVarSetName)

		// [UsdVariantSet] --------------------------
		pxr::UsdVariantSet UsdVariantSet = pxr::UsdPrim(UsdPrim).GetVariantSet(sPrimVarSetName);

		std::vector<std::string> UsdVariantNames = UsdVariantSet.GetVariantNames();

		for (size_t i = 0; i < UsdVariantNames.size(); ++i) {
			const std::string& UsdVariantName = UsdVariantNames[i];
			UE_LOG(LogPython, Warning, TEXT("\t\t [%s] UsdVariantName: %s"), UTF8_TO_TCHAR(sPrimVarSetName), UTF8_TO_TCHAR(UsdVariantName.c_str()));

			/*std::vector<std::string>::const_iterator oldValuesIt =
				std::find(oldValues.begin(), oldValues.end(), newLayer);
			if (oldValuesIt == oldValues.end()) {
				continue;
			}

			const size_t oldLayerOffsetIndex =
				std::distance(oldValues.begin(), oldValuesIt);
			newLayerOffsets[i] = oldLayerOffsets[oldLayerOffsetIndex];*/
		}
		
		//for (const std::string& UsdVariantName : UsdVariantNames) {

		//	UE_LOG(LogPython, Warning, TEXT("\t\t [%s] UsdVariantName: %s"), UTF8_TO_TCHAR(sPrimVarSetName), UTF8_TO_TCHAR(UsdVariantName.c_str()));
		//	//Log(string(Concat + "\t\t\t%s%c\n").c_str(), VariantName.c_str(), ActiveChar);

		//	//PyList_Append(pyList, local_prim_data);
		//	PyList_Append(variant_names, PyUnicode_FromString(UsdVariantName.c_str()));


		//}//endof for -------------------
			
		////pxr::UsdVariantSet UsdVariantSet = pxr::UsdPrim(UsdPrim).GetVariantSet(UsdVariantSetName.c_str());
		////UsdVariantNames = pxr::UsdPrim(UsdPrim).GetVariantSet(UsdVariantSetName.c_str()).GetVariantNames();
		//UsdVariantNames = pxrUsdPrim.GetVariantSet(UsdVariantSetName.c_str()).GetVariantNames();
		////std::vector< std::string > VariantNames = UsdVariantSet.GetVariantNames();



		////for (size_t j = 0; j < VariantNames.size(); ++j)
		////for (const std::string& VariantName : VariantNames)
		////for (auto it = myvector.begin(); it != myvector.end(); ++it)
		////for (size_t j = 0; j < UsdVariantNames.size(); ++j)
		//for (auto it = UsdVariantNames.begin(); it != UsdVariantNames.end(); ++it)
		//{
		//	//char ActiveChar = ' ';
		//	//if (Set.GetVariantSelection() == VariantName)
		//	//{
		//	//	ActiveChar = '*';
		//	//}
		//	//const std::string& VariantName = UsdVariantNames.at(j);
		//	const std::string& VariantName = *it;

		//	UE_LOG(LogPython, Warning, TEXT("\t\t [%s] VariantName: %s"), UTF8_TO_TCHAR(UsdVariantSetName.c_str()), UTF8_TO_TCHAR(VariantName.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
		//	//Log(string(Concat + "\t\t\t%s%c\n").c_str(), VariantName.c_str(), ActiveChar);

		//	//PyList_Append(pyList, local_prim_data);
		//	PyList_Append(variant_set_name_variants_pylist, PyUnicode_FromString(VariantName.c_str()));
		//}


	}

	return variant_names;
	//Py_RETURN_NONE;
	
}


// ------------------
PyObject* py_ue_traverse_usd_stage(ue_PyUObject* self, PyObject* args)
{
	ue_py_check(self);

	AUsdStageActor* pUsdStageActor = ue_py_check_type<AUsdStageActor>(self);
	if (!pUsdStageActor)
		return PyErr_Format(PyExc_Exception, "uobject is not a UsdStageActor");


	// Pointer to FUsdStage
	UE::FUsdStage& pUsdStage = pUsdStageActor->GetUsdStage();

	//UE::FSdfLayer rootLayer = pUsdStage.GetRootLayer();
	//UE::FSdfLayer sessionLayer = pUsdStage.GetSessionLayer();

	UE::FUsdPrim pseudoRoot = pUsdStage.GetPseudoRoot();
	UE::FUsdPrim defaultPrim = pUsdStage.GetDefaultPrim();

	//TArray< FUsdAttribute > GetAttributes()

	//PseudoRoot Name ------
	FName pseudoRootName = pseudoRoot.GetName();
	FName pseudoRootTypeName = pseudoRoot.GetTypeName();

	FString fsPseudoRootName = pseudoRootName.ToString();
	FString fsPseudoRootTypeName = pseudoRootTypeName.ToString();

	// FName(UTF8_TO_TCHAR(name)
	UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage] pseudoRootName:      %s"), *pseudoRootName.ToString()); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
	UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage] pseudoRootTypeName : %s"), *pseudoRootTypeName.ToString()); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

	// ---------------------------
	// Create new PyDict 
	// ---------------------------
	PyObject* py_dict = PyDict_New();
	PyObject* ret = PyList_New(0);

	//PyObject* py_key = ue_py_convert_property(map_helper.KeyProp, ptr, 0);
	//PyObject* py_value = ue_py_convert_property(map_helper.ValueProp, ptr, 0);
	
	/*
	//* ---------------------------------------------
	PyDict_SetItem(py_dict, py_key, py_value);
	PyDict_SetItemString(py_struct_dict, TCHAR_TO_UTF8(*prop_name), struct_value);
	Py_DECREF(py_key);
	Py_DECREF(py_value);
	PyObject *py_list = PyList_New(0);
	TArray<FInputActionKeyMapping> mappings = UPlayerInput::GetEngineDefinedActionMappings();
	for (FInputActionKeyMapping mapping : mappings)
	{
		PyObject *py_mapping = PyDict_New();
		PyDict_SetItemString(py_mapping, (char *)"action_name", PyUnicode_FromString(TCHAR_TO_UTF8(*mapping.ActionName.ToString())));
		PyDict_SetItemString(py_mapping, (char *)"key", PyUnicode_FromString(TCHAR_TO_UTF8(*mapping.Key.ToString())));
		PyDict_SetItemString(py_mapping, (char *)"alt", mapping.bAlt ? Py_True : Py_False);
		PyDict_SetItemString(py_mapping, (char *)"cmd", mapping.bCmd ? Py_True : Py_False);
		PyDict_SetItemString(py_mapping, (char *)"ctrl", mapping.bCtrl ? Py_True : Py_False);
		PyDict_SetItemString(py_mapping, (char *)"shift", mapping.bShift ? Py_True : Py_False);
		PyList_Append(py_list, py_mapping);
	}
	return py_list;

	*/

	UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage][pre] fusd_prim_get_children_recursive()"));
	fusd_prim_get_children_recursive(pseudoRoot, false, ret);// , py_dict);

	// -------------------------
	Py_ssize_t ret_size;
	ret_size = PyList_Size(ret);
	PyObject* dict_item;

	int i;
	int num_items;
	num_items = ret_size;
	std::vector< std::string > UsdVariantSetsNames;
	std::vector<std::string> UsdVariantNames;

	for (i = 0; i < ret_size; i++) {
		dict_item = PyList_GetItem(ret, i); /* Can't fail */

		//const char* PyUnicode_AsUTF8(PyObject * unicode)
		//const char* prim_name_str = UEPyUnicode_AsUTF8(prim_name);
		//self->pin->DefaultValue = UTF8_TO_TCHAR(prim_name_str);


		PyObject* prim_name = PyDict_GetItemString(dict_item, "prim_name");// , PyUnicode_FromString(sPrimName.c_str()));
		PyObject* prim_type_name = PyDict_GetItemString(dict_item, "prim_type_name");//, PyUnicode_FromString(sPrimTypeName.c_str()));
		PyObject* prim_path_name = PyDict_GetItemString(dict_item, "prim_path");//, PyUnicode_FromString(sPrimPath.c_str()));

		// Check if the prim_name is in PyUnicode (which it should be)
		if (PyUnicode_Check(prim_name)) {
			UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage][%d/%d] prim_name_is_unicode: %s"), i + 1, num_items, UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(prim_name)));
		}

		// Check if the prim_type_name is in PyUnicode (which it should be)
		if (PyUnicode_Check(prim_type_name)) {
			UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage][%d/%d] prim_type_name_is_unicode: %s"), i + 1, num_items, UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(prim_type_name)));
		}

		// Check if the prim_path_name is in PyUnicode (which it should be)
		if (PyUnicode_Check(prim_path_name)) {
			UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage][%d/%d] prim_path_name_is_unicode: %s"), i + 1, num_items, UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(prim_path_name)));

			//UEPyUnicode_AsUTF8(prim_type_name)
			UE::FUsdPrim UsdPrim = pUsdStage.GetPrimAtPath(UE::FSdfPath( UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(prim_path_name) )));

			// If UsdPrim exists and has variant sets
			if (UsdPrim && UsdPrim.HasVariantSets()){

				pxr::UsdPrim pxrUsdPrim = pxr::UsdPrim(UsdPrim);
			
				UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage][%d/%d] UsdPrim && UsdPrim.HasVariantSets()..."), i + 1, num_items);

				// Get the variant sets
				//pxr::UsdVariantSets UsdVariantSets = pxr::UsdPrim(UsdPrim).GetVariantSets();
				pxr::UsdVariantSets UsdVariantSets = pxrUsdPrim.GetVariantSets();

				// Create a new std::vector of std::string
				UsdVariantSets.GetNames(&UsdVariantSetsNames);

				// Create a dictionary for local variant set
				PyObject* local_var_set = PyDict_New();

				
				//PyDict_SetItemString(local_prim_data, "prim_name", PyUnicode_FromString(sPrimName.c_str()));
				//PyDict_SetItemString(local_prim_data, "prim_object", usdPrimPyObj);

				

				for (const std::string& UsdVariantSetName : UsdVariantSetsNames)
				{
					//operator[](const std::string& variantSetName)
					//FUsdVariantSetViewModel VariantSet(this);
					//VariantSet.SetName = UsdToUnreal::ConvertString(UsdVariantSetName.c_str());
					//UE_LOG(LogPython, Warning, TEXT("\t\t SetName:  %s"), UTF8_TO_TCHAR(UsdVariantSetName.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
					UE_LOG(LogPython, Warning, TEXT("\t\t [%s] ----------------------------"), UTF8_TO_TCHAR(UsdVariantSetName.c_str()) ); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

					//Log(string(Concat + "\t\t%s:\n").c_str(), SetName.c_str());

					// -----------------------------------------
					// Get the UsdVariantSet
					//pxr::UsdVariantSet Set = pxr::UsdPrim(UsdPrim).GetVariantSet(UsdVariantSetName);
					//
					// Populate the VariantNames std::vector
					//UsdVariantNames = Set.GetVariantNames();
					// ------------------------------------------

					// Create New List for the Variant Set Variant Names
					PyObject* variant_set_name_variants_pylist = PyList_New(0);

					bool skipGettingVariantSetVariantNames = true;
					if (!skipGettingVariantSetVariantNames) {


						//pxr::UsdVariantSet UsdVariantSet = pxr::UsdPrim(UsdPrim).GetVariantSet(UsdVariantSetName.c_str());
						//UsdVariantNames = pxr::UsdPrim(UsdPrim).GetVariantSet(UsdVariantSetName.c_str()).GetVariantNames();
						UsdVariantNames = pxrUsdPrim.GetVariantSet(UsdVariantSetName.c_str()).GetVariantNames();
						//std::vector< std::string > VariantNames = UsdVariantSet.GetVariantNames();
					
					
					
						//for (size_t j = 0; j < VariantNames.size(); ++j)
						//for (const std::string& VariantName : VariantNames)
						//for (auto it = myvector.begin(); it != myvector.end(); ++it)
						//for (size_t j = 0; j < UsdVariantNames.size(); ++j)
						for (auto it = UsdVariantNames.begin(); it != UsdVariantNames.end(); ++it)
						{
							//char ActiveChar = ' ';
							//if (Set.GetVariantSelection() == VariantName)
							//{
							//	ActiveChar = '*';
							//}
							//const std::string& VariantName = UsdVariantNames.at(j);
							const std::string& VariantName = *it;
						
							UE_LOG(LogPython, Warning, TEXT("\t\t [%s] VariantName: %s"), UTF8_TO_TCHAR(UsdVariantSetName.c_str()), UTF8_TO_TCHAR(VariantName.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
							//Log(string(Concat + "\t\t\t%s%c\n").c_str(), VariantName.c_str(), ActiveChar);

							//PyList_Append(pyList, local_prim_data);
							PyList_Append(variant_set_name_variants_pylist, PyUnicode_FromString(VariantName.c_str()));
						}

						UsdVariantNames.clear();

					}
					

					PyDict_SetItemString(local_var_set, UsdVariantSetName.c_str(), variant_set_name_variants_pylist);// PyUnicode_FromString(sPrimName.c_str()));
					

				} // endof for each variant set ------------------

				//dict_item
				PyDict_SetItemString(dict_item, "prim_variants", local_var_set);// PyUnicode_FromString(sPrimName.c_str()));


				// endof for ----------------------------

				//// Get the Variant Sets
				//std::vector<std::string> SetNames = UsdVariantSets.GetNames();

				//// Get the Variant Set Names
				////SetNames = VariantSets.GetNames();

				//// --------------------------
				//int num_sets = 0;
				//for (std::string SetName : SetNames)
				//{
				//	UE_LOG(LogPython, Warning, TEXT("\t\t SetName:  %s"), UTF8_TO_TCHAR(SetName.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));	

				//}//endof for ----------------

				

			}
		}

		//PyObject* prim_object = PyDict_GetItem(item, "prim_object");
		
		//UE::FUsdPrim UsdPrim = UsdStage.GetPrimAtPath(UE::FSdfPath(PrimPath));
		

	}//endof for loop


	UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage] finished traversing stage getting variants...")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

	//UE_LOG(LogPython, Warning, TEXT("[py_ue_traverse_usd_stage][post] print_fusd_prim_get_children_recursive()"));
	//pxr::UsdPrim RootPrim = pUsdStage.GetPseudoRoot();


	//PyObject* parents = PyTuple_GetItem(args, 1);


	/*class USDHelpers
	{

	public:
		static void LogPrimTree(const UsdPrim& Root)*/

	//USDHelpers::LogPrimTree(pseudoRoot);


	//USDHelpers usdHelper = UnrealUSDWrapper::USDHelpers();
	//TUsdStore< pxr::UsdPrim > RootPrim = pUsdStage->GetPseudoRoot();


	//usdStage = UnrealUSDWrapper::OpenStage(*usdFilePath, EUsdInitialLoadSet::LoadAll, false);
	//USDHelpers


	/*for (auto iter = range.begin(); iter != range.end(); ++iter) {
		if (UsdModelAPI(*iter).GetKind() == KindTokens->component) {
			iter.PruneChildren();
		}
		else {
			nonComponents.push_back(*iter);
		}
	}*/


	// root.GetAllDescendants()

	/*
	SdfLayer& rootLayer = pUsdStage->GetRootLayer();
	SdfLayer& sessionLayer = pUsdStage->GetSessionLayer();

	FUsdPrim& pseudoRoot = pUsdStage->GetPseudoRoot();
	FUsdPrim& defaultPrim = pUsdStage->GetDefaultPrim();
	*/


	// ------------
	//Py_RETURN_NONE;

	


	return ret;

}


// ------------------
PyObject* py_ue_get_usd_prim_twin(ue_PyUObject* self, PyObject* args)
{
	ue_py_check(self);

	AUsdStageActor* pUsdStageActor = ue_py_check_type<AUsdStageActor>(self);
	if (!pUsdStageActor)
		return PyErr_Format(PyExc_Exception, "uobject is not a UsdStageActor");

	// Set this to nullptr to check for population
	UUsdPrimTwin* rootUsdPrim = nullptr;
	UClass* usdStageActorClass = pUsdStageActor->GetClass();
	for (FProperty* BPProperty = usdStageActorClass->PropertyLink; BPProperty != nullptr; BPProperty = BPProperty->PropertyLinkNext)
	{
		// Get the name of the current property
		FString propsName = BPProperty->GetName();
		UE_LOG(LogPython, Warning, TEXT("[py_ue_get_usd_stage] propsName: %s"), *propsName); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

		const FString RootUsdTwinName(TEXT("RootUsdTwin"));
		if (BPProperty->GetName() == RootUsdTwinName)
		{
			if (BPProperty->IsA(FObjectProperty::StaticClass())) {

				//TargetActor = Cast<UUsdPrimTwin>(SomeObject);
				//OldWaveStructProperty = CastField<FStructProperty>(BPProperty);

				// Native value
				//uint8* NativeValue = BPProperty->ContainerPtrToValuePtr<uint8>(usdStageActorClass->GetClass()->GetDefaultObject());
				uint8* valuePtr = BPProperty->ContainerPtrToValuePtr<uint8>(usdStageActorClass->GetClass()->GetDefaultObject());


				FObjectProperty* objectProperty = Cast<FObjectProperty>(BPProperty);

				if (objectProperty != nullptr)
				{
					UObject* defaultObject = usdStageActorClass->GetClass()->GetDefaultObject();

					void* ObjectContainer = objectProperty->ContainerPtrToValuePtr<void>(pUsdStageActor);
					UObject* TargetObject = objectProperty->GetObjectPropertyValue(ObjectContainer);

					//UUsdPrimTwin* nonPRootUsdPrim = Cast<UUsdPrimTwin>(TargetObject);
					rootUsdPrim = static_cast<UUsdPrimTwin*>(TargetObject);
					rootUsdPrim->PrimPath = TEXT("/");
					//rootUsdPrim = Cast<UUsdPrimTwin>(TargetObject);


					UE_LOG(LogPython, Warning, TEXT("[py_ue_get_usd_stage] rootUsdPrim->PrimPath: %s"), *rootUsdPrim->PrimPath); // This is legible ("Character")
					//UE_LOG(LogPython, Warning, TEXT("[py_ue_get_usd_stage] rootUsdPrim->PrimPath: %s"), *rootUsdPrim->PrimPath); // This is legible ("Character")


					//rootUsdPrim = objectProperty->ContainerPtrToValuePtr<UUsdPrimTwin>(this);

				}


				////auto objectProp = Cast<FObjectPropertyBase>(BPProperty);
				////FObjectPropertyBase* ExportObjectProp = Cast<FObjectPropertyBase>(BPProperty);
				////FObjectProperty* objectProp = Cast<FObjectProperty>(BPProperty);

				////  ue_py_convert_property(UProperty* prop, uint8* buffer, int32 index)
				//auto casted_prop = Cast<UObjectPropertyBase>(BPProperty);

				//// PyObject* ue_py_convert_property(UProperty* prop, uint8* buffer, int32 index)
				//// uint8* ptr = map_helper.GetPairPtr(i);
				//// uint8* ptr = map_helper.GetPairPtr(i);

				//// uint8* buffer = (uint8*)FMemory_Alloca(u_function->ParmsSize);
				//auto value = casted_prop->GetObjectPropertyValue_InContainer(valuePtr, 0);
				//if (value) {
				//	UE_LOG(LogPython, Warning, TEXT("[py_ue_get_usd_stage] value->GetName(): %s"), *value->GetName()); // This is legible ("Character")
				//}
				//NewTerm.ObjectLiteral = CastField<FObjectProperty>(BPProperty)->GetObjectPropertyValue(BPProperty->ContainerPtrToValuePtr<void>(StructData));
				//UObject* Obj = objectProp->GetObjectPropertyValue(valuePtr);
				//rootUsdPrim = (UUsdPrimTwin *)(Obj);
				//rootUsdPrim = static_cast<UUsdPrimTwin*>(Obj);
				//UE_LOG(LogPython, Warning, TEXT("[py_ue_get_usd_stage] rootUsdPrim->PrimPath: %s"), *rootUsdPrim->PrimPath); // This is legible ("Character")
				//UE_LOG(LogPython, Warning, TEXT("[py_ue_get_usd_stage] Obj->GetName(): %s"), *Obj->GetName()); // This is legible ("Character")


				break;

			}

		}

	}
	bool _continue = false;
	if (rootUsdPrim != nullptr && _continue) {

		//UE_LOG(LogPython, Warning, TEXT("[py_ue_get_usd_stage] rootUsdPrim->PrimPath: %s"), *rootUsdPrim->PrimPath); // This is legible ("Character")
		//rootUsdPrim->Clear();
		//rootUsdPrim->PrimPath = TEXT("/");
		//FString PrimPath;

		// *rootUsdPrim->PrimPath
		UE_LOG(LogPython, Warning, TEXT("[py_ue_get_usd_stage] rootUsdPrim->GetName: %s"), *rootUsdPrim->GetName()); // This is legible ("Character")

	}

	//ReloadAnimations();
	//RootUsdTwin->PrimPath = TEXT("/")
	//TSharedRef< FUsdSchemaTranslationContext > TranslationContext = FUsdStageActorImpl::CreateUsdSchemaTranslationContext(this, );

	// Modify the stage actor
	//pUsdStageActor->Modify();
	//pUsdStageActor->InitialLoadSet = EUsdInitialLoadSet::LoadNone;


	// OnStageChanged.Broadcast();
	Py_RETURN_NONE;

}

PyObject* py_ue_update_stage(ue_PyUObject* self, PyObject* args)
{
	ue_py_check(self);
	UE_LOG(LogPython, Warning, TEXT("[py_ue_update_stage] Pre...")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
	//UUsdPrimTwin* RootUsdTwin;
	/*UUsdPrimTwin *pUUsdPrimTwin = ue_py_check_type<UUsdPrimTwin>(self);
	if (!pUUsdPrimTwin)
		return PyErr_Format(PyExc_Exception, "uobject is not a UUsdPrimTwin");*/

	UE_LOG(LogPython, Warning, TEXT("[py_ue_update_stage] Got UUsdPrimTwin!")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
	Py_RETURN_NONE;

}



// ------------------
PyObject* py_ue_load_usd_stage(ue_PyUObject* self, PyObject* args)
{
	ue_py_check(self);

	AUsdStageActor* pUsdStageActor = ue_py_check_type<AUsdStageActor>(self);
	if (!pUsdStageActor)
		return PyErr_Format(PyExc_Exception, "uobject is not a UsdStageActor");

	/*char *name;
	if (!PyArg_ParseTuple(args, "s:get_actor_component", &name))
	{
		return NULL;
	}*/

	// ------------------------------------------------------------
	// This can get the stage but does not mean it is loaded
	UE::FUsdStage usdStage;


	// -----------------------------------------------------------------
	// Get the .USD file path from the UsdStageActor in the Scene
	// -----------------------------------------------------------------
	FString usdFilePath = pUsdStageActor->RootLayer.FilePath;
	//UE_LOG(LogPython, Warning, TEXT("[py_ue_load_usd_stage] Does USD file path exist?")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

	// Check if this file exists
	if (FPaths::FileExists(usdFilePath))
	{
		UE_LOG(LogPython, Warning, TEXT("[py_ue_load_usd_stage] usdFilePath exists...")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));


		bool isUsdPathValid = FPaths::ValidatePath(usdFilePath);

		// This is valid so load it
		if (isUsdPathValid) {
			UE_LOG(LogPython, Warning, TEXT("[py_ue_load_usd_stage] usdFilePath is valid.")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

			//// ----------------------------------------
			//// This is private so it can't be called
			//// Try loading the stage
			//pUsdStageActor->LoadUsdStage();

			// TUsdStore< pxr::UsdStageRefPtr > UnrealUSDWrapper::OpenUsdStage(const char* Path, const char* Filename)
			// UE::FUsdStage UnrealUSDWrapper::OpenStage( const TCHAR* FilePath, EUsdInitialLoadSet InitialLoadSet, bool bUseStageCache )

			// --------------------------------------------------------------
			// [1] Load the USD file
			// --------------------------------------------------------------
			usdStage = UnrealUSDWrapper::OpenStage(*usdFilePath, EUsdInitialLoadSet::LoadAll, false);

			// If usdStage is opened
			if (usdStage)
			{
				// -----------------------------------
				// Get USD Prim Path
				//UE::FSdfPath UsdPrimPath(*usdFilePath);
				//
				// Get the Root Layer
				//UE::FSdfLayer emptySdfLayer;
				//usdStage.SetEditTarget(usdStage.GetRootLayer());
				//
				//UsdListener.Register(usdStage);
				// -----------------------------------

				/*
				// ORIGINAL CODE
				UsdStage.SetEditTarget(UsdStage.GetRootLayer());
				UsdListener.Register(UsdStage);
				OnStageChanged.Broadcast();
				*/


				// --------------------------------------------------------------
				// [2] This loads the USD into the USD Stage Window
				// --------------------------------------------------------------
				// Setup UsdListener
				//UE::FSdfLayer t = usdStage.GetRootLayer();
				//usdStage.SetEditTarget(t);
				//UE_LOG(LogPython, Warning, TEXT("[py_ue_load_usd_stage] Getting usdStage root layer...")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
				usdStage.SetEditTarget(usdStage.GetRootLayer());
				pUsdStageActor->GetUsdListener().Register(usdStage); // FUsdListener& GetUsdListener()
				pUsdStageActor->OnStageChanged.Broadcast();
				//UE_LOG(LogPython, Warning, TEXT("[py_ue_load_usd_stage] OnStageChanged.Broadcast()...")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

				// --------------------------------------------------------------
				// [3] This forces the UsdStageActor to Update with Children
				// --------------------------------------------------------------
				FPropertyChangedEvent RootLayerPropertyChangedEvent(FindFieldChecked<FProperty>(pUsdStageActor->GetClass(), FName("RootLayer")));
				pUsdStageActor->PostEditChangeProperty(RootLayerPropertyChangedEvent);

				// LOADED in (1) memory, (2) USD Stage Window, and (3) Populated the UsdStageActor
				std::string resultString = "Successfully loaded .USD";
				//FSTring converted_std_str = UTF8_TO_TCHAR(str.c_str())
				return PyUnicode_FromString(resultString.c_str()); // std::string.c_str() --> to UTF8

				//return PyUnicode_FromString(TCHAR_TO_UTF8(*usdFilePath));
				//Py_RETURN_NONE;

			}//endif 

		}
		else {
			UE_LOG(LogPython, Warning, TEXT("[py_ue_get_usd_stage] usdFilePath is NOT valid.")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
			Py_RETURN_NONE;

		}//endif path is valid or not



	}//endif file path exists
	UE_LOG(LogPython, Warning, TEXT("[py_ue_load_usd_stage] ...usdFilePath DOES NOT exist.")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));
	Py_RETURN_NONE;


	//-------------------------------------------
	UE::FUsdStage& pUsdStage = pUsdStageActor->GetUsdStage();
	double startTime = pUsdStage.GetStartTimeCode();

	//TSharedRef< FUsdSchemaTranslationContext > TranslationContext = FUsdStageActorImpl::CreateUsdSchemaTranslationContext(this, RootUsdTwin->PrimPath);
	//pUsdStageActor->Modify();
	//pUsdStageActor->InitialLoadSet = EUsdInitialLoadSet::LoadAll;
	//UnrealUSDWrapper usdStageWrapper;
	//usdStageWrapper.OpenStage(pUsdStageActor->FilePath, EUsdInitialLoadSet InitialLoadSet, false);
	// OnStageChanged event --> Broadcast()
	//pUsdStageActor->OnStageChanged.Broadcast();

	/*void Clear();
	void OpenUsdStage();
	void LoadUsdStage();*/
	// --------------------------------------------------------
	// Can't call these from unresolved external symbol errors
	// --------------------------------------------------------
	//pUsdStageActor->ReloadAnimations();
	//pUsdStageActor->Refresh(); // 

	// Cast this to UObject
	//ue_PyUObject *item = ue_get_python_uobject(UsdStage);
	//ue_PyUObject *item = ue_get_python_uobject_usd(&UsdStage);
	//Py_RETURN_UOBJECT((UObject *)item);
	//Py_RETURN_UOBJECT((UObject *)controller->GetHUD());

	//Py_RETURN_UOBJECT(startTime);
	//Py_RETURN_NONE;
	//return PyFloat_FromDouble(startTime);

}