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

LIBSVM File Interface
----------------------

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

LIBSVM Machine Interface
------------------------

.. cpp:type:: PyBobLearnLibsvmMachineObject

   The pythonic object representation for a ``bob::learn::libsvm::Machine``
   object.

   .. code-block:: cpp

      typedef struct {
        PyObject_HEAD
        bob::learn::libsvm::Machine* cxx;
      } PyBobLearnLibsvmMachineObject

   .. cpp:member:: bob::learn::libsvm::Machine* cxx

      A pointer to the C++ file implementation.


.. include:: links.rst