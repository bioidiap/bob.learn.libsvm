/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @Thu 27 Mar 2014 15:44:46 CET
 *
 * @brief Bindings for a Bob compatible LIBSVM-based Machine for SVMs
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
 * Implementation of Support Vector Machine base class *
 *******************************************************/

PyDoc_STRVAR(s_svm_str, XBOB_EXT_MODULE_PREFIX ".Machine");

PyDoc_STRVAR(s_svm_doc,
"Machine(path)\n\
\n\
Machine(hdf5file)\n\
\n\
This class can load and run an SVM generated by libsvm.\n\
Libsvm is a simple, easy-to-use, and efficient software\n\
for SVM classification and regression. It solves C-SVM\n\
classification, nu-SVM classification, one-class-SVM,\n\
epsilon-SVM regression, and nu-SVM regression. It also provides\n\
an automatic model selection tool for C-SVM classification.\n\
More information about libsvm can be found on its `website\n\
<http://www.csie.ntu.edu.tw/~cjlin/libsvm/>`_. In particular,\n\
this class covers most of the functionality provided by the\n\
command-line utility svm-predict.\n\
\n\
Input and output is always performed on 1D or 2D arrays with\n\
64-bit floating point numbers.\n\
\n\
This machine can be initialized in two ways: the first is using\n\
an original SVM text file as produced by ``libsvm``. The\n\
second option is to pass a pre-opened HDF5 file pointing to the\n\
machine information to be loaded in memory.\n\
\n\
Using the first constructor, we build a new machine from a\n\
libsvm model file. When you load using the libsvm model loader,\n\
note that the scaling parameters will be set to defaults\n\
(subtraction of 0.0 and division by 1.0). If you need scaling\n\
to be applied, set it individually using the appropriate methods\n\
on the returned object.\n\
\n\
Using the second constructor, we build a new machine from an\n\
HDF5 file containing not only the machine support vectors, but\n\
also the scaling factors. Using this constructor assures a 100%\n\
state recovery from previous sessions.\n\
\n\
");

/***********************************************
 * Implementation of xbob.learn.libsvm.Machine *
 ***********************************************/

static int PyBobLearnLibsvmMachine_init_svmfile
(PyBobLearnLibsvmMachineObject* self, PyObject* args, PyObject* kwds) {

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {"filename", 0};
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
    self->cxx = new bob::learn::libsvm::Machine(c_filename);
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

static int PyBobLearnLibsvmMachine_init_hdf5
(PyBobLearnLibsvmMachineObject* self, PyObject* args, PyObject* kwds) {

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {"config", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyObject* config = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", kwlist,
        &PyBobIoHDF5File_Type, &config)) return -1;

  auto h5f = reinterpret_cast<PyBobIoHDF5FileObject*>(config);

  try {
    self->cxx = new bob::learn::libsvm::Machine(*(h5f->f));
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

static int PyBobLearnLibsvmMachine_init(PyBobLearnLibsvmMachineObject* self,
    PyObject* args, PyObject* kwds) {

  Py_ssize_t nargs = (args?PyTuple_Size(args):0) + (kwds?PyDict_Size(kwds):0);

  if (nargs != 1) {
    PyErr_Format(PyExc_RuntimeError, "number of arguments mismatch - %s requires 1 arguments, but you provided %" PY_FORMAT_SIZE_T "d (see help)", Py_TYPE(self)->tp_name, nargs);
    return -1;
  }

  PyObject* arg = 0; ///< borrowed (don't delete)
  if (PyTuple_Size(args)) arg = PyTuple_GET_ITEM(args, 0);
  else {
    PyObject* tmp = PyDict_Values(kwds);
    auto tmp_ = make_safe(tmp);
    arg = PyList_GET_ITEM(tmp, 0);
  }

  if (PyBobIoHDF5File_Check(arg)) {
    return PyBobLearnLibsvmMachine_init_hdf5(self, args, kwds);
  }
  else {
    return PyBobLearnLibsvmMachine_init_svmfile(self, args, kwds);
  }

}

static void PyBobLearnLibsvmMachine_delete
(PyBobLearnLibsvmMachineObject* self) {

  delete self->cxx;
  Py_TYPE(self)->tp_free((PyObject*)self);

}

int PyBobLearnLibsvmMachine_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobLearnLibsvmMachine_Type));
}

PyDoc_STRVAR(s_input_subtract_str, "input_subtract");
PyDoc_STRVAR(s_input_subtract_doc,
"Input subtraction factor, before feeding data through the\n\
weight matrix W. The subtraction is the first applied\n\
operation in the processing chain - by default, it is set to\n\
0.0.\n\
");

static PyObject* PyBobLearnLibsvmMachine_getInputSubtraction
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  return PyBlitzArray_NUMPY_WRAP(PyBlitzArrayCxx_NewFromConstArray(self->cxx->getInputSubtraction()));
}

