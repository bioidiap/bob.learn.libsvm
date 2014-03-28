/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Thu 27 Mar 2014 15:44:46 CET
 *
 * @brief Bindings for a Bob compatible LIBSVM-based Trainer for SVMs
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#define XBOB_LEARN_LIBSVM_MODULE
#include <xbob.blitz/cppapi.h>
#include <xbob.blitz/cleanup.h>
#include <xbob.io/api.h>
#include <xbob.learn.libsvm/api.h>
#include <structmember.h>

/*******************************************************
 * Implementation of Support Vector Trainer base class *
 *******************************************************/

PyDoc_STRVAR(s_svm_str, XBOB_EXT_MODULE_PREFIX ".Trainer");

PyDoc_STRVAR(s_svm_doc,
"Trainer([machine_type='C_SVC', [kernel_type='RBF', [cache_size=100, [eps=1e-3, [shrinking=True, [probability=False]]]]]]) -> new Trainer\n\
\n\
This class emulates the behavior of the command line utility\n\
called ``svm-train``, from LIBSVM. It allows you to create a\n\
parameterized LIBSVM trainer to fullfil a variety of needs and\n\
configurations. The constructor includes parameters which are\n\
global to all machine and kernel types. Specific parameters\n\
for specific machines or kernel types can be fine-tuned using\n\
object attributes (see help documentation).\n\
\n\
**Parameters**\n\
\n\
 machine_type, str\n\
   The type of SVM to be trained. Valid options are:\n\
   \n\
   * ``'C_SVC'`` (the default)\n\
   * ``'NU_SVC'``\n\
   * ``'ONE_CLASS'`` (**unsupported**)\n\
   * ``'EPSILON_SVR'`` (**unsupported** regression)\n\
   * ``'NU_SVR'`` (**unsupported** regression)\n\
\n\
kernel_type, str\n\
  The type of kernel to deploy on this machine. Valid options are:\n\
  \n\
  *  ``'LINEAR'``, for a linear kernel\n\
  *  ``'POLY'``, for a polynomial kernel\n\
  *  ``'RBF'``, for a radial-basis function kernel\n\
  *  ``'SIGMOID'``, for a sigmoidal kernel\n\
  *  ``'PRECOMPUTED'``, for a precomputed, user provided kernel\n\
    please note this option is currently **unsupported**.\n\
\n\
cache_size, float\n\
  The size of LIBSVM's internal cache, in megabytes\n\
\n\
eps, float\n\
  The epsilon value for the training stopping criterion\n\
\n\
shrinking, bool\n\
  If set to ``True`` (the default), the applies LIBSVM's\n\
  shrinking heuristic.\n\
\n\
probability, bool\n\
  If set to ``True``, then allows the outcoming machine\n\
  produced by this trainer to output probabilities\n\
  besides scores and class estimates. The default for\n\
  this option is ``False``.\n\
\n\
.. note::\n\
\n\
   These bindings do not support:\n\
   \n\
   * Precomputed Kernels\n\
   * Regression Problems\n\
   * Different weights for every label (-wi option in svm-train)\n\
   \n\
   Fell free to implement those and remove these remarks.\n\
\n\
");

/***********************************************
 * Implementation of xbob.learn.libsvm.Trainer *
 ***********************************************/

static int PyBobLearnLibsvmTrainer_init
(PyBobLearnLibsvmTrainerObject* self, PyObject* args, PyObject* kwds) {

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {
    "machine_type",
    "kernel_type",
    "cache_size",
    "eps",
    "shrinking",
    "probability",
    0,
  };
  static char** kwlist = const_cast<char**>(const_kwlist);

  const char* machine_type = "C_SVC";
  const char* kernel_type = "RBF";
  double cache_size= 100;
  double eps = 1e-3;
  PyObject* shrinking = Py_True; ///< borrowed
  PyObject* probability = Py_False; ///< borrowed

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ssddOO", kwlist,
        &machine_type, &kernel_type, &cache_size, &eps, &shrinking, &probability))
    return -1;

  bob::learn::libsvm::machine_t c_machine_type =
    PyBobLearnLibsvm_CStringAsMachineType(machine_type);
  if (PyErr_Occurred()) return -1;
  bob::learn::libsvm::kernel_t c_kernel_type =
    PyBobLearnLibsvm_CStringAsKernelType(kernel_type);
  if (PyErr_Occurred()) return -1;
  bool c_shrinking = PyObject_IsTrue(shrinking);
  bool c_probability = PyObject_IsTrue(probability);

  try {
    self->cxx = new bob::learn::libsvm::Trainer(c_machine_type, c_kernel_type,
        cache_size, eps, c_shrinking, c_probability);
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

static void PyBobLearnLibsvmTrainer_delete
(PyBobLearnLibsvmTrainerObject* self) {

  delete self->cxx;
  Py_TYPE(self)->tp_free((PyObject*)self);

}

int PyBobLearnLibsvmTrainer_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobLearnLibsvmTrainer_Type));
}

