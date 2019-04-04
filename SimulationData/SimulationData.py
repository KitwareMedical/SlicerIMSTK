import os
import unittest
import vtk, qt, ctk, slicer
from slicer.ScriptedLoadableModule import *
import logging

#
# SimulationData
#

class SimulationData(ScriptedLoadableModule):

  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "SimulationData"
    self.parent.categories = ["Simulation"]
    self.parent.dependencies = ["SampleData"]
    self.parent.contributors = ["Johan Andruejol (Kitware Inc.)"]
    self.parent.helpText = """This module gives access to simulation sample data
"""
    self.parent.helpText += self.getDefaultModuleDocumentationLink()
    self.parent.acknowledgementText = """\todo"""

    moduleDir = os.path.dirname(self.parent.path)
    iconsPath = os.path.join(moduleDir, 'Resources/Icons')
    print(iconsPath)

    # Register the sample data here:
    self.data = [
      {
      'category': 'Simulation',
      'sampleName': 'Kidney',
      'uris': 'https://data.kitware.com/api/v1/file/5ca766298d777f072b03017b/download',
      'fileNames': 'Kidney.vtu',
      'nodeNames': 'Kidney',
      'loadFileType': 'ModelFile',
      'thumbnailFileName': os.path.join(iconsPath, 'Kidney.png')
      },
      {
      'category': 'Simulation',
      'sampleName': 'Floor',
      'uris': 'https://data.kitware.com/api/v1/file/5ca766358d777f072b030187/download',
      'fileNames': 'Floor.vtk',
      'nodeNames': 'Floor',
      'loadFileType': 'ModelFile',
      'thumbnailFileName': os.path.join(iconsPath, 'Floor.png')
      }
    ]

    import SampleData
    for data in self.data:
      SampleData.SampleDataLogic.registerCustomSampleDataSource(
        category=data['category'],
        sampleName=data['sampleName'],
        uris=data['uris'],
        fileNames=data['fileNames'],
        nodeNames=data['nodeNames'],
        loadFileType=data['loadFileType'],
        thumbnailFileName=data['thumbnailFileName'],
      )

class SimulationDataTest(ScriptedLoadableModuleTest):
  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough.
    """
    slicer.mrmlScene.Clear(0)

  def runTest(self):
    """Run as few or as many tests as needed here.
    """
    self.setUp()
    self.test_CollisionSimulation() #\todo

  def test_CollisionSimulation(self):
    """ \todo
    """

    self.delayDisplay("Starting the test")
    self.delayDisplay('Test passed!')