static int PyBobLearnLibsvmMachine_setInputSubtraction
(PyBobLearnLibsvmMachineObject* self, PyObject* o, void* /*closure*/) {

  PyBlitzArrayObject* input_subtract = 0;
  if (!PyBlitzArray_Converter(o, &input_subtract)) return -1;
  auto input_subtract_ = make_safe(input_subtract);

  if (input_subtract->type_num != NPY_FLOAT64 || input_subtract->ndim != 1) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit floats 1D arrays for property array `input_subtract'", Py_TYPE(self)->tp_name);
    return -1;
  }

  try {
    self->cxx->setInputSubtraction(*PyBlitzArrayCxx_AsBlitz<double,1>(input_subtract));
  }
  catch (std::exception& ex) {
    PyErr_SetString(PyExc_RuntimeError, ex.what());
    return -1;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "cannot reset `input_subtract' of %s: unknown exception caught", Py_TYPE(self)->tp_name);
    return -1;
  }

  return 0;

}

PyDoc_STRVAR(s_input_divide_str, "input_divide");
PyDoc_STRVAR(s_input_divide_doc,
"Input division factor, before feeding data through the\n\
weight matrix W. The division is applied just after\n\
subtraction - by default, it is set to 1.0.\n\
");

static PyObject* PyBobLearnLibsvmMachine_getInputDivision
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  return PyBlitzArray_NUMPY_WRAP(PyBlitzArrayCxx_NewFromConstArray(self->cxx->getInputDivision()));
}

static int PyBobLearnLibsvmMachine_setInputDivision
(PyBobLearnLibsvmMachineObject* self, PyObject* o, void* /*closure*/) {

  PyBlitzArrayObject* input_divide = 0;
  if (!PyBlitzArray_Converter(o, &input_divide)) return -1;
  auto input_divide_ = make_safe(input_divide);

  if (input_divide->type_num != NPY_FLOAT64 || input_divide->ndim != 1) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit floats 1D arrays for property array `input_divide'", Py_TYPE(self)->tp_name);
    return -1;
  }

  try {
    self->cxx->setInputDivision(*PyBlitzArrayCxx_AsBlitz<double,1>(input_divide));
  }
  catch (std::exception& ex) {
    PyErr_SetString(PyExc_RuntimeError, ex.what());
    return -1;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "cannot reset `input_divide' of %s: unknown exception caught", Py_TYPE(self)->tp_name);
    return -1;
  }

  return 0;

}

PyDoc_STRVAR(s_shape_str, "shape");
PyDoc_STRVAR(s_shape_doc,
"A tuple that represents the size of the input vector\n\
followed by the size of the output vector in the format\n\
``(input, output)``.\n\
");

static PyObject* PyBobLearnLibsvmMachine_getShape
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  return Py_BuildValue("(nn)", self->cxx->inputSize(),
      self->cxx->outputSize());
}

PyDoc_STRVAR(s_labels_str, "labels");
PyDoc_STRVAR(s_labels_doc, "The class labels this machine will output");

static PyObject* PyBobLearnLibsvmMachine_getLabels
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  PyObject* retval = PyList_New(self->cxx->numberOfClasses());
  for (size_t k=0; k<self->cxx->numberOfClasses(); ++k) {
    PyList_SET_ITEM(retval, k, Py_BuildValue("i", self->cxx->classLabel(k)));
  }
  return retval;
}

PyDoc_STRVAR(s_machine_type_str, "machine_type");
PyDoc_STRVAR(s_machine_type_doc, "The type of SVM machine contained");

static PyObject* PyBobLearnLibsvmMachine_getMachineType
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  return PyBobLearnLibsvm_MachineTypeAsString(self->cxx->machineType());
}

PyDoc_STRVAR(s_svm_kernel_type_str, "kernel_type");
PyDoc_STRVAR(s_svm_kernel_type_doc,
"The type of kernel used by the support vectors in this machine");

static PyObject* PyBobLearnLibsvmMachine_getKernelType
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  return PyBobLearnLibsvm_KernelTypeAsString(self->cxx->kernelType());
}

PyDoc_STRVAR(s_degree_str, "degree");
PyDoc_STRVAR(s_degree_doc,
"The polinomial degree, only valid if the kernel is ``'POLY'``\n\
(polynomial)");

static PyObject* PyBobLearnLibsvmMachine_getPolynomialDegree
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  return Py_BuildValue("i", self->cxx->polynomialDegree());
}

PyDoc_STRVAR(s_gamma_str, "gamma");
PyDoc_STRVAR(s_gamma_doc,
"The :math:`\\gamma` parameter for ``'POLY'`` (polynomial),\n\
``'RBF'`` (gaussian) or ``'SIGMOID'`` (sigmoidal) kernels");

static PyObject* PyBobLearnLibsvmMachine_getGamma
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  return Py_BuildValue("d", self->cxx->gamma());
}

PyDoc_STRVAR(s_coef0_str, "coef0");
PyDoc_STRVAR(s_coef0_doc,
"The coefficient 0 for ``'POLY'`` (polynomial) or\n\
``'SIGMOIDAL'`` (sigmoidal) kernels");

static PyObject* PyBobLearnLibsvmMachine_getCoefficient0
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  return Py_BuildValue("d", self->cxx->coefficient0());
}

PyDoc_STRVAR(s_probability_str, "probability");
PyDoc_STRVAR(s_probability_doc,
"Set to ``True`` if this machine supports probability outputs");

