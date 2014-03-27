/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Tue 14 Jan 2014 14:26:09 CET
 *
 * @brief Bindings for a Bob compatible LIBSVM-based Machine for SVMs
 *
 * Copyright (C) 2011-2013 Idiap Research Institute, Martigny, Switzerland
 */

#define XBOB_LEARN_LIBSVM_MODULE
#include <xbob.blitz/cppapi.h>
#include <xbob.blitz/cleanup.h>
#include <xbob.io/api.h>
#include <xbob.learn.libsvm/api.h>
#include <structmember.h>

/***********************************************
 * Implementation of Support Vector File class *
 ***********************************************/

PyDoc_STRVAR(s_file_str, XBOB_EXT_MODULE_PREFIX ".File");

PyDoc_STRVAR(s_file_doc,
"File(path)\n\
\n\
Loads a given LIBSVM data file. The data file format, as\n\
defined on the library README is like this:\n\
\n\
.. code-block:: text\n\
\n\
   <label> <index1>:<value1> <index2>:<value2> ...\n\
   <label> <index1>:<value1> <index2>:<value2> ...\n\
   <label> <index1>:<value1> <index2>:<value2> ...\n\
   ...\n\
\n\
The labels are integer values, so are the indexes, starting\n\
from ``1`` (and not from zero as a C-programmer would expect).\n\
The values are floating point. Zero values are suppressed -\n\
LIBSVM uses a sparse format.\n\
\n\
Upon construction, objects of this class will inspect the input\n\
file so that the maximum sample size is computed. Once that job\n\
is performed, you can read the data in your own pace using the\n\
:py:meth:`File.read` method.\n\
\n\
This class is made available to you so you can input original\n\
LIBSVM files and convert them to another better supported\n\
representation. You cannot, from this object, save data or\n\
extend the current set.\n\
");

static int PyBobLearnLibsvmFile_init
(PyBobLearnLibsvmFileObject* self, PyObject* args, PyObject* kwds) {

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {"path", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyObject* filename = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&", kwlist,
        &PyBobIo_FilenameConverter, &filename))
    return -1;

  auto filename_ = make_safe(filename);

#if PY_VERSION_HEX >= 0x03000000
  const char* c_filename = PyBytes_AS_STRING(filename);
#else
  const char* c_filename = PyString_AS_STRING(filename);
#endif

  try {
    self->cxx = new bob::learn::libsvm::File(c_filename);
  }
  catch (std::exception& ex) {
    PyErr_SetString(PyExc_RuntimeError, ex.what());
    return -1;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "cannot create new object of type `%s' - unknown exception thrown", Py_TYPE(self)->tp_name);
    return -1;
  }

  return 0;

}

static void PyBobLearnLibsvmFile_delete
(PyBobLearnLibsvmFileObject* self) {

  delete self->cxx;
  Py_TYPE(self)->tp_free((PyObject*)self);

}

int PyBobLearnLibsvmFile_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobLearnLibsvmFile_Type));
}

PyDoc_STRVAR(s_shape_str, "shape");
PyDoc_STRVAR(s_shape_doc,
"The size of each sample in the file, as tuple with a single entry");

static PyObject* PyBobLearnLibsvmFile_getShape
(PyBobLearnLibsvmFileObject* self, void* /*closure*/) {
  return Py_BuildValue("(n)", self->cxx->shape());
}

PyDoc_STRVAR(s_samples_str, "samples");
PyDoc_STRVAR(s_samples_doc, "The number of samples in the file");

static PyObject* PyBobLearnLibsvmFile_getSamples
(PyBobLearnLibsvmFileObject* self, void* /*closure*/) {
  return Py_BuildValue("n", self->cxx->samples());
}

PyDoc_STRVAR(s_filename_str, "filename");
PyDoc_STRVAR(s_filename_doc, "The name of the file being read");

