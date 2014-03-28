/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Tue 25 Mar 2014 12:58:31 CET
 *
 * @brief C/C++ API for bob::machine
 */

#ifndef XBOB_LEARN_LIBSVM_H
#define XBOB_LEARN_LIBSVM_H

#include <Python.h>
#include <xbob.learn.libsvm/config.h>
#include <xbob.learn.libsvm/file.h>
#include <xbob.learn.libsvm/machine.h>
#include <xbob.learn.libsvm/trainer.h>

#define XBOB_LEARN_LIBSVM_MODULE_PREFIX xbob.learn.libsvm
#define XBOB_LEARN_LIBSVM_MODULE_NAME _library

/*******************
 * C API functions *
 *******************/

/**************
 * Versioning *
 **************/

#define PyXbobLearnLibsvm_APIVersion_NUM 0
#define PyXbobLearnLibsvm_APIVersion_TYPE int

/***************************************
 * Bindings for xbob.learn.libsvm.File *
 ***************************************/

typedef struct {
  PyObject_HEAD
  bob::learn::libsvm::File* cxx;
} PyBobLearnLibsvmFileObject;

#define PyBobLearnLibsvmFile_Type_NUM 1
#define PyBobLearnLibsvmFile_Type_TYPE PyTypeObject

#define PyBobLearnLibsvmFile_Check_NUM 2
#define PyBobLearnLibsvmFile_Check_RET int
#define PyBobLearnLibsvmFile_Check_PROTO (PyObject* o)

/******************************************
 * Bindings for xbob.learn.libsvm.Machine *
 ******************************************/

typedef struct {
  PyObject_HEAD
  bob::learn::libsvm::Machine* cxx;
} PyBobLearnLibsvmMachineObject;

#define PyBobLearnLibsvmMachine_Type_NUM 3
#define PyBobLearnLibsvmMachine_Type_TYPE PyTypeObject

#define PyBobLearnLibsvmMachine_Check_NUM 4
#define PyBobLearnLibsvmMachine_Check_RET int
#define PyBobLearnLibsvmMachine_Check_PROTO (PyObject* o)

#define PyBobLearnLibsvmMachine_NewFromMachine_NUM 5
#define PyBobLearnLibsvmMachine_NewFromMachine_RET PyObject*
#define PyBobLearnLibsvmMachine_NewFromMachine_PROTO (bob::learn::libsvm::Machine* m)

/******************************************
 * Bindings for xbob.learn.libsvm.Trainer *
 ******************************************/

typedef struct {
  PyObject_HEAD
  bob::learn::libsvm::Trainer* cxx;
} PyBobLearnLibsvmTrainerObject;

#define PyBobLearnLibsvmTrainer_Type_NUM 6
#define PyBobLearnLibsvmTrainer_Type_TYPE PyTypeObject

#define PyBobLearnLibsvmTrainer_Check_NUM 7
#define PyBobLearnLibsvmTrainer_Check_RET int
#define PyBobLearnLibsvmTrainer_Check_PROTO (PyObject* o)

/*********************************
 * Bindings to general utilities *
 *********************************/

#define PyBobLearnLibsvm_MachineTypeAsString_NUM 8
#define PyBobLearnLibsvm_MachineTypeAsString_RET PyObject*
#define PyBobLearnLibsvm_MachineTypeAsString_PROTO (bob::learn::libsvm::machine_t s)

#define PyBobLearnLibsvm_StringAsMachineType_NUM 9
#define PyBobLearnLibsvm_StringAsMachineType_RET bob::learn::libsvm::machine_t
#define PyBobLearnLibsvm_StringAsMachineType_PROTO (PyObject* o)

#define PyBobLearnLibsvm_CStringAsMachineType_NUM 10
#define PyBobLearnLibsvm_CStringAsMachineType_RET bob::learn::libsvm::machine_t
#define PyBobLearnLibsvm_CStringAsMachineType_PROTO (const char* s)

#define PyBobLearnLibsvm_KernelTypeAsString_NUM 11
#define PyBobLearnLibsvm_KernelTypeAsString_RET PyObject*
#define PyBobLearnLibsvm_KernelTypeAsString_PROTO (bob::learn::libsvm::kernel_t s)

#define PyBobLearnLibsvm_StringAsKernelType_NUM 12
#define PyBobLearnLibsvm_StringAsKernelType_RET bob::learn::libsvm::kernel_t
#define PyBobLearnLibsvm_StringAsKernelType_PROTO (PyObject* o)

#define PyBobLearnLibsvm_CStringAsKernelType_NUM 13
#define PyBobLearnLibsvm_CStringAsKernelType_RET bob::learn::libsvm::kernel_t
#define PyBobLearnLibsvm_CStringAsKernelType_PROTO (const char* s)

/* Total number of C API pointers */
#define PyXbobLearnLibsvm_API_pointers 14