static PyObject* PyBobLearnLibsvmMachine_getSupportsProbability
(PyBobLearnLibsvmMachineObject* self, void* /*closure*/) {
  if (self->cxx->supportsProbability()) Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

static PyGetSetDef PyBobLearnLibsvmMachine_getseters[] = {
    {
      s_input_subtract_str,
      (getter)PyBobLearnLibsvmMachine_getInputSubtraction,
      (setter)PyBobLearnLibsvmMachine_setInputSubtraction,
      s_input_subtract_doc,
      0
    },
    {
      s_input_divide_str,
      (getter)PyBobLearnLibsvmMachine_getInputDivision,
      (setter)PyBobLearnLibsvmMachine_setInputDivision,
      s_input_divide_doc,
      0
    },
    {
      s_shape_str,
      (getter)PyBobLearnLibsvmMachine_getShape,
      0,
      s_shape_doc,
      0
    },
    {
      s_labels_str,
      (getter)PyBobLearnLibsvmMachine_getLabels,
      0,
      s_labels_doc,
      0
    },
    {
      s_machine_type_str,
      (getter)PyBobLearnLibsvmMachine_getMachineType,
      0,
      s_machine_type_doc,
      0
    },
    {
      s_svm_kernel_type_str,
      (getter)PyBobLearnLibsvmMachine_getKernelType,
      0,
      s_svm_kernel_type_doc,
      0
    },
    {
      s_degree_str,
      (getter)PyBobLearnLibsvmMachine_getPolynomialDegree,
      0,
      s_degree_doc,
      0
    },
    {
      s_gamma_str,
      (getter)PyBobLearnLibsvmMachine_getGamma,
      0,
      s_gamma_doc,
      0
    },
    {
      s_coef0_str,
      (getter)PyBobLearnLibsvmMachine_getCoefficient0,
      0,
      s_coef0_doc,
      0
    },
    {
      s_probability_str,
      (getter)PyBobLearnLibsvmMachine_getSupportsProbability,
      0,
      s_probability_doc,
      0
    },
    {0}  /* Sentinel */
};

#if PY_VERSION_HEX >= 0x03000000
#  define PYOBJECT_STR PyObject_Str
#else
#  define PYOBJECT_STR PyObject_Unicode
#endif

PyObject* PyBobLearnLibsvmMachine_Repr(PyBobLearnLibsvmMachineObject* self) {

  /**
   * Expected output:
   *
   * <xbob.learn.libsvm.Machine float64@(3, 2)>
   */

  auto shape = make_safe(PyObject_GetAttrString((PyObject*)self, "shape"));
  auto shape_str = make_safe(PyObject_Str(shape.get()));

  PyObject* retval = PyUnicode_FromFormat("<%s float64@%U>",
      Py_TYPE(self)->tp_name, shape_str.get());

#if PYTHON_VERSION_HEX < 0x03000000
  if (!retval) return 0;
  PyObject* tmp = PyObject_Str(retval);
  Py_DECREF(retval);
  retval = tmp;
#endif

  return retval;

}

PyObject* PyBobLearnLibsvmMachine_Str(PyBobLearnLibsvmMachineObject* self) {

  /**
   * Expected output:
   *
   * xbob.learn.libsvm.Machine (float64) 3 inputs, 2 outputs
   *   properties...
   *
   * TODO: Not fully implemented yet
   */

  return PyBobLearnLibsvmMachine_Repr(self);

}

PyDoc_STRVAR(s_forward_str, "forward");
PyDoc_STRVAR(s_forward_doc,
"o.forward(input, [output]) -> array\n\
\n\
o.predict_class(input, [output]) -> array\n\
\n\
o(input, [output]) -> array\n\
\n\
Calculates the **predicted class** using this Machine, given\n\
one single feature vector or multiple ones.\n\
\n\
The ``input`` array can be either 1D or 2D 64-bit float arrays.\n\
The ``output`` array, if provided, must be of type ``int64``,\n\
always uni-dimensional. The output corresponds to the predicted\n\
classes for each of the input rows.\n\
\n\
.. note::\n\
\n\
   This method only accepts 64-bit float arrays as input and\n\
   64-bit integers as output.\n\
\n");

static PyObject* PyBobLearnLibsvmMachine_forward
(PyBobLearnLibsvmMachineObject* self, PyObject* args, PyObject* kwds) {

  static const char* const_kwlist[] = {"input", "output", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyBlitzArrayObject* input = 0;
  PyBlitzArrayObject* output = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&|O&", kwlist,
        &PyBlitzArray_Converter, &input,
        &PyBlitzArray_OutputConverter, &output
        )) return 0;

  //protects acquired resources through this scope
  auto input_ = make_safe(input);
  auto output_ = make_xsafe(output);

  if (input->type_num != NPY_FLOAT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit float arrays for input array `input'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (output && output->type_num != NPY_INT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit integer arrays for output array `output'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (input->ndim < 1 || input->ndim > 2) {
    PyErr_Format(PyExc_TypeError, "`%s' only accepts 1 or 2-dimensional arrays (not %" PY_FORMAT_SIZE_T "dD arrays)", Py_TYPE(self)->tp_name, input->ndim);
    return 0;
  }

  if (output && output->ndim != 1) {
    PyErr_Format(PyExc_RuntimeError, "Output arrays should always be 1D but you provided an object with %" PY_FORMAT_SIZE_T "d dimensions", output->ndim);
    return 0;
  }

  if (input->ndim == 1) {
    if (input->shape[0] != (Py_ssize_t)self->cxx->inputSize()) {
      PyErr_Format(PyExc_RuntimeError, "1D `input' array should have %" PY_FORMAT_SIZE_T "d elements matching `%s' input size, not %" PY_FORMAT_SIZE_T "d elements", self->cxx->inputSize(), Py_TYPE(self)->tp_name, input->shape[0]);
      return 0;
    }
    if (output && output->shape[0] != 1) {
      PyErr_Format(PyExc_RuntimeError, "1D `output' array should have 1 element, not %" PY_FORMAT_SIZE_T "d elements", output->shape[0]);
      return 0;
    }
  }
  else {
    if (input->shape[1] != (Py_ssize_t)self->cxx->inputSize()) {
      PyErr_Format(PyExc_RuntimeError, "2D `input' array should have %" PY_FORMAT_SIZE_T "d columns, matching `%s' input size, not %" PY_FORMAT_SIZE_T "d", self->cxx->inputSize(), Py_TYPE(self)->tp_name, input->shape[1]);
      return 0;
    }
    if (output && input->shape[0] != output->shape[0]) {
      PyErr_Format(PyExc_RuntimeError, "1D `output' array should have %" PY_FORMAT_SIZE_T "d elements matching the number of rows on `input', not %" PY_FORMAT_SIZE_T "d rows", input->shape[0], output->shape[0]);
      return 0;
    }
  }

  /** if ``output`` was not pre-allocated, do it now **/
  if (!output) {
    Py_ssize_t osize = 1;
    if (input->ndim == 2) osize = input->shape[0];
    output = (PyBlitzArrayObject*)PyBlitzArray_SimpleNew(NPY_INT64, 1, &osize);
    output_ = make_safe(output);
  }

  /** all basic checks are done, can call the machine now **/
  try {
    if (input->ndim == 1) {
      auto bzout = PyBlitzArrayCxx_AsBlitz<int64_t,1>(output);
      (*bzout)(0) = self->cxx->predictClass_(*PyBlitzArrayCxx_AsBlitz<double,1>(input));
    }
    else {
      auto bzin = PyBlitzArrayCxx_AsBlitz<double,2>(input);
      auto bzout = PyBlitzArrayCxx_AsBlitz<int64_t,1>(output);
      blitz::Range all = blitz::Range::all();
      for (int k=0; k<bzin->extent(0); ++k) {
        blitz::Array<double,1> i_ = (*bzin)(k, all);
        (*bzout)(k) = self->cxx->predictClass_(i_); ///< no need to re-check
      }
    }
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot forward data: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }

  Py_INCREF(output);
  return PyBlitzArray_NUMPY_WRAP(reinterpret_cast<PyObject*>(output));

}

