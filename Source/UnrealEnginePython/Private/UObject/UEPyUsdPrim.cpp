#include "UEPyUsdPrim.h"


#include "UsdWrappers/UsdAttribute.h"
#include "UsdWrappers/SdfLayer.h"
#include "UsdWrappers/SdfPath.h"
#include "UsdWrappers/UsdPrim.h"
#include "UnrealUSDWrapper.h"

#include "USDPrimTwin.h"
#include "USDMemory.h"



// This does not need to be declared in the header file since it's static
static PyObject* py_ue_usd_prim_get_variant_names(ue_PyFUsdPrim* self, PyObject* args)
{
	UE_LOG(LogPython, Warning, TEXT("[py_ue_usd_prim_get_variant_names] called.")); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));

	PyObject* py_list = PyList_New(0);

	/*for (int i = 0; i < uvs.Size(); i++)
	{
		FbxVector2 vec = uvs.GetAt(i);
		PyList_Append(py_list, Py_BuildValue((char*)"(ff)", vec[0], vec[1]));
	}*/

	
	// Cast the pointer to an object
	//static_cast<UE::FUsdPrim>self->usdPrim;
	//UE::FUsdPrim localUsdPrim = *(self->usdPrim);

	bool hasVariants = self->usdPrim->HasVariantSets();
	if (hasVariants) {
		UE_LOG(LogPython, Warning, TEXT("\t\t Has Variant Sets..."));
	}

	/*
	// Get the variant sets
	pxr::UsdVariantSets UsdVariantSets = pxr::UsdPrim(*(self->usdPrim)).GetVariantSets();

	// Get the Variant Set Names
	std::vector<std::string> SetNames = UsdVariantSets.GetNames();

	// --------------------------
	int num_sets = 0;
	for (std::string SetName : SetNames)
	{
		UE_LOG(LogPython, Warning, TEXT("\t\t SetName:  %s"), UTF8_TO_TCHAR(SetName.c_str())); //UTF8_TO_TCHAR(TCHAR_TO_UTF8(parent_name)));	

	}//endof for ----------------
	*/

	return py_list;
}



static PyMethodDef ue_PyFUsdPrim_methods[] = {
	{ "get_variant_names", (PyCFunction)py_ue_usd_prim_get_variant_names, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFUsdPrimType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FUsdPrim", /* tp_name */
	sizeof(ue_PyFUsdPrim),    /* tp_basicsize */
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
	"Unreal Engine FUsdPrim", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFUsdPrim_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};


// -----------------------------------------------------

static int py_ue_usd_prim_init(ue_PyFUsdPrim* self, PyObject* args)
{
	PyObject* py_object;
	char* name;
	if (!PyArg_ParseTuple(args, "Os", &py_object, &name))
	{
		return -1;
	}

	/*
	ue_PyFbxManager* py_fbx_manager = py_ue_is_fbx_manager(py_object);
	if (!py_fbx_manager)
	{
		PyErr_SetString(PyExc_Exception, "argument is not a FbxManager");
		return -1;
	}
	*/

	UE::FUsdPrim temp = UE::FUsdPrim();
	self->usdPrim = &temp;
	return 0;
}



void ue_python_init_usd_prim(PyObject* ue_module)
{
	ue_PyFUsdPrimType.tp_new = PyType_GenericNew;;
	ue_PyFUsdPrimType.tp_init = (initproc)py_ue_usd_prim_init;
	if (PyType_Ready(&ue_PyFUsdPrimType) < 0)
		return;

	Py_INCREF(&ue_PyFUsdPrimType);
	PyModule_AddObject(ue_module, "FUsdPrim", (PyObject*)&ue_PyFUsdPrimType);
}

// -----------------------------------------------------
PyObject* py_ue_new_usd_prim(UE::FUsdPrim& usdPrim)
{
	ue_PyFUsdPrim* ret = (ue_PyFUsdPrim*)PyObject_New(ue_PyFUsdPrim, &ue_PyFUsdPrimType);
	ret->usdPrim = &usdPrim;
	return (PyObject*)ret;
}