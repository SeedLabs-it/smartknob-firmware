#pragma once

#include <pb_encode.h>
#include <pb_decode.h>

#include <map>

#include <PacketSerial.h>
#include <logging/adapters/freertos/protocols/serial/serial_protocol.h>

#include "proto_gen/smartknob.pb.h"
#include "proto_helpers.h"
#include "crc32.h"

class SerialProtocolProtobuf : public SerialProtocol
{
public:
    struct TagHandlerParams
    {
        std::function<void(const PB_ToSmartknob &)> *handler;
        PB_ToSmartknob pb_rx_buffer_copy;
    };

    using TagCallback = std::function<void(const PB_ToSmartknob &)>;
    using CommandCallback = std::function<void()>;

    SerialProtocolProtobuf(Stream &stream);
    ~SerialProtocolProtobuf() {}

    void log(const LogMessage &log_msg) override;
    void log_raw(const char *msg) override;

    void registerTagCallback(pb_size_t tag, TagCallback callback);
    void registerCommandCallback(PB_SmartKnobCommand command, CommandCallback callback);
    void sendKnobInfo(PB_Knob knob);

protected:
    PB_FromSmartKnob pb_tx_buffer_;
    PB_ToSmartknob pb_rx_buffer_;

    uint8_t tx_buffer_[PB_FromSmartKnob_size + 4]; // Max message size + CRC32

    PacketSerial_<COBS, 0, (PB_ToSmartknob_size + 4) * 2 + 10> packet_serial_;

    uint32_t last_nonce_;

    void readSerial() override;

    void handlePacket(const uint8_t *buffer, size_t size);
    void sendPBTxBuffer();
    void ack(uint32_t nonce);

private:
    std::map<pb_size_t, TagCallback> tag_callbacks_;
    std::map<PB_SmartKnobCommand, CommandCallback> command_callbacks_;
};