#ifdef XBOB_LEARN_LIBSVM_MODULE

  /* This section is used when compiling `xbob.learn.libsvm' itself */

  /**************
   * Versioning *
   **************/

  extern int PyXbobLearnLibsvm_APIVersion;

  /***************************************
   * Bindings for xbob.learn.libsvm.File *
   ***************************************/

  extern PyBobLearnLibsvmFile_Type_TYPE PyBobLearnLibsvmFile_Type;

  PyBobLearnLibsvmFile_Check_RET PyBobLearnLibsvmFile_Check PyBobLearnLibsvmFile_Check_PROTO;

  /******************************************
   * Bindings for xbob.learn.libsvm.Machine *
   ******************************************/

  extern PyBobLearnLibsvmMachine_Type_TYPE PyBobLearnLibsvmMachine_Type;

  PyBobLearnLibsvmMachine_Check_RET PyBobLearnLibsvmMachine_Check PyBobLearnLibsvmMachine_Check_PROTO;

  PyBobLearnLibsvmMachine_NewFromMachine_RET PyBobLearnLibsvmMachine_NewFromMachine PyBobLearnLibsvmMachine_NewFromMachine_PROTO;

  /******************************************
   * Bindings for xbob.learn.libsvm.Trainer *
   ******************************************/

  extern PyBobLearnLibsvmTrainer_Type_TYPE PyBobLearnLibsvmTrainer_Type;

  PyBobLearnLibsvmTrainer_Check_RET PyBobLearnLibsvmTrainer_Check PyBobLearnLibsvmTrainer_Check_PROTO;

  /*********************************
   * Bindings to general utilities *
   *********************************/

  PyBobLearnLibsvm_MachineTypeAsString_RET PyBobLearnLibsvm_MachineTypeAsString PyBobLearnLibsvm_MachineTypeAsString_PROTO;

  PyBobLearnLibsvm_StringAsMachineType_RET PyBobLearnLibsvm_StringAsMachineType PyBobLearnLibsvm_StringAsMachineType_PROTO;

  PyBobLearnLibsvm_CStringAsMachineType_RET PyBobLearnLibsvm_CStringAsMachineType PyBobLearnLibsvm_CStringAsMachineType_PROTO;

  PyBobLearnLibsvm_KernelTypeAsString_RET PyBobLearnLibsvm_KernelTypeAsString PyBobLearnLibsvm_KernelTypeAsString_PROTO;

  PyBobLearnLibsvm_StringAsKernelType_RET PyBobLearnLibsvm_StringAsKernelType PyBobLearnLibsvm_StringAsKernelType_PROTO;

  PyBobLearnLibsvm_CStringAsKernelType_RET PyBobLearnLibsvm_CStringAsKernelType PyBobLearnLibsvm_CStringAsKernelType_PROTO;

#else

  /* This section is used in modules that use `xbob.learn.libsvm's' C-API */

/************************************************************************
 * Macros to avoid symbol collision and allow for separate compilation. *
 * We pig-back on symbols already defined for NumPy and apply the same  *
 * set of rules here, creating our own API symbol names.                *
 ************************************************************************/

#  if defined(PY_ARRAY_UNIQUE_SYMBOL)
#    define XBOB_LEARN_LIBSVM_MAKE_API_NAME_INNER(a) XBOB_LEARN_LIBSVM_ ## a
#    define XBOB_LEARN_LIBSVM_MAKE_API_NAME(a) XBOB_LEARN_LIBSVM_MAKE_API_NAME_INNER(a)
#    define PyXbobLearnLibsvm_API XBOB_LEARN_LIBSVM_MAKE_API_NAME(PY_ARRAY_UNIQUE_SYMBOL)
#  endif

#  if defined(NO_IMPORT_ARRAY)
  extern void **PyXbobLearnLibsvm_API;
#  else
#    if defined(PY_ARRAY_UNIQUE_SYMBOL)
  void **PyXbobLearnLibsvm_API;
#    else
  static void **PyXbobLearnLibsvm_API=NULL;
#    endif
#  endif

  /**************
   * Versioning *
   **************/

# define PyXbobLearnLibsvm_APIVersion (*(PyXbobLearnLibsvm_APIVersion_TYPE *)PyXbobLearnLibsvm_API[PyXbobLearnLibsvm_APIVersion_NUM])

  /***************************************
   * Bindings for xbob.learn.libsvm.File *
   ***************************************/

# define PyBobLearnLibsvmFile_Type (*(PyBobLearnLibsvmFile_Type_TYPE *)PyXbobLearnLibsvm_API[PyBobLearnLibsvmFile_Type_NUM])

# define PyBobLearnLibsvmFile_Check (*(PyBobLearnLibsvmFile_Check_RET (*)PyBobLearnLibsvmFile_Check_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvmFile_Check_NUM])

  /******************************************
   * Bindings for xbob.learn.libsvm.Machine *
   ******************************************/

# define PyBobLearnLibsvmMachine_Type (*(PyBobLearnLibsvmMachine_Type_TYPE *)PyXbobLearnLibsvm_API[PyBobLearnLibsvmMachine_Type_NUM])

# define PyBobLearnLibsvmMachine_Check (*(PyBobLearnLibsvmMachine_Check_RET (*)PyBobLearnLibsvmMachine_Check_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvmMachine_Check_NUM])

