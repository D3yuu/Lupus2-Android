#include "StdAfx.h"

// The Android renderer does not yet receive the production captcha image
// packet, but the shared root scripts import this module in every phase.
// Keep the API available so login/select are not coupled to that renderer.
static PyObject* captchaNoop(PyObject*, PyObject*)
{
	return Py_BuildNone();
}

void initcaptcha()
{
	static PyMethodDef methods[] =
	{
		{ "SetCaptchaImage", captchaNoop, METH_VARARGS },
		{ "ClearCaptchaImage", captchaNoop, METH_VARARGS },
		{ "ClearAllCaptchaImage", captchaNoop, METH_VARARGS },
		{ NULL, NULL, 0 },
	};

	PyObject* module = Py_InitModule("captcha", methods);
	PyModule_AddIntConstant(module, "OK", 0);
	PyModule_AddIntConstant(module, "NO_ATTEMPTS", 1);
	PyModule_AddIntConstant(module, "WRONG_CODE", 2);
	PyModule_AddIntConstant(module, "EXPIRE", 3);
	PyModule_AddIntConstant(module, "REQUEST_CHANGE", 4);
	PyModule_AddIntConstant(module, "DISCONNECT", 5);
}
