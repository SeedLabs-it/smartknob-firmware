#include <PacketSerial.h>

#include "../proto_gen/smartknob.pb.h"

#include "proto_helpers.h"

#include "crc32.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "serial_protocol_protobuf.h"

static SerialProtocolProtobuf *singleton_for_packet_serial = 0;

static const uint16_t MIN_STATE_INTERVAL_MILLIS = 5;
static const uint16_t PERIODIC_STATE_INTERVAL_MILLIS = 5000;

SerialProtocolProtobuf::SerialProtocolProtobuf(Stream &stream, Configuration *configuration, ConfigCallback config_callback, MotorCalibrationCallback motor_calibration_callback, StrainCalibrationCallback strain_calibration_callback) : SerialProtocol(),
                                                                                                                                                                                                                                           stream_(stream),
                                                                                                                                                                                                                                           configuration_(configuration),
                                                                                                                                                                                                                                           config_callback_(config_callback),
                                                                                                                                                                                                                                           motor_calibration_callback_(motor_calibration_callback),
                                                                                                                                                                                                                                           strain_calibration_callback_(strain_calibration_callback),
                                                                                                                                                                                                                                           packet_serial_()
{
    packet_serial_.setStream(&stream);

    // Note: not threadsafe or instance safe!! but PacketSerial requires a legacy function pointer, so we can't
    // use a member, std::function, or lambda with captures
    assert(singleton_for_packet_serial == 0);
    singleton_for_packet_serial = this;

    packet_serial_.setPacketHandler([](const uint8_t *buffer, size_t size)
                                    { singleton_for_packet_serial->handlePacket(buffer, size); });
}

void SerialProtocolProtobuf::handleState(const PB_SmartKnobState &state)
{
    bool substantial_change = (latest_state_.current_position != state.current_position) || (latest_state_.config.detent_strength_unit != state.config.detent_strength_unit) || (latest_state_.config.endstop_strength_unit != state.config.endstop_strength_unit) || (latest_state_.config.min_position != state.config.min_position) || (latest_state_.config.max_position != state.config.max_position);
    latest_state_ = state;

    if (substantial_change)
    {
        char buf_[200];
        sprintf(buf_, "STATE: %d [%d, %d]  (detent strength: %0.2f, width: %0.0f deg, endstop strength: %0.2f)",
                state.current_position,
                state.config.min_position,
                state.config.max_position,
                state.config.detent_strength_unit,
                degrees(state.config.position_width_radians),
                state.config.endstop_strength_unit);
        LOGD(buf_);
    }
}

void SerialProtocolProtobuf::ack(uint32_t nonce)
{
    pb_tx_buffer_ = {};
    pb_tx_buffer_.which_payload = PB_FromSmartKnob_ack_tag;
    pb_tx_buffer_.payload.ack.nonce = nonce;
    sendPbTxBuffer();
}

void SerialProtocolProtobuf::log(const char *msg)
{
    char origin_[256];
    snprintf(origin_, sizeof(origin_), "%s:%s:%d ", __FILE__, __func__, __LINE__);
    log(PB_LogLevel_INFO, false, origin_, msg);
}

void SerialProtocolProtobuf::log(const PB_LogLevel log_level, bool isVerbose_, const char *origin, const char *msg)
{
    pb_tx_buffer_ = {};
    pb_tx_buffer_.which_payload = PB_FromSmartKnob_log_tag;
    pb_tx_buffer_.payload.log.level = log_level;
    pb_tx_buffer_.payload.log.isVerbose = isVerbose_;

    strlcpy(pb_tx_buffer_.payload.log.origin, origin, sizeof(pb_tx_buffer_.payload.log.origin));
    strlcpy(pb_tx_buffer_.payload.log.msg, msg, sizeof(pb_tx_buffer_.payload.log.msg));

    sendPbTxBuffer();
}

void SerialProtocolProtobuf::sendInitialInfo()
{
    pb_tx_buffer_ = {};
    pb_tx_buffer_.which_payload = PB_FromSmartKnob_knob_tag;
    strlcpy(pb_tx_buffer_.payload.knob.ip_address, WiFi.localIP().toString().c_str(), sizeof(pb_tx_buffer_.payload.knob.ip_address));
    strlcpy(pb_tx_buffer_.payload.knob.mac_address, WiFi.macAddress().c_str(), sizeof(pb_tx_buffer_.payload.knob.mac_address));
    PB_PersistentConfiguration config = configuration_->get();
    // if (config.version != 0)
    // {
    //     pb_tx_buffer_.payload.knob.has_persistent_config = config.has_motor;
    //     // pb_tx_buffer_.payload.knob.persistent_config = config;
    // }
    // else
    // {
    //     pb_tx_buffer_.payload.knob.has_persistent_config = false;
    // }

    sendPbTxBuffer();
}

void SerialProtocolProtobuf::sendStrainCalibState(const uint8_t step)

{
    LOGD("Sending strain calibration state");
    pb_tx_buffer_ = {};
    pb_tx_buffer_.which_payload = PB_FromSmartKnob_strain_calib_state_tag;
    pb_tx_buffer_.payload.strain_calib_state.step = step;
    pb_tx_buffer_.payload.strain_calib_state.strain_scale = 1.0f;

    sendPbTxBuffer();
}

