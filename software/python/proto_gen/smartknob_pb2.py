# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: smartknob.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import nanopb_pb2 as nanopb__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x0fsmartknob.proto\x12\x02PB\x1a\x0cnanopb.proto\"\x9a\x02\n\rFromSmartKnob\x12\x1f\n\x10protocol_version\x18\x01 \x01(\rB\x05\x92?\x02\x38\x08\x12\x18\n\x04knob\x18\x03 \x01(\x0b\x32\x08.PB.KnobH\x00\x12\x16\n\x03\x61\x63k\x18\x04 \x01(\x0b\x32\x07.PB.AckH\x00\x12\x16\n\x03log\x18\x05 \x01(\x0b\x32\x07.PB.LogH\x00\x12-\n\x0fsmartknob_state\x18\x06 \x01(\x0b\x32\x12.PB.SmartKnobStateH\x00\x12\x30\n\x11motor_calib_state\x18\x07 \x01(\x0b\x32\x13.PB.MotorCalibStateH\x00\x12\x32\n\x12strain_calib_state\x18\x08 \x01(\x0b\x32\x14.PB.StrainCalibStateH\x00\x42\t\n\x07payload\"\x8c\x02\n\x0bToSmartknob\x12\x1f\n\x10protocol_version\x18\x01 \x01(\rB\x05\x92?\x02\x38\x08\x12\r\n\x05nonce\x18\x02 \x01(\r\x12)\n\rrequest_state\x18\x03 \x01(\x0b\x32\x10.PB.RequestStateH\x00\x12/\n\x10smartknob_config\x18\x04 \x01(\x0b\x32\x13.PB.SmartKnobConfigH\x00\x12\x31\n\x11smartknob_command\x18\x05 \x01(\x0e\x32\x14.PB.SmartKnobCommandH\x00\x12\x33\n\x12strain_calibration\x18\x06 \x01(\x0b\x32\x15.PB.StrainCalibrationH\x00\x42\t\n\x07payload\"u\n\x04Knob\x12\x1a\n\x0bmac_address\x18\x01 \x01(\tB\x05\x92?\x02p2\x12\x19\n\nip_address\x18\x02 \x01(\tB\x05\x92?\x02p2\x12\x36\n\x11persistent_config\x18\x03 \x01(\x0b\x32\x1b.PB.PersistentConfiguration\"%\n\x0fMotorCalibState\x12\x12\n\ncalibrated\x18\x01 \x01(\x08\"6\n\x10StrainCalibState\x12\x0c\n\x04step\x18\x01 \x01(\r\x12\x14\n\x0cstrain_scale\x18\x02 \x01(\x02\"\x14\n\x03\x41\x63k\x12\r\n\x05nonce\x18\x01 \x01(\r\"b\n\x03Log\x12\x13\n\x03msg\x18\x01 \x01(\tB\x06\x92?\x03p\xff\x01\x12\x1b\n\x05level\x18\x02 \x01(\x0e\x32\x0c.PB.LogLevel\x12\x16\n\x06origin\x18\x03 \x01(\tB\x06\x92?\x03p\x80\x01\x12\x11\n\tisVerbose\x18\x04 \x01(\x08\"\x86\x01\n\x0eSmartKnobState\x12\x18\n\x10\x63urrent_position\x18\x01 \x01(\x05\x12\x19\n\x11sub_position_unit\x18\x02 \x01(\x02\x12#\n\x06\x63onfig\x18\x03 \x01(\x0b\x32\x13.PB.SmartKnobConfig\x12\x1a\n\x0bpress_nonce\x18\x04 \x01(\rB\x05\x92?\x02\x38\x08\"\xe1\x02\n\x0fSmartKnobConfig\x12\x10\n\x08position\x18\x01 \x01(\x05\x12\x19\n\x11sub_position_unit\x18\x02 \x01(\x02\x12\x1d\n\x0eposition_nonce\x18\x03 \x01(\rB\x05\x92?\x02\x38\x08\x12\x14\n\x0cmin_position\x18\x04 \x01(\x05\x12\x14\n\x0cmax_position\x18\x05 \x01(\x05\x12\x1e\n\x16position_width_radians\x18\x06 \x01(\x02\x12\x1c\n\x14\x64\x65tent_strength_unit\x18\x07 \x01(\x02\x12\x1d\n\x15\x65ndstop_strength_unit\x18\x08 \x01(\x02\x12\x12\n\nsnap_point\x18\t \x01(\x02\x12\x13\n\x04text\x18\n \x01(\tB\x05\x92?\x02p2\x12\x1f\n\x10\x64\x65tent_positions\x18\x0b \x03(\x05\x42\x05\x92?\x02\x10\x05\x12\x17\n\x0fsnap_point_bias\x18\x0c \x01(\x02\x12\x16\n\x07led_hue\x18\r \x01(\x05\x42\x05\x92?\x02\x38\x10\"\x0e\n\x0cRequestState\"e\n\x17PersistentConfiguration\x12\x0f\n\x07version\x18\x01 \x01(\r\x12#\n\x05motor\x18\x02 \x01(\x0b\x32\x14.PB.MotorCalibration\x12\x14\n\x0cstrain_scale\x18\x03 \x01(\x02\"p\n\x10MotorCalibration\x12\x12\n\ncalibrated\x18\x01 \x01(\x08\x12\x1e\n\x16zero_electrical_offset\x18\x02 \x01(\x02\x12\x14\n\x0c\x64irection_cw\x18\x03 \x01(\x08\x12\x12\n\npole_pairs\x18\x04 \x01(\r\"8\n\x0bStrainState\x12\x14\n\x0cpress_weight\x18\x01 \x01(\x05\x12\x13\n\x0bpress_value\x18\x02 \x01(\x02\"/\n\x11StrainCalibration\x12\x1a\n\x12\x63\x61libration_weight\x18\x01 \x01(\x02*D\n\x08LogLevel\x12\x08\n\x04INFO\x10\x00\x12\x0b\n\x07WARNING\x10\x01\x12\t\n\x05\x45RROR\x10\x02\x12\t\n\x05\x44\x45\x42UG\x10\x03\x12\x0b\n\x07VERBOSE\x10\x04*P\n\x10SmartKnobCommand\x12\x11\n\rGET_KNOB_INFO\x10\x00\x12\x13\n\x0fMOTOR_CALIBRATE\x10\x01\x12\x14\n\x10STRAIN_CALIBRATE\x10\x02\x62\x06proto3')

