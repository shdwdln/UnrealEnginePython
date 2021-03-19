#include "UEPyUsdStageActor.h"
#include "UsdWrappers\UsdStage.h"

#include "UsdWrappers/SdfLayer.h"
#include "UsdWrappers/SdfPath.h"
#include "UsdWrappers/UsdPrim.h"

#include "USDPrimTwin.h"
#include "USDMemory.h"

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