PyDoc_STRVAR(s_scores_str, "predict_class_and_scores");
PyDoc_STRVAR(s_scores_doc,
"o.predict_class_and_scores(input, [cls, [score]]) -> (array, array)\n\
\n\
Calculates the **predicted class** and output scores for the SVM\n\
using the this Machine, given one single feature vector or multiple\n\
ones.\n\
\n\
The ``input`` array can be either 1D or 2D 64-bit float arrays.\n\
The ``cls`` array, if provided, must be of type ``int64``,\n\
always uni-dimensional. The ``cls`` output corresponds to the\n\
predicted classes for each of the input rows. The ``score`` array,\n\
if provided, must be of type ``float64`` (like ``input``) and have\n\
as many rows as ``input`` and ``C`` columns, matching the \n\
number of combinations of the outputs 2-by-2. To score, LIBSVM\n\
will compare the SV outputs for each set two classes in the machine\n\
and output 1 score. If there is only 1 output, then the problem is\n\
binary and only 1 score is produced (``C = 1``). If the SVM is\n\
multi-class, then the number of combinations ``C`` is the total\n\
amount of output combinations which is possible. If ``N`` is\n\
the number of classes in this SVM, then :math:`C = N\\cdot(N-1)/2`.\n\
If ``N = 3``, then ``C = 3``. If ``N = 5``, then ``C = 10``.\n\
\n\
This method always returns a tuple composed of the predicted classes\n\
for each row in the ``input`` array, with data type ``int64`` and\n\
of scores for each output of the SVM in a 1D or 2D ``float64`` array.\n\
If you don't provide the arrays upon calling this method, we will\n\
allocate new ones internally and return them. If you are calling\n\
this method on a tight loop, it is recommended you pass the ``cls``\n\
and ``score`` arrays to avoid constant re-allocation.\n\
");