_LOGLEVEL = DESCRIPTOR.enum_types_by_name['LogLevel']
LogLevel = enum_type_wrapper.EnumTypeWrapper(_LOGLEVEL)
_SMARTKNOBCOMMAND = DESCRIPTOR.enum_types_by_name['SmartKnobCommand']
SmartKnobCommand = enum_type_wrapper.EnumTypeWrapper(_SMARTKNOBCOMMAND)
INFO = 0
WARNING = 1
ERROR = 2
DEBUG = 3
VERBOSE = 4
GET_KNOB_INFO = 0
MOTOR_CALIBRATE = 1
STRAIN_CALIBRATE = 2


_FROMSMARTKNOB = DESCRIPTOR.message_types_by_name['FromSmartKnob']
_TOSMARTKNOB = DESCRIPTOR.message_types_by_name['ToSmartknob']
_KNOB = DESCRIPTOR.message_types_by_name['Knob']
_MOTORCALIBSTATE = DESCRIPTOR.message_types_by_name['MotorCalibState']
_STRAINCALIBSTATE = DESCRIPTOR.message_types_by_name['StrainCalibState']
_ACK = DESCRIPTOR.message_types_by_name['Ack']
_LOG = DESCRIPTOR.message_types_by_name['Log']
_SMARTKNOBSTATE = DESCRIPTOR.message_types_by_name['SmartKnobState']
_SMARTKNOBCONFIG = DESCRIPTOR.message_types_by_name['SmartKnobConfig']
_REQUESTSTATE = DESCRIPTOR.message_types_by_name['RequestState']
_PERSISTENTCONFIGURATION = DESCRIPTOR.message_types_by_name['PersistentConfiguration']
_MOTORCALIBRATION = DESCRIPTOR.message_types_by_name['MotorCalibration']
_STRAINSTATE = DESCRIPTOR.message_types_by_name['StrainState']
_STRAINCALIBRATION = DESCRIPTOR.message_types_by_name['StrainCalibration']
FromSmartKnob = _reflection.GeneratedProtocolMessageType('FromSmartKnob', (_message.Message,), {
  'DESCRIPTOR' : _FROMSMARTKNOB,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.FromSmartKnob)
  })