void SerialProtocolProtobuf::loop()
{
    do
    {
        packet_serial_.update();
    } while (stream_.available());

    // Rate limit state change transmissions
    bool state_changed = !state_eq(latest_state_, last_sent_state_) && millis() - last_sent_state_millis_ >= MIN_STATE_INTERVAL_MILLIS;

    // Send state periodically or when forced, regardless of rate limit for state changes
    bool force_send_state = state_requested_ || millis() - last_sent_state_millis_ > PERIODIC_STATE_INTERVAL_MILLIS;
    if (state_changed || force_send_state)
    {
        state_requested_ = false;
        pb_tx_buffer_ = {};
        pb_tx_buffer_.which_payload = PB_FromSmartKnob_smartknob_state_tag;
        pb_tx_buffer_.payload.smartknob_state = latest_state_;

        sendPbTxBuffer();

        last_sent_state_ = latest_state_;
        last_sent_state_millis_ = millis();
    }
}

void SerialProtocolProtobuf::handlePacket(const uint8_t *buffer, size_t size)
{
    if (size <= 4)
    {
        // Too small, ignore bad packet
        LOGD("Small packet received. Ignoring.");
        return;
    }

    // Compute and append little-endian CRC32
    uint32_t expected_crc = 0;
    crc32(buffer, size - 4, &expected_crc);

    uint32_t provided_crc = buffer[size - 4] | (buffer[size - 3] << 8) | (buffer[size - 2] << 16) | (buffer[size - 1] << 24);

    if (expected_crc != provided_crc)
    {
        char buf[200];
        snprintf(buf, sizeof(buf), "Bad CRC (%u byte packet). Expected %08x but got %08x.", size - 4, expected_crc, provided_crc);
        LOGE(buf);
        return;
    }

    pb_istream_t stream = pb_istream_from_buffer(buffer, size - 4);
    if (!pb_decode(&stream, PB_ToSmartknob_fields, &pb_rx_buffer_))
    {
        char buf[200];
        snprintf(buf, sizeof(buf), "Decoding failed: %s", PB_GET_ERROR(&stream));
        LOGE(buf);
        return;
    }

    if (pb_rx_buffer_.protocol_version != PROTOBUF_PROTOCOL_VERSION)
    {
        char buf[200];
        snprintf(buf, sizeof(buf), "Invalid protocol version. Expected %u, received %u", PROTOBUF_PROTOCOL_VERSION, pb_rx_buffer_.protocol_version);
        LOGE(buf);
        return;
    }

    // Always ACK immediately
    ack(pb_rx_buffer_.nonce);
    if (pb_rx_buffer_.nonce == last_nonce_)
    {
        // Ignore any extraneous retries
        char buf[200];
        snprintf(buf, sizeof(buf), "Already handled nonce %u", pb_rx_buffer_.nonce);
        LOGD(buf);
        return;
    }
    last_nonce_ = pb_rx_buffer_.nonce;

    switch (pb_rx_buffer_.which_payload)
    {
    case PB_ToSmartknob_smartknob_config_tag:
    {
        config_callback_(pb_rx_buffer_.payload.smartknob_config);
        break;
    }
    case PB_ToSmartknob_smartknob_command_tag:
    {
        // Handle command
        LOGD("Command received");
        switch (pb_rx_buffer_.payload.smartknob_command)
        {
        case PB_SmartKnobCommand_GET_KNOB_INFO:
            LOGD("Get Knob Info");
            sendInitialInfo();
            break;
        case PB_SmartKnobCommand_MOTOR_CALIBRATE:
            LOGD("Motor Calibrate");
            motor_calibration_callback_();
            break;
        case PB_SmartKnobCommand_STRAIN_CALIBRATE:
            LOGD("Strain Calibrate");
            strain_calibration_callback_();
            break;
        default:
            LOGD("Unknown command");
            break;
        }
        break;
    }
    default:
    {
        char buf[200];
        snprintf(buf, sizeof(buf), "Unknown payload type: %d", pb_rx_buffer_.which_payload);
        LOGW(buf);
        return;
    }
    }
}

void SerialProtocolProtobuf::sendPbTxBuffer()
{
    // Encode protobuf message to byte buffer

    pb_ostream_t stream = pb_ostream_from_buffer(tx_buffer_, sizeof(tx_buffer_));
    pb_tx_buffer_.protocol_version = PROTOBUF_PROTOCOL_VERSION;

    stream.bytes_written = 0;
    if (!pb_encode(&stream, PB_FromSmartKnob_fields, &pb_tx_buffer_))
    {
        stream_.println(stream.errmsg);
        stream_.flush();
        LOGV(PB_LogLevel_ERROR, "PB Encoding failed: %s", stream.errmsg);
        LOGV(PB_LogLevel_ERROR, "PB Bytes written when failed: %d", stream.bytes_written);
        return;
    }

    // Compute and append little-endian CRC32
    uint32_t crc = 0;
    crc32(tx_buffer_, stream.bytes_written, &crc);
    tx_buffer_[stream.bytes_written + 0] = (crc >> 0) & 0xFF;
    tx_buffer_[stream.bytes_written + 1] = (crc >> 8) & 0xFF;
    tx_buffer_[stream.bytes_written + 2] = (crc >> 16) & 0xFF;
    tx_buffer_[stream.bytes_written + 3] = (crc >> 24) & 0xFF;

    // Encode and send proto+CRC as a COBS packet
    packet_serial_.send(tx_buffer_, stream.bytes_written + 4);
    stream_.flush();
}
