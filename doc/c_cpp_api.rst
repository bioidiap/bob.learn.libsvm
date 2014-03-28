.. vim: set fileencoding=utf-8 :
.. Andre Anjos <andre.dos.anjos@gmail.com>
.. Tue 15 Oct 14:59:05 2013

=========
 C++ API
=========

The C++ API of ``xbob.learn.libsvm`` allows users to leverage from automatic
converters for classes in :py:class:`xbob.learn.libsvm`.  To use the C API,
clients should first, include the header file ``<xbob.learn.libsvm/api.h>`` on
their compilation units and then, make sure to call once
``import_xbob_learn_libsvm()`` at their module instantiation, as explained at
the `Python manual
<http://docs.python.org/2/extending/extending.html#using-capsules>`_.

Here is a dummy C example showing how to include the header and where to call
the import function:

.. code-block:: c++

   #include <xbob.blitz/capi.h>
   #include <xbob.io/api.h>
   #include <xbob.learn.libsvm/api.h>

   PyMODINIT_FUNC initclient(void) {

     PyObject* m Py_InitModule("client", ClientMethods);

     if (!m) return 0;

     if (import_xbob_blitz() < 0) return 0;
     if (import_xbob_io() < 0) return 0;
     if (import_xbob_learn_libsvm() < 0) return 0;

     return m;

   }

.. note::

  The include directory can be discovered using
  :py:func:`xbob.learn.libsvm.get_include`.

File Interface
--------------

.. cpp:type:: PyBobLearnLibsvmFileObject

   The pythonic object representation for a ``bob::learn::libsvm::File``
   object.

   .. code-block:: cpp

      typedef struct {
        PyObject_HEAD
        bob::learn::libsvm::File* cxx;
      } PyBobLearnLibsvmFileObject

   .. cpp:member:: bob::learn::libsvm::File* cxx

      A pointer to the C++ file implementation.


.. cpp:function:: int PyBobLearnLibsvmFile_Check(PyObject* o)

   Checks if the input object ``o`` is a ``PyBobLearnLibsvmFileObject``.
   Returns ``1`` if it is, and ``0`` otherwise.

Machine Interface
-----------------

.. cpp:type:: PyBobLearnLibsvmMachineObject

   The pythonic object representation for a ``bob::learn::libsvm::Machine``
   object.

   .. code-block:: cpp

      typedef struct {
        PyObject_HEAD
        bob::learn::libsvm::Machine* cxx;
      } PyBobLearnLibsvmMachineObject

   .. cpp:member:: bob::learn::libsvm::Machine* cxx

      A pointer to the C++ machine implementation.


.. cpp:function:: int PyBobLearnLibsvmMachine_Check(PyObject* o)

   Checks if the input object ``o`` is a ``PyBobLearnLibsvmMachineObject``.
   Returns ``1`` if it is, and ``0`` otherwise.

.. cpp:function:: PyObject* PyBobLearnLibsvmMachine_NewFromMachine(bob::learn::libsvm::Machine* m)

   Builds a new Python object from an existing :cpp:class:`Machine`. The
   machine object ``m`` is **stolen** from the user, which should not delete it
   anymore.

Trainer Interface
-----------------

.. cpp:type:: PyBobLearnLibsvmTrainerObject

   The pythonic object representation for a ``bob::learn::libsvm::Trainer``
   object.

   .. code-block:: cpp

      typedef struct {
        PyObject_HEAD
        bob::learn::libsvm::Trainer* cxx;
      } PyBobLearnLibsvmTrainerObject

   .. cpp:member:: bob::learn::libsvm::Trainer* cxx

      A pointer to the C++ trainer implementation.


.. cpp:function:: int PyBobLearnLibsvmTrainer_Check(PyObject* o)

   Checks if the input object ``o`` is a ``PyBobLearnLibsvmTrainerObject``.
   Returns ``1`` if it is, and ``0`` otherwise.


Other Utilities
---------------

.. cpp:function:: PyObject* PyBobLearnLibsvm_MachineTypeAsString(bob::learn::libsvm::machine_t s)

   Returns a Python string representing given a machine type. Returns ``NULL``
   and sets an :py:class:`RuntimeError` if the enumeration provided is not
   supported.

   This function will return a proper :c:type:`PyStringObject` on Python 2.x
   and a :c:type:`PyUnicodeObject` on Python 3.x.

.. cpp:function:: bob::learn::libsvm::machine_t PyBobLearnLibsvm_StringAsMachineType(PyObject* o)

   Decodes the machine type enumeration from a pythonic string. Works with any
   string type or subtype. A :py:class:`RuntimeError` is set if the string
   cannot be encoded as one of the available enumerations. You must check for
   :c:func:`PyErr_Occurred` after a call to this function to make sure that the
   conversion was correctly performed.

.. cpp:function:: bob::learn::libsvm::machine_t PyBobLearnLibsvm_CStringAsMachineType(const char* s)

   This function works the same as
   :cpp:func:`PyBobLearnLibsvm_StringAsMachineType`, but accepts a C-style
   string instead of a Python object as input. A :py:class:`RuntimeError` is
   set if the string cannot be encoded as one of the available enumerations.
   You must check for :c:func:`PyErr_Occurred` after a call to this function to
   make sure that the conversion was correctly performed.

.. cpp:function:: PyObject* PyBobLearnLibsvm_KernelTypeAsString(bob::learn::libsvm::kernel_t s)

   Returns a Python string representing given a kernel type. Returns ``NULL``
   and sets an :py:class:`RuntimeError` if the enumeration provided is not
   supported.

   This function will return a proper :c:type:`PyStringObject` on Python 2.x
   and a :c:type:`PyUnicodeObject` on Python 3.x.

.. cpp:function:: bob::learn::libsvm::kernel_t PyBobLearnLibsvm_StringAsKernelType(PyObject* o)

   Decodes the kernel type enumeration from a pythonic string. Works with any
   string type or subtype. A :py:class:`RuntimeError` is set if the string
   cannot be encoded as one of the available enumerations. You must check for
   :c:func:`PyErr_Occurred` after a call to this function to make sure that the
   conversion was correctly performed.

.. cpp:function:: bob::learn::libsvm::kernel_t PyBobLearnLibsvm_CStringAsKernelType(const char* s)

   This function works the same as
   :cpp:func:`PyBobLearnLibsvm_StringAsKernelType`, but accepts a C-style
   string instead of a Python object as input. A :py:class:`RuntimeError` is
   set if the string cannot be encoded as one of the available enumerations.
   You must check for :c:func:`PyErr_Occurred` after a call to this function to
   make sure that the conversion was correctly performed.

.. include:: links.rst
