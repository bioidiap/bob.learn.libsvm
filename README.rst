.. vim: set fileencoding=utf-8 :
.. Andre Anjos <andre.anjos@idiap.ch>
.. Fri 13 Dec 2013 12:35:22 CET

.. image:: https://travis-ci.org/bioidiap/bob.learn.libsvm.svg?branch=master
   :target: https://travis-ci.org/bioidiap/bob.learn.libsvm
.. image:: https://coveralls.io/repos/bioidiap/bob.learn.libsvm/badge.png
   :target: https://coveralls.io/r/bioidiap/bob.learn.libsvm
.. image:: http://img.shields.io/github/tag/bioidiap/bob.learn.libsvm.png
   :target: https://github.com/bioidiap/bob.learn.libsvm
.. image:: http://img.shields.io/pypi/v/bob.learn.libsvm.png
   :target: https://pypi.python.org/pypi/bob.learn.libsvm
.. image:: http://img.shields.io/pypi/dm/bob.learn.libsvm.png
   :target: https://pypi.python.org/pypi/bob.learn.libsvm

================================
 Bob-Python bindings for LIBSVM
================================

This package contains a set of Pythonic bindings for LIBSVM.

External Software Requirements
------------------------------

This package requires you have ``LIBSVM`` installed on your system. If
``LIBSVM`` is installed on a standard location, this package will find it
automatically. You can just skip to the installation instructions.

In case you have installed ``LIBSVM`` on a non-standard location (e.g. your
home directory), then you can specify the path to the root of that installation
using the environment variable ``BOB_PREFIX_PATH``, **before** building and
installing the package. For example, if ``BOB_PREFIX_PATH`` is set to
``/home/user/libsvm-3.12``, then we will search for the header file ``svm.h``
in ``/home/user/libsvm-3.12/include`` and expect that the library file file is
found under ``/home/user/libsvm-3.12/lib``.

If you are installing this package via ``pip`` for example, just set the
environment variable like this::

  $ BOB_PREFIX_PATH=/home/user/libsvm-3.12 pip install bob.learn.libsvm

If you are installing this package via ``zc.buildout``, we recommend you use
our `bob.buildout <http://pypi.python.org/pypi/bob.buildout>`_ extension and
set the ``[environ]`` section to define ``BOB_PREFIX_PATH``.

Installation
------------

Install it through normal means, via ``pip`` or use ``zc.buildout`` to
bootstrap the package itself and run test units.

Documentation
-------------

You can generate the documentation for this package, after installation, using
Sphinx::

  $ sphinx-build -b html doc sphinx

This shall place in the directory ``sphinx``, the current version for the
documentation of the package.

Testing
-------

You can run a set of tests using the nose test runner::

  $ nosetests -sv bob.learn.libsvm

.. warning::

   If Bob <= 1.2.1 is installed on your python path, nose will automatically
   load the old version of the insulate plugin available in Bob, which will
   trigger the loading of incompatible shared libraries (from Bob itself), in
   to your working binary. This will cause a stack corruption. Either remove
   the centrally installed version of Bob, or build your own version of Python
   in which Bob <= 1.2.1 is not installed.

You can run our documentation tests using sphinx itself::

  $ sphinx-build -b doctest doc sphinx

You can test overall test coverage with::

  $ nosetests --with-coverage --cover-package=bob.learn.libsvm

The ``coverage`` egg must be installed for this to work properly.

Development
-----------

To develop this package, install using ``zc.buildout``, using the buildout
configuration found on the root of the package::

  $ python bootstrap.py
  ...
  $ ./bin/buildout

Tweak the options in ``buildout.cfg`` to disable/enable verbosity and debug
builds.