static PyObject* PyBobLearnLibsvmFile_getFilename
(PyBobLearnLibsvmFileObject* self, void* /*closure*/) {
  return Py_BuildValue("s", self->cxx->filename().c_str());
}

static PyGetSetDef PyBobLearnLibsvmFile_getseters[] = {
    {
      s_shape_str,
      (getter)PyBobLearnLibsvmFile_getShape,
      0,
      s_shape_doc,
      0
    },
    {
      s_samples_str,
      (getter)PyBobLearnLibsvmFile_getSamples,
      0,
      s_samples_doc,
      0
    },
    {
      s_filename_str,
      (getter)PyBobLearnLibsvmFile_getFilename,
      0,
      s_filename_doc,
      0
    },
    {0}  /* Sentinel */
};

#if PY_VERSION_HEX >= 0x03000000
#  define PYOBJECT_STR PyObject_Str
#else
#  define PYOBJECT_STR PyObject_Unicode
#endif

PyObject* PyBobLearnLibsvmFile_Repr(PyBobLearnLibsvmFileObject* self) {

  /**
   * Expected output:
   *
   * xbob.learn.libsvm.File('filename')
   */

  PyObject* retval = PyUnicode_FromFormat("%s('%s')",
      Py_TYPE(self)->tp_name, self->cxx->filename().c_str());

#if PYTHON_VERSION_HEX < 0x03000000
  if (!retval) return 0;
  PyObject* tmp = PyObject_Str(retval);
  Py_DECREF(retval);
  retval = tmp;
#endif

  return retval;

}

PyObject* PyBobLearnLibsvmFile_Str(PyBobLearnLibsvmFileObject* self) {

  /**
   * Expected output:
   *
   * xbob.learn.libsvm.File('filename') <float64@(3, 4)>
   */

  PyObject* retval = PyUnicode_FromFormat("%s('%s')  <float64@(%" PY_FORMAT_SIZE_T "d, %" PY_FORMAT_SIZE_T "d)>",
      Py_TYPE(self)->tp_name, self->cxx->filename().c_str(),
      self->cxx->samples(), self->cxx->shape());

#if PYTHON_VERSION_HEX < 0x03000000
  if (!retval) return 0;
  PyObject* tmp = PyObject_Str(retval);
  Py_DECREF(retval);
  retval = tmp;
#endif

  return retval;

}

PyDoc_STRVAR(s_reset_str, "reset");
PyDoc_STRVAR(s_reset_doc,
"o.reset() -> None\n\
\n\
Resets the current file so it starts reading from the begin\n\
once more.\n\
");

PyObject* PyBobLearnLibsvmFile_reset(PyBobLearnLibsvmFileObject* self) {
  try {
    self->cxx->reset();
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot reset: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }
  Py_RETURN_NONE;
}

PyDoc_STRVAR(s_good_str, "good");
PyDoc_STRVAR(s_good_doc,
"o.good() -> None\n\
\n\
Returns if the file is in a good state for readout.\n\
It is ``True`` if the current file it has neither the\n\
``eof``, ``fail`` or ``bad`` bits set, whic means that\n\
next :py:meth:`File.read` operation may succeed.\n\
");

PyObject* PyBobLearnLibsvmFile_good(PyBobLearnLibsvmFileObject* self) {
  try {
    if (self->cxx->good()) Py_RETURN_TRUE;
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot check file: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }
  Py_RETURN_FALSE;
}

PyDoc_STRVAR(s_fail_str, "fail");
PyDoc_STRVAR(s_fail_doc,
"o.fail() -> None\n\
\n\
Returns ``True`` if the file has a ``fail`` condition or\n\
``bad`` bit sets. It means the read operation has found a\n\
critical condition and you can no longer proceed in reading\n\
from the file. Note this is not the same as :py:meth:`File.eof`\n\
which informs if the file has ended, but no errors were found\n\
during the read operations.\n\
");

