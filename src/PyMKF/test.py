import pprint
import PyMKF


constants = PyMKF.get_constants()
pprint.pprint(constants['residualGap'])
pprint.pprint(constants['minimumNonResidualGap'])
pprint.pprint(PyMKF.get_constants())


# coreData = {'functionalDescription': {'bobbin': None,
#                            'gapping': [{'area': None,
#                                         'coordinates': None,
#                                         'distanceClosestNormalSurface': None,
#                                         'length': 0.001,
#                                         'sectionDimensions': None,
#                                         'shape': None,
#                                         'type': 'subtractive'},
#                                        {'area': None,
#                                         'coordinates': None,
#                                         'distanceClosestNormalSurface': None,
#                                         'length': 1e-05,
#                                         'sectionDimensions': None,
#                                         'shape': None,
#                                         'type': 'residual'},
#                                        {'area': None,
#                                         'coordinates': None,
#                                         'distanceClosestNormalSurface': None,
#                                         'length': 1e-05,
#                                         'sectionDimensions': None,
#                                         'shape': None,
#                                         'type': 'residual'}],
#                            'material': '3C97',
#                            'name': 'default',
#                            'numberStacks': 1,
#                            'shape': {'aliases': [],
#                                      'dimensions': {'A': 0.0391,
#                                                     'B': 0.0198,
#                                                     'C': 0.0125,
#                                                     'D': 0.0146,
#                                                     'E': 0.030100000000000002,
#                                                     'F': 0.0125,
#                                                     'G': 0.0,
#                                                     'H': 0.0},
#                                      'family': 'etd',
#                                      'familySubtype': '1',
#                                      'name': 'Custom',
#                                      'type': 'custom'},
#                            'type': 'two-piece set'},
#  'geometricalDescription': None,
#  'processedDescription': None}

# core_data = PyMKF.get_core_data(coreData)
# pprint.pprint(core_data)

# coreData = {'functionalDescription': {'bobbin': None,
#                            'gapping': [{'area': 0.000123,
#                                         'coordinates': [0.0, 0.0005, 0.0],
#                                         'distanceClosestNormalSurface': 0.0141,
#                                         'length': 0.001,
#                                         'sectionDimensions': [0.0125, 0.0125],
#                                         'shape': 'round',
#                                         'type': 'subtractive'},
#                                        {'area': 6.2e-05,
#                                         'coordinates': [0.017301, 0.0, 0.0],
#                                         'distanceClosestNormalSurface': 0.014595,
#                                         'length': 1e-05,
#                                         'sectionDimensions': [0.004501, 0.0125],
#                                         'shape': 'irregular',
#                                         'type': 'residual'},
#                                        {'area': 6.2e-05,
#                                         'coordinates': [-0.017301, 0.0, 0.0],
#                                         'distanceClosestNormalSurface': 0.014595,
#                                         'length': 1e-05,
#                                         'sectionDimensions': [0.004501, 0.0125],
#                                         'shape': 'irregular',
#                                         'type': 'residual'}],
#                            'material': '3C97',
#                            'name': 'default',
#                            'numberStacks': 1,
#                            'shape': {'aliases': [],
#                                      'dimensions': {'A': 0.0125,
#                                                     'B': 0.0064,
#                                                     'C': 0.0088,
#                                                     'D': 0.0046,
#                                                     'E': 0.01,
#                                                     'F': 0.0043,
#                                                     'G': 0.0,
#                                                     'H': 0.0,
#                                                     'K': 0.0023},
#                                      'family': 'ep',
#                                      'familySubtype': '1',
#                                      'name': 'Custom',
#                                      'type': 'custom'},
#                            'type': 'two-piece set'},
#  'geometricalDescription': None,
#  'processedDescription': None}

# core_data = PyMKF.get_core_data(coreData)
# pprint.pprint(core_data)


# coreData = {'functionalDescription': {'bobbin': None,
#                            'gapping': [{'area': 1.5e-05,
#                                         'coordinates': [0.0, 0.0, 0.0],
#                                         'distanceClosestNormalSurface': 0.00455,
#                                         'length': 0.0001,
#                                         'sectionDimensions': [0.0043, 0.0043],
#                                         'shape': 'round',
#                                         'type': 'subtractive'},
#                                        {'area': 8.8e-05,
#                                         'coordinates': [0.0, 0.0, 0.00805],
#                                         'distanceClosestNormalSurface': 0.004598,
#                                         'length': 5e-06,
#                                         'sectionDimensions': [0.058628,
#                                                               0.001501],
#                                         'shape': 'irregular',
#                                         'type': 'residual'}],
#                            'material': '3C97',
#                            'name': 'default',
#                            'numberStacks': 1,
#                            'shape': {'aliases': [],
#                                      'dimensions': {'A': 0.101,
#                                                     'B': 0.076,
#                                                     'C': 0.03,
#                                                     'D': 0.048,
#                                                     'E': 0.044,
#                                                     'G': 0.0,
#                                                     'H': 0.0},
#                                      'family': 'u',
#                                      'familySubtype': '1',
#                                      'name': 'Custom',
#                                      'type': 'custom'},
#                            'type': 'two-piece set'},
#  'geometricalDescription': None,
#  'processedDescription': None}

# core_data = PyMKF.get_core_data(coreData)
# pprint.pprint(core_data)


coreData = {"functionalDescription":{"name":"default","type":"two-piece set","material":"3C97","shape":{"aliases":[],"dimensions":{"A":0.0595,"B":0.036,"C":0.017,"D":0.0215,"H":0,"G":0},"family":"ur","familySubtype":"1","name":"UR 59/36/17","type":"standard"},"gapping":[{"area":0.000083,"coordinates":[0,0,0],"distanceClosestNormalSurface":0.0015,"length":0.001,"sectionDimensions":[0.0063,0.0145],"shape":"oblong","type":"subtractive"},{"area":0.000043,"coordinates":[0.01145,0,0],"distanceClosestNormalSurface":0.001996,"length":0.00001,"sectionDimensions":[0.002101,0.02],"shape":"rectangular","type":"residual"},{"area":0.000043,"coordinates":[-0.01145,0,0],"distanceClosestNormalSurface":0.001996,"length":0.00001,"sectionDimensions":[0.002101,0.02],"shape":"rectangular","type":"residual"}],"numberStacks":1,"bobbin":None},"geometricalDescription":None,"processedDescription":None}

pprint.pprint("mierda")
core_data = PyMKF.get_core_data(coreData)
pprint.pprint(core_data['processedDescription']['columns'])
pprint.pprint(len(core_data['functionalDescription']['gapping']))


# coreGap = {
#     'area': 0.000123,
#     'coordinates': [0.0, 0.0, 0.0],
#     'distanceClosestNormalSurface': 0.0146,
#     'length': 0.0001,
#     'sectionDimensions': [0.0125, 0.0125],
#     'shape': 'round',
#     'type': 'additive'
# }

# reluctance_data = PyMKF.get_gap_reluctance(coreGap, "ZHANG")
# pprint.pprint(reluctance_data)


# coreGap = {
#     'area': 0.00017,
#     'coordinates': [0, 0.0002, 0],
#     'distanceClosestNormalSurface': 0.0104,
#     'height': 0,
#     'length': 0.001,
#     'sectionDimensions': [0.0147, 0.0147],
#     'shape': 'round',
#     'type': 'subtractive'
# }

# reluctance_data = PyMKF.get_gap_reluctance(coreGap, "ZHANG")
# pprint.pprint(reluctance_data)

# models_info = PyMKF.get_gap_reluctance_model_information()
# pprint.pprint(models_info)