_sym_db.RegisterMessage(FromSmartKnob)

ToSmartknob = _reflection.GeneratedProtocolMessageType('ToSmartknob', (_message.Message,), {
  'DESCRIPTOR' : _TOSMARTKNOB,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.ToSmartknob)
  })
_sym_db.RegisterMessage(ToSmartknob)

Knob = _reflection.GeneratedProtocolMessageType('Knob', (_message.Message,), {
  'DESCRIPTOR' : _KNOB,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.Knob)
  })
_sym_db.RegisterMessage(Knob)

MotorCalibState = _reflection.GeneratedProtocolMessageType('MotorCalibState', (_message.Message,), {
  'DESCRIPTOR' : _MOTORCALIBSTATE,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.MotorCalibState)
  })
_sym_db.RegisterMessage(MotorCalibState)

StrainCalibState = _reflection.GeneratedProtocolMessageType('StrainCalibState', (_message.Message,), {
  'DESCRIPTOR' : _STRAINCALIBSTATE,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.StrainCalibState)
  })
_sym_db.RegisterMessage(StrainCalibState)

Ack = _reflection.GeneratedProtocolMessageType('Ack', (_message.Message,), {
  'DESCRIPTOR' : _ACK,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.Ack)
  })
_sym_db.RegisterMessage(Ack)

Log = _reflection.GeneratedProtocolMessageType('Log', (_message.Message,), {
  'DESCRIPTOR' : _LOG,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.Log)
  })
_sym_db.RegisterMessage(Log)

SmartKnobState = _reflection.GeneratedProtocolMessageType('SmartKnobState', (_message.Message,), {
  'DESCRIPTOR' : _SMARTKNOBSTATE,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.SmartKnobState)
  })
_sym_db.RegisterMessage(SmartKnobState)

SmartKnobConfig = _reflection.GeneratedProtocolMessageType('SmartKnobConfig', (_message.Message,), {
  'DESCRIPTOR' : _SMARTKNOBCONFIG,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.SmartKnobConfig)
  })
_sym_db.RegisterMessage(SmartKnobConfig)

RequestState = _reflection.GeneratedProtocolMessageType('RequestState', (_message.Message,), {
  'DESCRIPTOR' : _REQUESTSTATE,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.RequestState)
  })
_sym_db.RegisterMessage(RequestState)

PersistentConfiguration = _reflection.GeneratedProtocolMessageType('PersistentConfiguration', (_message.Message,), {
  'DESCRIPTOR' : _PERSISTENTCONFIGURATION,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.PersistentConfiguration)
  })
_sym_db.RegisterMessage(PersistentConfiguration)

MotorCalibration = _reflection.GeneratedProtocolMessageType('MotorCalibration', (_message.Message,), {
  'DESCRIPTOR' : _MOTORCALIBRATION,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.MotorCalibration)
  })
_sym_db.RegisterMessage(MotorCalibration)

StrainState = _reflection.GeneratedProtocolMessageType('StrainState', (_message.Message,), {
  'DESCRIPTOR' : _STRAINSTATE,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.StrainState)
  })
_sym_db.RegisterMessage(StrainState)

