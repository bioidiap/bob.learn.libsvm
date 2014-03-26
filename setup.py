#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Mon 16 Apr 08:18:08 2012 CEST

from setuptools import setup, find_packages, dist
dist.Distribution(dict(setup_requires=['xbob.extension', 'xbob.blitz', 'xbob.io']))
from xbob.extension.utils import egrep, find_header, find_library
from xbob.blitz.extension import Extension
import xbob.io

import os
package_dir = os.path.dirname(os.path.realpath(__file__))
package_dir = os.path.join(package_dir, 'xbob', 'learn', 'libsvm', 'include')
include_dirs = [
    package_dir,
    xbob.io.get_include(),
    ]

packages = []
version = '2.0.0a0'

# process libsvm requirement
import os
from distutils.version import LooseVersion

def libsvm_version(svm_h):

  matches = egrep(svm_h, r"#\s*define\s+LIBSVM_VERSION\s+(\d+)")
  if not len(matches): return None

  # we have a match, produce a string version of the version number
  version_int = int(matches[0].group(1))
  version_tuple = (version_int // 100, version_int % 100)
  return '.'.join([str(k) for k in version_tuple])

class libsvm:
  """A class for capturing configuration information from libsvm

  Example usage:

  .. doctest::
     :options: +NORMALIZE_WHITESPACE +ELLIPSIS

     >>> from xbob.learn.libsvm import libsvm
     >>> l = libsvm('>= 3.12')
     >>> l.include_directory
     '...'
     >>> l.version
     '...'
     >>> l.library_directory
     '...'
     >>> l.libraries
     [...]

  """

  def __init__ (self, requirement='', only_static=False):
    """
    Searches for libsvm in stock locations. Allows user to override.

    If the user sets the environment variable XBOB_PREFIX_PATH, that prefixes
    the standard path locations.

    Parameters:

    requirement, str
      A string, indicating a version requirement for libsvm. For example,
      ``'>= 3.12'``.

    only_static, boolean
      A flag, that indicates if we intend to link against the static library
      only. This will trigger our library search to disconsider shared
      libraries when searching.
    """

    candidates = find_header('svm.h', subpaths=['libsvm'])

    if not candidates:
      raise RuntimeError("could not find libsvm's `svm.h' - have you installed libsvm on this machine?")

    found = False

    if not requirement:
      self.include_directory = os.path.dirname(candidates[0])
      self.version = libsvm_version(candidates[0])
      found = True

    else:

      # requirement is 'operator' 'version'
      operator, required = [k.strip() for k in requirement.split(' ', 1)]

      # now check for user requirements
      for candidate in candidates:
        version = libsvm_version(candidate)
        available = LooseVersion(version)
        if (operator == '<' and available < required) or \
           (operator == '<=' and available <= required) or \
           (operator == '>' and available > required) or \
           (operator == '>=' and available >= required) or \
           (operator == '==' and available == required):
          self.include_directory = os.path.dirname(candidate)
          self.version = version
          found = True
          break

    if not found:
      raise RuntimeError("could not find the required (%s) version of libsvm on the file system (looked at: %s)" % (requirement, ', '.join(candidates)))

    # normalize
    self.include_directory = os.path.normpath(self.include_directory)

    # find library
    prefix = os.path.dirname(os.path.dirname(self.include_directory))
    candidates = find_library('svm', version=self.version,
            prefixes=[prefix], only_static=only_static)

    if not candidates:
      raise RuntimeError("cannot find required libsvm binary module `%s' - make sure libsvm is installed on `%s'" % (module, prefix))

    # libraries
    self.libraries = []
    name, ext = os.path.splitext(os.path.basename(candidates[0]))
    if ext in ['.so', '.a', '.dylib', '.dll']:
      self.libraries.append(name[3:]) #strip 'lib' from the name
    else: #link against the whole thing
      self.libraries.append(':' + os.path.basename(candidates[0]))

    # library path
    self.library_directory = os.path.dirname(candidates[0])

  def macros(self):
    """Returns package availability and version number macros

    This method returns a python list with 2 macros indicating package
    availability and a version number, using standard GNU compatible names.
    Example:

    .. doctest::
       :options: +NORMALIZE_WHITESPACE +ELLIPSIS

       >>> from xbob.learn.libsvm import libsvm
       >>> pkg = libsvm()
       >>> pkg.macros()
       [('HAVE_LIBSVM', '1'), ('LIBSVM_VERSION', '"..."')]

    """
    return [('HAVE_LIBSVM', '1'), ('LIBSVM_VERSION', '"%s"' % self.version)]

pkg = libsvm()
extra_compile_args = ['-isystem', pkg.include_directory]
library_dirs = [pkg.library_directory]
libraries = pkg.libraries
define_macros = pkg.macros()

setup(

    name='xbob.learn.libsvm',
    version=version,
    description='Bob bindings to libsvm',
    url='http://github.com/bioidiap/xbob.learn.libsvm',
    license='BSD',
    author='Andre Anjos',
    author_email='andre.anjos@idiap.ch',

    long_description=open('README.rst').read(),

    packages=find_packages(),
    include_package_data=True,

    install_requires=[
      'setuptools',
      'xbob.blitz',
      'xbob.io',
    ],

    namespace_packages=[
      "xbob",
      "xbob.learn",
      ],

    ext_modules = [
      Extension("xbob.learn.libsvm.version",
        [
          "xbob/learn/libsvm/version.cpp",
          ],
        packages = packages,
        include_dirs = include_dirs,
        version = version,
        extra_compile_args = extra_compile_args,
        define_macros = define_macros,
        library_dirs = library_dirs,
        libraries = libraries,
        ),
      Extension("xbob.learn.libsvm._library",
        [
          "xbob/learn/libsvm/file.cpp",
          ],
        packages = packages,
        include_dirs = include_dirs,
        version = version,
        extra_compile_args = extra_compile_args,
        define_macros = define_macros,
        library_dirs = library_dirs,
        libraries = libraries,
        ),
      ],

    entry_points={
      'console_scripts': [
        ],
      },

    classifiers = [
      'Development Status :: 3 - Alpha',
      'Intended Audience :: Developers',
      'License :: OSI Approved :: BSD License',
      'Natural Language :: English',
      'Programming Language :: Python',
      'Programming Language :: Python :: 3',
      'Topic :: Software Development :: Libraries :: Python Modules',
      ],

    )