# define PyBobLearnLibsvmMachine_NewFromMachine (*(PyBobLearnLibsvmMachine_NewFromMachine_RET (*)PyBobLearnLibsvmMachine_NewFromMachine_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvmMachine_NewFromMachine_NUM])

  /******************************************
   * Bindings for xbob.learn.libsvm.Trainer *
   ******************************************/

# define PyBobLearnLibsvmTrainer_Type (*(PyBobLearnLibsvmTrainer_Type_TYPE *)PyXbobLearnLibsvm_API[PyBobLearnLibsvmTrainer_Type_NUM])

# define PyBobLearnLibsvmTrainer_Check (*(PyBobLearnLibsvmTrainer_Check_RET (*)PyBobLearnLibsvmTrainer_Check_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvmTrainer_Check_NUM])

  /*********************************
   * Bindings to general utilities *
   *********************************/

# define PyBobLearnLibsvm_MachineTypeAsString (*(PyBobLearnLibsvm_MachineTypeAsString_RET (*)PyBobLearnLibsvm_MachineTypeAsString_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvm_MachineTypeAsString_NUM])

# define PyBobLearnLibsvm_StringAsMachineType (*(PyBobLearnLibsvm_StringAsMachineType_RET (*)PyBobLearnLibsvm_StringAsMachineType_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvm_StringAsMachineType_NUM])

# define PyBobLearnLibsvm_CStringAsMachineType (*(PyBobLearnLibsvm_CStringAsMachineType_RET (*)PyBobLearnLibsvm_CStringAsMachineType_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvm_CStringAsMachineType_NUM])

# define PyBobLearnLibsvm_KernelTypeAsString (*(PyBobLearnLibsvm_KernelTypeAsString_RET (*)PyBobLearnLibsvm_KernelTypeAsString_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvm_KernelTypeAsString_NUM])

# define PyBobLearnLibsvm_StringAsKernelType (*(PyBobLearnLibsvm_StringAsKernelType_RET (*)PyBobLearnLibsvm_StringAsKernelType_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvm_StringAsKernelType_NUM])

# define PyBobLearnLibsvm_CStringAsKernelType (*(PyBobLearnLibsvm_CStringAsKernelType_RET (*)PyBobLearnLibsvm_CStringAsKernelType_PROTO) PyXbobLearnLibsvm_API[PyBobLearnLibsvm_CStringAsKernelType_NUM])

# if !defined(NO_IMPORT_ARRAY)

  /**
   * Returns -1 on error, 0 on success. PyCapsule_Import will set an exception
   * if there's an error.
   */
  static int import_xbob_learn_libsvm(void) {

    PyObject *c_api_object;
    PyObject *module;

    module = PyImport_ImportModule(BOOST_PP_STRINGIZE(XBOB_LEARN_LIBSVM_MODULE_PREFIX) "." BOOST_PP_STRINGIZE(XBOB_LEARN_LIBSVM_MODULE_NAME));

    if (module == NULL) return -1;

    c_api_object = PyObject_GetAttrString(module, "_C_API");

    if (c_api_object == NULL) {
      Py_DECREF(module);
      return -1;
    }

#   if PY_VERSION_HEX >= 0x02070000
    if (PyCapsule_CheckExact(c_api_object)) {
      PyXbobLearnLibsvm_API = (void **)PyCapsule_GetPointer(c_api_object,
          PyCapsule_GetName(c_api_object));
    }
#   else
    if (PyCObject_Check(c_api_object)) {
      XbobLearnLibsvm_API = (void **)PyCObject_AsVoidPtr(c_api_object);
    }
#   endif

    Py_DECREF(c_api_object);
    Py_DECREF(module);

    if (!XbobLearnLibsvm_API) {
      PyErr_Format(PyExc_ImportError,
#   if PY_VERSION_HEX >= 0x02070000
          "cannot find C/C++ API capsule at `%s.%s._C_API'",
#   else
          "cannot find C/C++ API cobject at `%s.%s._C_API'",
#   endif
          BOOST_PP_STRINGIZE(XBOB_LEARN_LIBSVM_MODULE_PREFIX),
          BOOST_PP_STRINGIZE(XBOB_LEARN_LIBSVM_MODULE_NAME));
      return -1;
    }

    /* Checks that the imported version matches the compiled version */
    int imported_version = *(int*)PyXbobLearnLibsvm_API[PyXbobLearnLibsvm_APIVersion_NUM];

    if (XBOB_LEARN_LIBSVM_API_VERSION != imported_version) {
      PyErr_Format(PyExc_ImportError, "%s.%s import error: you compiled against API version 0x%04x, but are now importing an API with version 0x%04x which is not compatible - check your Python runtime environment for errors", BOOST_PP_STRINGIZE(XBOB_LEARN_LIBSVM_MODULE_PREFIX), BOOST_PP_STRINGIZE(XBOB_LEARN_LIBSVM_MODULE_NAME), XBOB_LEARN_LIBSVM_API_VERSION, imported_version);
      return -1;
    }

    /* If you get to this point, all is good */
    return 0;

  }

# endif //!defined(NO_IMPORT_ARRAY)

#endif /* XBOB_LEARN_LIBSVM_MODULE */

#endif /* XBOB_LEARN_LIBSVM_H */