static PyObject* PyBobLearnLibsvmMachine_predictClassAndScores
(PyBobLearnLibsvmMachineObject* self, PyObject* args, PyObject* kwds) {

  static const char* const_kwlist[] = {"input", "cls", "score", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyBlitzArrayObject* input = 0;
  PyBlitzArrayObject* cls = 0;
  PyBlitzArrayObject* score = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&|O&O&", kwlist,
        &PyBlitzArray_Converter, &input,
        &PyBlitzArray_OutputConverter, &cls,
        &PyBlitzArray_OutputConverter, &score
        )) return 0;

  //protects acquired resources through this scope
  auto input_ = make_safe(input);
  auto cls_ = make_xsafe(cls);
  auto score_ = make_xsafe(score);

  //calculates the number of scores expected: combinatorics between
  //all class outputs
  Py_ssize_t N = self->cxx->outputSize();
  Py_ssize_t number_of_scores = N < 2 ? 1 : (N*(N-1))/2;

  if (input->type_num != NPY_FLOAT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit float arrays for input array `input'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (cls && cls->type_num != NPY_INT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit integer arrays for output array `cls'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (score && score->type_num != NPY_FLOAT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit float arrays for score array `score'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (input->ndim < 1 || input->ndim > 2) {
    PyErr_Format(PyExc_TypeError, "`%s' only accepts 1 or 2-dimensional arrays (not %" PY_FORMAT_SIZE_T "dD arrays)", Py_TYPE(self)->tp_name, input->ndim);
    return 0;
  }

  if (cls && cls->ndim != 1) {
    PyErr_Format(PyExc_RuntimeError, "the `cls' array should always be 1D but you provided an object with %" PY_FORMAT_SIZE_T "d dimensions", cls->ndim);
    return 0;
  }

  if (score && input->ndim != score->ndim) {
    PyErr_Format(PyExc_RuntimeError, "Input and score arrays should have matching number of dimensions, but input array `input' has %" PY_FORMAT_SIZE_T "d dimensions while output array `score' has %" PY_FORMAT_SIZE_T "d dimensions", input->ndim, score->ndim);
    return 0;
  }

  if (input->ndim == 1) {
    if (input->shape[0] != (Py_ssize_t)self->cxx->inputSize()) {
      PyErr_Format(PyExc_RuntimeError, "1D `input' array should have %" PY_FORMAT_SIZE_T "d elements matching `%s' input size, not %" PY_FORMAT_SIZE_T "d elements", self->cxx->inputSize(), Py_TYPE(self)->tp_name, input->shape[0]);
      return 0;
    }
    if (cls && cls->shape[0] != 1) {
      PyErr_Format(PyExc_RuntimeError, "1D `cls' array should have 1 element, not %" PY_FORMAT_SIZE_T "d elements", cls->shape[0]);
      return 0;
    }
    if (score && score->shape[0] != number_of_scores) {
      PyErr_Format(PyExc_RuntimeError, "1D `score' array should have %" PY_FORMAT_SIZE_T "d elements matching the expected number of scores for `%s', not %" PY_FORMAT_SIZE_T "d elements", number_of_scores, Py_TYPE(self)->tp_name, score->shape[0]);
      return 0;
    }
  }
  else {
    if (input->shape[1] != (Py_ssize_t)self->cxx->inputSize()) {
      PyErr_Format(PyExc_RuntimeError, "2D `input' array should have %" PY_FORMAT_SIZE_T "d columns, matching `%s' input size, not %" PY_FORMAT_SIZE_T "d elements", self->cxx->inputSize(), Py_TYPE(self)->tp_name, input->shape[1]);
      return 0;
    }
    if (cls && input->shape[0] != cls->shape[0]) {
      PyErr_Format(PyExc_RuntimeError, "1D `cls' array should have %" PY_FORMAT_SIZE_T "d elements matching the number of rows on `input', not %" PY_FORMAT_SIZE_T "d rows", input->shape[0], cls->shape[0]);
      return 0;
    }
    if (score && score->shape[1] != number_of_scores) {
      PyErr_Format(PyExc_RuntimeError, "2D `score' array should have %" PY_FORMAT_SIZE_T "d columns matching the output size of `%s', not %" PY_FORMAT_SIZE_T "d elements", number_of_scores, Py_TYPE(self)->tp_name, score->shape[1]);
      return 0;
    }
    if (score && input->shape[0] != score->shape[0]) {
      PyErr_Format(PyExc_RuntimeError, "2D `score' array should have %" PY_FORMAT_SIZE_T "d rows matching `input' size, not %" PY_FORMAT_SIZE_T "d rows", input->shape[0], score->shape[0]);
      return 0;
    }
  }

  /** if ``cls`` was not pre-allocated, do it now **/
  if (!cls) {
    Py_ssize_t osize = 1;
    if (input->ndim == 2) osize = input->shape[0];
    cls = (PyBlitzArrayObject*)PyBlitzArray_SimpleNew(NPY_INT64, 1, &osize);
    cls_ = make_safe(cls);
  }

  /** if ``score`` was not pre-allocated, do it now **/
  if (!score) {
    Py_ssize_t osize[2];
    if (input->ndim == 1) {
      osize[0] = number_of_scores;
    }
    else {
      osize[0] = input->shape[0];
      osize[1] = number_of_scores;
    }
    score = (PyBlitzArrayObject*)PyBlitzArray_SimpleNew(NPY_FLOAT64, input->ndim, osize);
    score_ = make_safe(score);
  }

  /** all basic checks are done, can call the machine now **/
  try {
    if (input->ndim == 1) {
      auto bzin = PyBlitzArrayCxx_AsBlitz<double,1>(input);
      auto bzcls = PyBlitzArrayCxx_AsBlitz<int64_t,1>(cls);
      auto bzscore = PyBlitzArrayCxx_AsBlitz<double,1>(score);
      (*bzcls)(0) = self->cxx->predictClassAndScores_(*bzin, *bzscore);
    }
    else {
      auto bzin = PyBlitzArrayCxx_AsBlitz<double,2>(input);
      auto bzcls = PyBlitzArrayCxx_AsBlitz<int64_t,1>(cls);
      auto bzscore = PyBlitzArrayCxx_AsBlitz<double,2>(score);
      blitz::Range all = blitz::Range::all();
      for (int k=0; k<bzin->extent(0); ++k) {
        blitz::Array<double,1> i_ = (*bzin)(k, all);
        blitz::Array<double,1> s_ = (*bzscore)(k, all);
        (*bzcls)(k) = self->cxx->predictClassAndScores_(i_, s_);
      }
    }
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot forward data: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }

  Py_INCREF(cls);
  Py_INCREF(score);
  return Py_BuildValue("OO",
      PyBlitzArray_NUMPY_WRAP(reinterpret_cast<PyObject*>(cls)),
      PyBlitzArray_NUMPY_WRAP(reinterpret_cast<PyObject*>(score))
      );

}