PyDoc_STRVAR(s_machine_type_str, "machine_type");
PyDoc_STRVAR(s_machine_type_doc, "The type of SVM machine that will be trained");

static PyObject* PyBobLearnLibsvmTrainer_getMachineType
(PyBobLearnLibsvmTrainerObject* self, void* /*closure*/) {
  return PyBobLearnLibsvm_MachineTypeAsString(self->cxx->getMachineType());
}

static PyObject* PyBobLearnLibsvmTrainer_setMachineType
(PyBobLearnLibsvmTrainerObject* self, PyObject* o, void* /*closure*/) {
  auto m = PyBobLearnLibsvm_StringAsMachineType(o);
  if (PyErr_Occurred()) return 0;
  self->cxx->setMachineType(m);
  Py_RETURN_NONE;
}

PyDoc_STRVAR(s_svm_kernel_type_str, "kernel_type");
PyDoc_STRVAR(s_svm_kernel_type_doc,
"The type of kernel used by the support vectors in this machine");

static PyObject* PyBobLearnLibsvmTrainer_getKernelType
(PyBobLearnLibsvmTrainerObject* self, void* /*closure*/) {
  return PyBobLearnLibsvm_KernelTypeAsString(self->cxx->getKernelType());
}

static PyObject* PyBobLearnLibsvmTrainer_setKernelType
(PyBobLearnLibsvmTrainerObject* self, PyObject* o, void* /*closure*/) {
  auto m = PyBobLearnLibsvm_StringAsKernelType(o);
  if (PyErr_Occurred()) return 0;
  self->cxx->setKernelType(m);
  Py_RETURN_NONE;
}

PyDoc_STRVAR(s_degree_str, "degree");
PyDoc_STRVAR(s_degree_doc,
"The polinomial degree, only valid if the kernel is ``'POLY'``\n\
(polynomial)");

static PyObject* PyBobLearnLibsvmTrainer_getDegree
(PyBobLearnLibsvmTrainerObject* self, void* /*closure*/) {
  return Py_BuildValue("i", self->cxx->getDegree());
}

static PyObject* PyBobLearnLibsvmTrainer_setDegree
(PyBobLearnLibsvmTrainerObject* self, PyObject* o, void* /*closure*/) {
  self->cxx->setDegree(PyNumber_AsSsize_t(o, PyExc_OverflowError));
  if (PyErr_Occurred()) return 0;
  Py_RETURN_NONE;
}

PyDoc_STRVAR(s_gamma_str, "gamma");
PyDoc_STRVAR(s_gamma_doc,
"The :math:`\\gamma` parameter for ``'POLY'`` (polynomial),\n\
``'RBF'`` (gaussian) or ``'SIGMOID'`` (sigmoidal) kernels");

static PyObject* PyBobLearnLibsvmTrainer_getGamma
(PyBobLearnLibsvmTrainerObject* self, void* /*closure*/) {
  return Py_BuildValue("d", self->cxx->getGamma());
}

static PyObject* PyBobLearnLibsvmTrainer_setGamma
(PyBobLearnLibsvmTrainerObject* self, PyObject* o, void* /*closure*/) {
  self->cxx->setGamma(PyFloat_AsDouble(o));
  if (PyErr_Occurred()) return 0;
  Py_RETURN_NONE;
}

PyDoc_STRVAR(s_coef0_str, "coef0");
PyDoc_STRVAR(s_coef0_doc,
"The coefficient 0 for ``'POLY'`` (polynomial) or\n\
``'SIGMOID'`` (sigmoidal) kernels");

static PyObject* PyBobLearnLibsvmTrainer_getCoef0
(PyBobLearnLibsvmTrainerObject* self, void* /*closure*/) {
  return Py_BuildValue("d", self->cxx->getCoef0());
}

