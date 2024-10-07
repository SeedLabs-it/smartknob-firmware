#pragma once

#include <functional>

#include "proto/proto_gen/smartknob.pb.h"

typedef std::function<void(PB_SmartKnobConfig &)> ConfigCallback;
typedef std::function<void(void)> MotorCalibrationCallback;
typedef std::function<void(float)> StrainCalibrationCallback;
typedef std::function<void(float)> FactoryStrainCalibrationCallback;
typedef std::function<void(void)> WeightMeasurementCallback;