StrainCalibration = _reflection.GeneratedProtocolMessageType('StrainCalibration', (_message.Message,), {
  'DESCRIPTOR' : _STRAINCALIBRATION,
  '__module__' : 'smartknob_pb2'
  # @@protoc_insertion_point(class_scope:PB.StrainCalibration)
  })
_sym_db.RegisterMessage(StrainCalibration)

if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _FROMSMARTKNOB.fields_by_name['protocol_version']._options = None
  _FROMSMARTKNOB.fields_by_name['protocol_version']._serialized_options = b'\222?\0028\010'
  _TOSMARTKNOB.fields_by_name['protocol_version']._options = None
  _TOSMARTKNOB.fields_by_name['protocol_version']._serialized_options = b'\222?\0028\010'
  _KNOB.fields_by_name['mac_address']._options = None
  _KNOB.fields_by_name['mac_address']._serialized_options = b'\222?\002p2'
  _KNOB.fields_by_name['ip_address']._options = None
  _KNOB.fields_by_name['ip_address']._serialized_options = b'\222?\002p2'
  _LOG.fields_by_name['msg']._options = None
  _LOG.fields_by_name['msg']._serialized_options = b'\222?\003p\377\001'
  _LOG.fields_by_name['origin']._options = None
  _LOG.fields_by_name['origin']._serialized_options = b'\222?\003p\200\001'
  _SMARTKNOBSTATE.fields_by_name['press_nonce']._options = None
  _SMARTKNOBSTATE.fields_by_name['press_nonce']._serialized_options = b'\222?\0028\010'
  _SMARTKNOBCONFIG.fields_by_name['position_nonce']._options = None
  _SMARTKNOBCONFIG.fields_by_name['position_nonce']._serialized_options = b'\222?\0028\010'
  _SMARTKNOBCONFIG.fields_by_name['text']._options = None
  _SMARTKNOBCONFIG.fields_by_name['text']._serialized_options = b'\222?\002p2'
  _SMARTKNOBCONFIG.fields_by_name['detent_positions']._options = None
  _SMARTKNOBCONFIG.fields_by_name['detent_positions']._serialized_options = b'\222?\002\020\005'
  _SMARTKNOBCONFIG.fields_by_name['led_hue']._options = None
  _SMARTKNOBCONFIG.fields_by_name['led_hue']._serialized_options = b'\222?\0028\020'
  _LOGLEVEL._serialized_start=1762
  _LOGLEVEL._serialized_end=1830
  _SMARTKNOBCOMMAND._serialized_start=1832
  _SMARTKNOBCOMMAND._serialized_end=1912
  _FROMSMARTKNOB._serialized_start=38
  _FROMSMARTKNOB._serialized_end=320
  _TOSMARTKNOB._serialized_start=323
  _TOSMARTKNOB._serialized_end=591
  _KNOB._serialized_start=593
  _KNOB._serialized_end=710
  _MOTORCALIBSTATE._serialized_start=712
  _MOTORCALIBSTATE._serialized_end=749
  _STRAINCALIBSTATE._serialized_start=751
  _STRAINCALIBSTATE._serialized_end=805
  _ACK._serialized_start=807
  _ACK._serialized_end=827
  _LOG._serialized_start=829
  _LOG._serialized_end=927
  _SMARTKNOBSTATE._serialized_start=930
  _SMARTKNOBSTATE._serialized_end=1064
  _SMARTKNOBCONFIG._serialized_start=1067
  _SMARTKNOBCONFIG._serialized_end=1420
  _REQUESTSTATE._serialized_start=1422
  _REQUESTSTATE._serialized_end=1436
  _PERSISTENTCONFIGURATION._serialized_start=1438
  _PERSISTENTCONFIGURATION._serialized_end=1539
  _MOTORCALIBRATION._serialized_start=1541
  _MOTORCALIBRATION._serialized_end=1653
  _STRAINSTATE._serialized_start=1655
  _STRAINSTATE._serialized_end=1711
  _STRAINCALIBRATION._serialized_start=1713
  _STRAINCALIBRATION._serialized_end=1760
# @@protoc_insertion_point(module_scope)