PyDoc_STRVAR(s_probabilities_str, "predict_class_and_probabilities");
PyDoc_STRVAR(s_probabilities_doc,
"o.predict_class_and_probabilities(input, [cls, [prob]]) -> (array, array)\n\
\n\
Calculates the **predicted class** and output probabilities for the\n\
SVM using the this Machine, given one single feature vector or\n\
multiple ones.\n\
\n\
The ``input`` array can be either 1D or 2D 64-bit float arrays.\n\
The ``cls`` array, if provided, must be of type ``int64``,\n\
always uni-dimensional. The ``cls`` output corresponds to the\n\
predicted classes for each of the input rows. The ``prob`` array,\n\
if provided, must be of type ``float64`` (like ``input``) and have\n\
as many rows as ``input`` and ``len(o.labels)`` columns, matching\n\
the number of classes for this SVM.\n\
\n\
This method always returns a tuple composed of the predicted classes\n\
for each row in the ``input`` array, with data type ``int64`` and\n\
of probabilities for each output of the SVM in a 1D or 2D ``float64``\n\
array. If you don't provide the arrays upon calling this method, we\n\
will allocate new ones internally and return them. If you are calling\n\
this method on a tight loop, it is recommended you pass the ``cls``\n\
and ``prob`` arrays to avoid constant re-allocation.\n\
");