static PyObject* PyBobLearnLibsvmTrainer_setCoef0
(PyBobLearnLibsvmTrainerObject* self, PyObject* o, void* /*closure*/) {
  self->cxx->setCoef0(PyFloat_AsDouble(o));
  if (PyErr_Occurred()) return 0;
  Py_RETURN_NONE;
}

PyDoc_STRVAR(s_probability_str, "probability");
PyDoc_STRVAR(s_probability_doc,
"If set to ``True``, output Machines will support outputting\n\
probability estimates");

static PyObject* PyBobLearnLibsvmTrainer_getSupportsProbability
(PyBobLearnLibsvmTrainerObject* self, void* /*closure*/) {
  if (self->cxx->getProbabilityEstimates()) Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

static PyObject* PyBobLearnLibsvmTrainer_setSupportsProbability
(PyBobLearnLibsvmTrainerObject* self, PyObject* o, void* /*closure*/) {
  if (PyObject_IsTrue(o)) self->cxx->setProbabilityEstimates(true);
  else self->cxx->setProbabilityEstimates(false);
  Py_RETURN_NONE;
}

PyDoc_STRVAR(s_shrinking_str, "shrinking");
PyDoc_STRVAR(s_shrinking_doc,
"If set to ``True``, then use LIBSVM's Shrinking Heuristics");

static PyObject* PyBobLearnLibsvmTrainer_getShrinking
(PyBobLearnLibsvmTrainerObject* self, void* /*closure*/) {
  if (self->cxx->getUseShrinking()) Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

static PyObject* PyBobLearnLibsvmTrainer_setShrinking
(PyBobLearnLibsvmTrainerObject* self, PyObject* o, void* /*closure*/) {
  if (PyObject_IsTrue(o)) self->cxx->setUseShrinking(true);
  else self->cxx->setUseShrinking(false);
  Py_RETURN_NONE;
}

static PyGetSetDef PyBobLearnLibsvmTrainer_getseters[] = {
    {
      s_machine_type_str,
      (getter)PyBobLearnLibsvmTrainer_getMachineType,
      (setter)PyBobLearnLibsvmTrainer_setMachineType,
      s_machine_type_doc,
      0
    },
    {
      s_svm_kernel_type_str,
      (getter)PyBobLearnLibsvmTrainer_getKernelType,
      (setter)PyBobLearnLibsvmTrainer_setKernelType,
      s_svm_kernel_type_doc,
      0
    },
    {
      s_degree_str,
      (getter)PyBobLearnLibsvmTrainer_getDegree,
      (setter)PyBobLearnLibsvmTrainer_setDegree,
      s_degree_doc,
      0
    },
    {
      s_gamma_str,
      (getter)PyBobLearnLibsvmTrainer_getGamma,
      (setter)PyBobLearnLibsvmTrainer_setGamma,
      s_gamma_doc,
      0
    },
    {
      s_coef0_str,
      (getter)PyBobLearnLibsvmTrainer_getCoef0,
      (setter)PyBobLearnLibsvmTrainer_setCoef0,
      s_coef0_doc,
      0
    },
    {
      s_probability_str,
      (getter)PyBobLearnLibsvmTrainer_getSupportsProbability,
      (setter)PyBobLearnLibsvmTrainer_setSupportsProbability,
      s_probability_doc,
      0
    },
    {
      s_shrinking_str,
      (getter)PyBobLearnLibsvmTrainer_getShrinking,
      (setter)PyBobLearnLibsvmTrainer_setShrinking,
      s_shrinking_doc,
      0
    },
    {0}  /* Sentinel */
};

#if PY_VERSION_HEX >= 0x03000000
#  define PYOBJECT_STR PyObject_Str
#else
#  define PYOBJECT_STR PyObject_Unicode
#endif

PyObject* PyBobLearnLibsvmTrainer_Repr(PyBobLearnLibsvmTrainerObject* self) {

  /**
   * Expected output:
   *
   * <xbob.learn.libsvm.Trainer>
   *
   * @TODO: Improve this output
   */

  PyObject* retval = PyUnicode_FromFormat("<%s>", Py_TYPE(self)->tp_name);

#if PYTHON_VERSION_HEX < 0x03000000
  if (!retval) return 0;
  PyObject* tmp = PyObject_Str(retval);
  Py_DECREF(retval);
  retval = tmp;
#endif

  return retval;

}

PyObject* PyBobLearnLibsvmTrainer_Str(PyBobLearnLibsvmTrainerObject* self) {

  /**
   * Expected output:
   *
   * xbob.learn.libsvm.Trainer
   *
   * TODO: Not fully implemented yet
   */

  PyObject* retval = PyUnicode_FromFormat("%s", Py_TYPE(self)->tp_name);

#if PYTHON_VERSION_HEX < 0x03000000
  if (!retval) return 0;
  PyObject* tmp = PyObject_Str(retval);
  Py_DECREF(retval);
  retval = tmp;
#endif

  return retval;

}

PyDoc_STRVAR(s_train_str, "train");
PyDoc_STRVAR(s_train_doc,
"o.train(data, [subtract, divide]) -> array\n\
\n\
Trains a new machine for multi-class classification. If the\n\
number of classes in data is 2, then the assigned labels will\n\
be -1 and +1. If the number of classes is greater than 2, labels\n\
are picked starting from 1 (i.e., 1, 2, 3, 4, etc.).\n\
\n\
The input object ``data`` must be an iterable object (such\n\
as a Python list or tuple) containing 2D 64-bit float arrays\n\
each representing data for one single class. The data in each\n\
array should be organized row-wise (i.e. 1 row represents 1\n\
sample). All rows for all arrays should have exactly the same\n\
number of columns - this will be checked.\n\
\n\
Optionally, you may also provide **both** input arrays\n\
``subtract`` and ``divide``, which will be used to normalize\n\
the input data **before** it is fed into the training code.\n\
If provided, both arrays should be 1D and contain 64-bit\n\
floats with the same width as all data in the input array\n\
``data``. The normalization is applied in the following way:\n\
\n\
.. math::\n\
\n\
   d' = \\frac{d-\\text{subtract}}{\\text{divide}}\n\
\n\
");

static PyObject* PyBobLearnLibsvmTrainer_train
(PyBobLearnLibsvmTrainerObject* self, PyObject* args, PyObject* kwds) {

  static const char* const_kwlist[] = {"data", "subtract", "divide", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyObject* X = 0;
  PyBlitzArrayObject* subtract = 0;
  PyBlitzArrayObject* divide = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|O&O&", kwlist,
        &X,
        &PyBlitzArray_OutputConverter, &subtract,
        &PyBlitzArray_OutputConverter, &divide
        )) return 0;

  //protects acquired resources through this scope
  auto X_ = make_safe(X);
  auto subtract_ = make_xsafe(subtract);
  auto divide_ = make_xsafe(divide);

  /**
  // Note: strangely, if you pass dict.values(), this check does not work
  if (!PyIter_Check(X)) {
    PyErr_Format(PyExc_TypeError, "`%s' requires an iterable for parameter `X', but you passed `%s' which does not implement the iterator protocol", Py_TYPE(self)->tp_name, Py_TYPE(X)->tp_name);
    return 0;
  }
  **/

  /* Checks and converts all entries */
  std::vector<blitz::Array<double,2> > Xseq;
  std::vector<boost::shared_ptr<PyBlitzArrayObject>> Xseq_;

  PyObject* iterator = PyObject_GetIter(X);
  if (!iterator) return 0;
  auto iterator_ = make_safe(iterator);

  while (PyObject* item = PyIter_Next(iterator)) {
    auto item_ = make_safe(item);

    PyBlitzArrayObject* bz = 0;

    if (!PyBlitzArray_Converter(item, &bz)) {
      PyErr_Format(PyExc_TypeError, "`%s' could not convert object of type `%s' at position %" PY_FORMAT_SIZE_T "d of input sequence `X' into an array - check your input", Py_TYPE(self)->tp_name, Py_TYPE(item)->tp_name, Xseq.size());
      return 0;
    }

    if (bz->ndim != 2 || bz->type_num != NPY_FLOAT64) {
      PyErr_Format(PyExc_TypeError, "`%s' only supports 2D 64-bit float arrays for input sequence `X' (or any other object coercible to that), but at position %" PY_FORMAT_SIZE_T "d I have found an object with %" PY_FORMAT_SIZE_T "d dimensions and with type `%s' which is not compatible - check your input", Py_TYPE(self)->tp_name, Xseq.size(), bz->ndim, PyBlitzArray_TypenumAsString(bz->type_num));
      Py_DECREF(bz);
      return 0;
    }

    Xseq_.push_back(make_safe(bz)); ///< prevents data deletion
    Xseq.push_back(*PyBlitzArrayCxx_AsBlitz<double,2>(bz)); ///< only a view!
  }

  if (PyErr_Occurred()) return 0;

  if (Xseq.size() < 2) {
    PyErr_Format(PyExc_RuntimeError, "`%s' requires an iterable for parameter `X' leading to, at least, two entries (representing two classes), but you have passed something that has only %" PY_FORMAT_SIZE_T "d entries", Py_TYPE(self)->tp_name, Xseq.size());
    return 0;
  }

  if (subtract && !divide) {
    PyErr_Format(PyExc_RuntimeError, "`%s' requires you provide both `subtract' and `divide' or neither, but you provided only `subtract'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (divide && !subtract) {
    PyErr_Format(PyExc_RuntimeError, "`%s' requires you provide both `subtract' and `divide' or neither, but you provided only `divide'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (subtract && subtract->type_num != NPY_FLOAT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit float arrays for input array `subtract'", Py_TYPE(self)->tp_name);
  }

  if (divide && divide->type_num != NPY_FLOAT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit float arrays for input array `divide'", Py_TYPE(self)->tp_name);
  }

  /** all basic checks are done, can call the machine now **/
  try {
    if (subtract && divide) {
      auto machine = self->cxx->train(Xseq,
          *PyBlitzArrayCxx_AsBlitz<double,1>(subtract),
          *PyBlitzArrayCxx_AsBlitz<double,1>(divide)
          );
      return PyBobLearnLibsvmMachine_NewFromMachine(machine.release());
    }
    else {
      auto machine = self->cxx->train(Xseq);
      return PyBobLearnLibsvmMachine_NewFromMachine(machine.release());
    }
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot train: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }

  PyErr_Format(PyExc_RuntimeError, "%s: unexpected return condition - DEBUG ME", Py_TYPE(self)->tp_name);
  return 0;
}

static PyMethodDef PyBobLearnLibsvmTrainer_methods[] = {
  {
    s_train_str,
    (PyCFunction)PyBobLearnLibsvmTrainer_train,
    METH_VARARGS|METH_KEYWORDS,
    s_train_doc
  },
  {0} /* Sentinel */
};

static PyObject* PyBobLearnLibsvmTrainer_new
(PyTypeObject* type, PyObject*, PyObject*) {

  /* Allocates the python object itself */
  PyBobLearnLibsvmTrainerObject* self =
    (PyBobLearnLibsvmTrainerObject*)type->tp_alloc(type, 0);

  self->cxx = 0;

  return reinterpret_cast<PyObject*>(self);

}

PyTypeObject PyBobLearnLibsvmTrainer_Type = {
    PyVarObject_HEAD_INIT(0, 0)
    s_svm_str,                                        /* tp_name */
    sizeof(PyBobLearnLibsvmTrainerObject),            /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)PyBobLearnLibsvmTrainer_delete,       /* tp_dealloc */
    0,                                                /* tp_print */
    0,                                                /* tp_getattr */
    0,                                                /* tp_setattr */
    0,                                                /* tp_compare */
    (reprfunc)PyBobLearnLibsvmTrainer_Repr,           /* tp_repr */
    0,                                                /* tp_as_number */
    0,                                                /* tp_as_sequence */
    0,                                                /* tp_as_mapping */
    0,                                                /* tp_hash */
    (ternaryfunc)PyBobLearnLibsvmTrainer_train,       /* tp_call */
    (reprfunc)PyBobLearnLibsvmTrainer_Str,            /* tp_str */
    0,                                                /* tp_getattro */
    0,                                                /* tp_setattro */
    0,                                                /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         /* tp_flags */
    s_svm_doc,                                        /* tp_doc */
    0,                                                /* tp_traverse */
    0,                                                /* tp_clear */
    0,                                                /* tp_richcompare */
    0,                                                /* tp_weaklistoffset */
    0,                                                /* tp_iter */
    0,                                                /* tp_iternext */
    PyBobLearnLibsvmTrainer_methods,                  /* tp_methods */
    0,                                                /* tp_members */
    PyBobLearnLibsvmTrainer_getseters,                /* tp_getset */
    0,                                                /* tp_base */
    0,                                                /* tp_dict */
    0,                                                /* tp_descr_get */
    0,                                                /* tp_descr_set */
    0,                                                /* tp_dictoffset */
    (initproc)PyBobLearnLibsvmTrainer_init,           /* tp_init */
    0,                                                /* tp_alloc */
    PyBobLearnLibsvmTrainer_new,                      /* tp_new */
};