PyObject* PyBobLearnLibsvmFile_fail(PyBobLearnLibsvmFileObject* self) {
  try {
    if (self->cxx->fail()) Py_RETURN_TRUE;
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot check file: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }
  Py_RETURN_FALSE;
}

PyDoc_STRVAR(s_eof_str, "eof");
PyDoc_STRVAR(s_eof_doc,
"o.eof() -> None\n\
\n\
Returns ``True`` if the file has reached its end. To start\n\
reading from the file again, you must call :py:meth:`File.reset`\n\
before another read operation may succeed.\n\
");

PyObject* PyBobLearnLibsvmFile_eof(PyBobLearnLibsvmFileObject* self) {
  try {
    if (self->cxx->eof()) Py_RETURN_TRUE;
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot check file: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }
  Py_RETURN_FALSE;
}

static PyMethodDef PyBobLearnLibsvmFile_methods[] = {
  {
    s_reset_str,
    (PyCFunction)PyBobLearnLibsvmFile_reset,
    METH_NOARGS,
    s_reset_doc
  },
  {
    s_good_str,
    (PyCFunction)PyBobLearnLibsvmFile_good,
    METH_NOARGS,
    s_good_doc
  },
  {
    s_fail_str,
    (PyCFunction)PyBobLearnLibsvmFile_fail,
    METH_NOARGS,
    s_fail_doc
  },
  {
    s_eof_str,
    (PyCFunction)PyBobLearnLibsvmFile_eof,
    METH_NOARGS,
    s_eof_doc
  },
  {0} /* Sentinel */
};

static PyObject* PyBobLearnLibsvmFile_new
(PyTypeObject* type, PyObject*, PyObject*) {

  /* Allocates the python object itself */
  PyBobLearnLibsvmFileObject* self =
    (PyBobLearnLibsvmFileObject*)type->tp_alloc(type, 0);

  self->cxx = 0;

  return reinterpret_cast<PyObject*>(self);

}

PyTypeObject PyBobLearnLibsvmFile_Type = {
    PyVarObject_HEAD_INIT(0, 0)
    s_file_str,                                    /* tp_name */
    sizeof(PyBobLearnLibsvmFileObject),            /* tp_basicsize */
    0,                                             /* tp_itemsize */
    (destructor)PyBobLearnLibsvmFile_delete,       /* tp_dealloc */
    0,                                             /* tp_print */
    0,                                             /* tp_getattr */
    0,                                             /* tp_setattr */
    0,                                             /* tp_compare */
    (reprfunc)PyBobLearnLibsvmFile_Repr,           /* tp_repr */
    0,                                             /* tp_as_number */
    0,                                             /* tp_as_sequence */
    0,                                             /* tp_as_mapping */
    0,                                             /* tp_hash */
    0,                                             /* tp_call */
    (reprfunc)PyBobLearnLibsvmFile_Str,            /* tp_str */
    0,                                             /* tp_getattro */
    0,                                             /* tp_setattro */
    0,                                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,      /* tp_flags */
    s_file_doc,                                    /* tp_doc */
    0,                                             /* tp_traverse */
    0,                                             /* tp_clear */
    0,                                             /* tp_richcompare */
    0,                                             /* tp_weaklistoffset */
    0,                                             /* tp_iter */
    0,                                             /* tp_iternext */
    PyBobLearnLibsvmFile_methods,                  /* tp_methods */
    0,                                             /* tp_members */
    PyBobLearnLibsvmFile_getseters,                /* tp_getset */
    0,                                             /* tp_base */
    0,                                             /* tp_dict */
    0,                                             /* tp_descr_get */
    0,                                             /* tp_descr_set */
    0,                                             /* tp_dictoffset */
    (initproc)PyBobLearnLibsvmFile_init,           /* tp_init */
    0,                                             /* tp_alloc */
    PyBobLearnLibsvmFile_new,                      /* tp_new */
};