static PyObject* PyBobLearnLibsvmMachine_predictClassAndProbabilities
(PyBobLearnLibsvmMachineObject* self, PyObject* args, PyObject* kwds) {

  if (!self->cxx->supportsProbability()) {
    PyErr_Format(PyExc_RuntimeError, "`%s' object does not support probabilities - in the future, use `o.probability' to query for this property", Py_TYPE(self)->tp_name);
    return 0;
  }

  static const char* const_kwlist[] = {"input", "cls", "prob", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyBlitzArrayObject* input = 0;
  PyBlitzArrayObject* cls = 0;
  PyBlitzArrayObject* prob= 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&|O&O&", kwlist,
        &PyBlitzArray_Converter, &input,
        &PyBlitzArray_OutputConverter, &cls,
        &PyBlitzArray_OutputConverter, &prob
        )) return 0;

  //protects acquired resources through this scope
  auto input_ = make_safe(input);
  auto cls_ = make_xsafe(cls);
  auto prob_ = make_xsafe(prob);

  if (input->type_num != NPY_FLOAT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit float arrays for input array `input'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (cls && cls->type_num != NPY_INT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit integer arrays for output array `cls'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (prob && prob->type_num != NPY_FLOAT64) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 64-bit float arrays for probability array `prob'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (input->ndim < 1 || input->ndim > 2) {
    PyErr_Format(PyExc_TypeError, "`%s' only accepts 1 or 2-dimensional arrays (not %" PY_FORMAT_SIZE_T "dD arrays)", Py_TYPE(self)->tp_name, input->ndim);
    return 0;
  }

  if (cls && cls->ndim != 1) {
    PyErr_Format(PyExc_RuntimeError, "the `cls' array should always be 1D but you provided an object with %" PY_FORMAT_SIZE_T "d dimensions", cls->ndim);
    return 0;
  }

  if (prob && input->ndim != prob->ndim) {
    PyErr_Format(PyExc_RuntimeError, "Input and probability arrays should have matching number of dimensions, but input array `input' has %" PY_FORMAT_SIZE_T "d dimensions while output array `prob' has %" PY_FORMAT_SIZE_T "d dimensions", input->ndim, prob->ndim);
    return 0;
  }

  if (input->ndim == 1) {
    if (input->shape[0] != (Py_ssize_t)self->cxx->inputSize()) {
      PyErr_Format(PyExc_RuntimeError, "1D `input' array should have %" PY_FORMAT_SIZE_T "d elements matching `%s' input size, not %" PY_FORMAT_SIZE_T "d elements", self->cxx->inputSize(), Py_TYPE(self)->tp_name, input->shape[0]);
      return 0;
    }
    if (cls && cls->shape[0] != 1) {
      PyErr_Format(PyExc_RuntimeError, "1D `cls' array should have 1 element, not %" PY_FORMAT_SIZE_T "d elements", cls->shape[0]);
      return 0;
    }
    if (prob && prob->shape[0] != (Py_ssize_t)self->cxx->numberOfClasses()) {
      PyErr_Format(PyExc_RuntimeError, "1D `prob' array should have %" PY_FORMAT_SIZE_T "d elements matching the number of classes of `%s', not %" PY_FORMAT_SIZE_T "d elements", self->cxx->numberOfClasses(), Py_TYPE(self)->tp_name, prob->shape[0]);
      return 0;
    }
  }
  else {
    if (input->shape[1] != (Py_ssize_t)self->cxx->inputSize()) {
      PyErr_Format(PyExc_RuntimeError, "2D `input' array should have %" PY_FORMAT_SIZE_T "d columns, matching `%s' input size, not %" PY_FORMAT_SIZE_T "d elements", self->cxx->inputSize(), Py_TYPE(self)->tp_name, input->shape[1]);
      return 0;
    }
    if (cls && input->shape[0] != cls->shape[0]) {
      PyErr_Format(PyExc_RuntimeError, "1D `cls' array should have %" PY_FORMAT_SIZE_T "d elements matching the number of rows on `input', not %" PY_FORMAT_SIZE_T "d rows", input->shape[0], cls->shape[0]);
      return 0;
    }
    if (prob && prob->shape[1] != (Py_ssize_t)self->cxx->numberOfClasses()) {
      PyErr_Format(PyExc_RuntimeError, "2D `prob' array should have %" PY_FORMAT_SIZE_T "d columns matching the number of classes of `%s', not %" PY_FORMAT_SIZE_T "d elements", self->cxx->numberOfClasses(), Py_TYPE(self)->tp_name, prob->shape[1]);
      return 0;
    }
    if (prob && input->shape[0] != prob->shape[0]) {
      PyErr_Format(PyExc_RuntimeError, "2D `prob' array should have %" PY_FORMAT_SIZE_T "d rows matching `input' size, not %" PY_FORMAT_SIZE_T "d rows", input->shape[0], prob->shape[0]);
      return 0;
    }
  }

  /** if ``cls`` was not pre-allocated, do it now **/
  if (!cls) {
    Py_ssize_t osize = 1;
    if (input->ndim == 2) osize = input->shape[0];
    cls = (PyBlitzArrayObject*)PyBlitzArray_SimpleNew(NPY_INT64, 1, &osize);
    cls_ = make_safe(cls);
  }

  /** if ``prob`` was not pre-allocated, do it now **/
  if (!prob) {
    Py_ssize_t osize[2];
    if (input->ndim == 1) {
      osize[0] = self->cxx->numberOfClasses();
    }
    else {
      osize[0] = input->shape[0];
      osize[1] = self->cxx->numberOfClasses();
    }
    prob = (PyBlitzArrayObject*)PyBlitzArray_SimpleNew(NPY_FLOAT64, input->ndim, osize);
    prob_ = make_safe(prob);
  }

  /** all basic checks are done, can call the machine now **/
  try {
    if (input->ndim == 1) {
      auto bzin = PyBlitzArrayCxx_AsBlitz<double,1>(input);
      auto bzcls = PyBlitzArrayCxx_AsBlitz<int64_t,1>(cls);
      auto bzprob = PyBlitzArrayCxx_AsBlitz<double,1>(prob);
      (*bzcls)(0) = self->cxx->predictClassAndProbabilities_(*bzin, *bzprob);
    }
    else {
      auto bzin = PyBlitzArrayCxx_AsBlitz<double,2>(input);
      auto bzcls = PyBlitzArrayCxx_AsBlitz<int64_t,1>(cls);
      auto bzprob = PyBlitzArrayCxx_AsBlitz<double,2>(prob);
      blitz::Range all = blitz::Range::all();
      for (int k=0; k<bzin->extent(0); ++k) {
        blitz::Array<double,1> i_ = (*bzin)(k, all);
        blitz::Array<double,1> p_ = (*bzprob)(k, all);
        (*bzcls)(k) = self->cxx->predictClassAndProbabilities_(i_, p_);
      }
    }
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot forward data: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }

  Py_INCREF(cls);
  Py_INCREF(prob);
  return Py_BuildValue("OO",
      PyBlitzArray_NUMPY_WRAP(reinterpret_cast<PyObject*>(cls)),
      PyBlitzArray_NUMPY_WRAP(reinterpret_cast<PyObject*>(prob))
      );

}

PyDoc_STRVAR(s_save_str, "save");
PyDoc_STRVAR(s_save_doc,
"o.save(path) -> None\n\
\n\
o.save(hdf5file) -> None\n\
\n\
Saves itself at a LIBSVM model file or into a\n\
:py:class:`xbob.io.HDF5File`. Saving the SVM into an\n\
:py:class:`xbob.io.HDF5File` object, has the advantage of saving\n\
input normalization options together with the machine, which are\n\
automatically reloaded when you re-initialize it from the same\n\
:py:class:`HDF5File`.\n\
");

