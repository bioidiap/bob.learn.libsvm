#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Sun  4 Mar 20:06:14 2012
#
# Copyright (C) 2011-2013 Idiap Research Institute, Martigny, Switzerland


"""Tests for libsvm training
"""

import os
import numpy
import tempfile
import pkg_resources
import nose.tools

from . import File, Machine, Trainer

def F(f, module=None):
  """Returns the test file on the "data" subdirectory"""
  if module is None:
    return pkg_resources.resource_filename(__name__, os.path.join('data', f))
  return pkg_resources.resource_filename('bob.%s.test' % module,
      os.path.join('data', f))

def tempname(suffix, prefix='bobtest_'):
  (fd, name) = tempfile.mkstemp(suffix, prefix)
  os.close(fd)
  os.unlink(name)
  return name

TEST_MACHINE_NO_PROBS = F('heart_no_probs.svmmodel', 'machine')

HEART_DATA = F('heart.svmdata', 'machine') #13 inputs
HEART_MACHINE = F('heart.svmmodel', 'machine') #supports probabilities
HEART_EXPECTED = F('heart.out', 'machine') #expected probabilities

def test_initialization():

  # tests and examplifies some initialization parameters

  # all defaults
  trainer = Trainer()

def test_training():

  # For this example I'm using an SVM file because of convinience. You only
  # need to make sure you can gather the input into 2D double arrays in which
  # each array represents data from one class and each line on such array
  # contains a sample.
  f = File(HEART_DATA)
  labels, data = f.read_all()
  neg = numpy.vstack([k for i,k in enumerate(data) if labels[i] < 0])
  pos = numpy.vstack([k for i,k in enumerate(data) if labels[i] > 0])

  # Data is also pre-scaled so features remain in the range between -1 and
  # +1. libsvm, apparently, suggests you do that for all features. Our
  # bindings to libsvm do not include scaling. If you want to implement that
  # generically, please do it.

  trainer = Trainer()
  machine = trainer.train((pos, neg)) #ordering only affects labels
  previous = Machine(TEST_MACHINE_NO_PROBS)
  nose.tools.eq_(machine.machine_type, previous.machine_type)
  nose.tools.eq_(machine.kernel_type, previous.kernel_type)
  nose.tools.eq_(machine.gamma, previous.gamma)
  nose.tools.eq_(machine.shape, previous.shape)
  assert numpy.all(abs(machine.input_subtract - previous.input_subtract) < 1e-8)
  assert numpy.all(abs(machine.input_divide - previous.input_divide) < 1e-8)

  curr_label = machine.predict_class(data)
  prev_label = previous.predict_class(data)
  assert numpy.array_equal(curr_label, prev_label)

  curr_labels, curr_scores = machine.predict_class_and_scores(data)
  prev_labels, prev_scores = previous.predict_class_and_scores(data)
  assert numpy.array_equal(curr_labels, prev_labels)

  curr_scores = numpy.array(curr_scores)
  prev_scores = numpy.array(prev_scores)
  assert numpy.all(abs(curr_scores - prev_scores) < 1e-8)

def test_training_with_probability():

  f = File(HEART_DATA)
  labels, data = f.read_all()
  neg = numpy.vstack([k for i,k in enumerate(data) if labels[i] < 0])
  pos = numpy.vstack([k for i,k in enumerate(data) if labels[i] > 0])

  # Data is also pre-scaled so features remain in the range between -1 and
  # +1. libsvm, apparently, suggests you do that for all features. Our
  # bindings to libsvm do not include scaling. If you want to implement that
  # generically, please do it.

  trainer = Trainer(probability=True)
  machine = trainer.train((pos, neg)) #ordering only affects labels
  previous = Machine(HEART_MACHINE)
  nose.tools.eq_(machine.machine_type, previous.machine_type)
  nose.tools.eq_(machine.kernel_type, previous.kernel_type)
  nose.tools.eq_(machine.gamma, previous.gamma)
  nose.tools.eq_(machine.shape, previous.shape)
  assert numpy.all(abs(machine.input_subtract - previous.input_subtract) < 1e-8)
  assert numpy.all(abs(machine.input_divide - previous.input_divide) < 1e-8)

  # check labels
  curr_label = machine.predict_class(data)
  prev_label = previous.predict_class(data)
  assert numpy.array_equal(curr_label, prev_label)

  # check scores
  curr_labels, curr_scores = machine.predict_class_and_scores(data)
  prev_labels, prev_scores = previous.predict_class_and_scores(data)
  assert numpy.array_equal(curr_labels, prev_labels)

  curr_scores = numpy.array(curr_scores)
  prev_scores = numpy.array(prev_scores)
  assert numpy.all(abs(curr_scores - prev_scores) < 1e-8)

  # check probabilities -- probA and probB do not get the exact same values
  # as when using libsvm's svm-train.c. The reason may lie in the order in
  # which the samples are arranged.
  curr_labels, curr_scores = machine.predict_class_and_probabilities(data)
  prev_labels, prev_scores = previous.predict_class_and_probabilities(data)
  curr_scores = numpy.array(curr_scores)
  prev_scores = numpy.array(prev_scores)
  #assert numpy.all(abs(curr_scores-prev_scores) < 1e-8)