static PyObject* PyBobLearnLibsvmMachine_Save
(PyBobLearnLibsvmMachineObject* self, PyObject* f) {

  if (PyBobIoHDF5File_Check(f)) {
    auto h5f = reinterpret_cast<PyBobIoHDF5FileObject*>(f);
    try {
      self->cxx->save(*h5f->f);
    }
    catch (std::exception& e) {
      PyErr_SetString(PyExc_RuntimeError, e.what());
      return 0;
    }
    catch (...) {
      PyErr_Format(PyExc_RuntimeError, "`%s' cannot write data to file `%s' (at group `%s'): unknown exception caught", Py_TYPE(self)->tp_name,
          h5f->f->filename().c_str(), h5f->f->cwd().c_str());
      return 0;
    }
    Py_RETURN_NONE;
  }

  // try a filename conversion and use libsvm's original file format
  PyObject* filename = 0;
  int ok = PyBobIo_FilenameConverter(f, &filename);
  if (!ok) {
    PyErr_Format(PyExc_TypeError, "cannot convert `%s' into a valid string for a file path - objects of type `%s' can only save to HDF5 files or text files using LIBSVM's original file format (pass a string referring to a valid filesystem path in this case)", Py_TYPE(f)->tp_name, Py_TYPE(self)->tp_name);
    return 0;
  }

  // at this point we know we have a valid file system string
  auto filename_ = make_safe(filename);

#if PY_VERSION_HEX >= 0x03000000
  const char* c_filename = PyBytes_AS_STRING(filename);
#else
  const char* c_filename = PyString_AS_STRING(filename);
#endif

  try {
    self->cxx->save(c_filename);
  }
  catch (std::exception& ex) {
    PyErr_SetString(PyExc_RuntimeError, ex.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "`%s' cannot write data to file `%s' (using LIBSVM's original text format): unknown exception caught", Py_TYPE(self)->tp_name, c_filename);
    return 0;
  }

  Py_RETURN_NONE;

}

PyDoc_STRVAR(s_predict_class_str, "predict_class");

static PyMethodDef PyBobLearnLibsvmMachine_methods[] = {
  {
    s_forward_str,
    (PyCFunction)PyBobLearnLibsvmMachine_forward,
    METH_VARARGS|METH_KEYWORDS,
    s_forward_doc
  },
  {
    s_predict_class_str,
    (PyCFunction)PyBobLearnLibsvmMachine_forward,
    METH_VARARGS|METH_KEYWORDS,
    s_forward_doc
  },
  {
    s_scores_str,
    (PyCFunction)PyBobLearnLibsvmMachine_predictClassAndScores,
    METH_VARARGS|METH_KEYWORDS,
    s_scores_doc,
  },
  {
    s_probabilities_str,
    (PyCFunction)PyBobLearnLibsvmMachine_predictClassAndProbabilities,
    METH_VARARGS|METH_KEYWORDS,
    s_probabilities_doc,
  },
  {
    s_save_str,
    (PyCFunction)PyBobLearnLibsvmMachine_Save,
    METH_O,
    s_save_doc
  },
  {0} /* Sentinel */
};

static PyObject* PyBobLearnLibsvmMachine_new
(PyTypeObject* type, PyObject*, PyObject*) {

  /* Allocates the python object itself */
  PyBobLearnLibsvmMachineObject* self =
    (PyBobLearnLibsvmMachineObject*)type->tp_alloc(type, 0);

  self->cxx = 0;

  return reinterpret_cast<PyObject*>(self);

}

PyObject* PyBobLearnLibsvmMachine_NewFromMachine
(bob::learn::libsvm::Machine* m) {

  PyBobLearnLibsvmMachineObject* retval = (PyBobLearnLibsvmMachineObject*)PyBobLearnLibsvmMachine_new(&PyBobLearnLibsvmMachine_Type, 0, 0);

  retval->cxx = m; ///< takes ownership

  return reinterpret_cast<PyObject*>(retval);

}

PyTypeObject PyBobLearnLibsvmMachine_Type = {
    PyVarObject_HEAD_INIT(0, 0)
    s_svm_str,                                        /* tp_name */
    sizeof(PyBobLearnLibsvmMachineObject),            /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)PyBobLearnLibsvmMachine_delete,       /* tp_dealloc */
    0,                                                /* tp_print */
    0,                                                /* tp_getattr */
    0,                                                /* tp_setattr */
    0,                                                /* tp_compare */
    (reprfunc)PyBobLearnLibsvmMachine_Repr,           /* tp_repr */
    0,                                                /* tp_as_number */
    0,                                                /* tp_as_sequence */
    0,                                                /* tp_as_mapping */
    0,                                                /* tp_hash */
    (ternaryfunc)PyBobLearnLibsvmMachine_forward,     /* tp_call */
    (reprfunc)PyBobLearnLibsvmMachine_Str,            /* tp_str */
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
    PyBobLearnLibsvmMachine_methods,                  /* tp_methods */
    0,                                                /* tp_members */
    PyBobLearnLibsvmMachine_getseters,                /* tp_getset */
    0,                                                /* tp_base */
    0,                                                /* tp_dict */
    0,                                                /* tp_descr_get */
    0,                                                /* tp_descr_set */
    0,                                                /* tp_dictoffset */
    (initproc)PyBobLearnLibsvmMachine_init,           /* tp_init */
    0,                                                /* tp_alloc */
    PyBobLearnLibsvmMachine_new,                      /* tp_new */
};
