#include "serial_protocol_protobuf.h"

static SerialProtocolProtobuf *singleton_for_packet_serial = 0;

SerialProtocolProtobuf::SerialProtocolProtobuf(Stream &stream) : SerialProtocol(stream)
{

    packet_serial_.setStream(&stream);

    // Note: not threadsafe or instance safe!! but PacketSerial requires a legacy function pointer, so we can't
    // use a member, std::function, or lambda with captures
    assert(singleton_for_packet_serial == 0);
    singleton_for_packet_serial = this;

    packet_serial_.setPacketHandler([](const uint8_t *buffer, size_t size)
                                    { singleton_for_packet_serial->handlePacket(buffer, size); });
}

void SerialProtocolProtobuf::log(LogMessage *log_msg)
{
    pb_tx_buffer_ = {};
    pb_tx_buffer_.which_payload = PB_FromSmartKnob_log_tag;
    pb_tx_buffer_.payload.log.level = LogLevelConverter::toPBLogLevel(log_msg->level);
    pb_tx_buffer_.payload.log.isVerbose = log_msg->verbose;

    strlcpy(pb_tx_buffer_.payload.log.origin, log_msg->origin, sizeof(pb_tx_buffer_.payload.log.origin));
    strlcpy(pb_tx_buffer_.payload.log.msg, log_msg->msg, sizeof(pb_tx_buffer_.payload.log.msg));
    // TODO add timestamp

    sendPBTxBuffer();
}

void SerialProtocolProtobuf::log_raw(const char *msg)
{
    LOGW("LOG_RAW NOT IMPLEMENTED FOR PROTOBUF PROTCOL");
}

void SerialProtocolProtobuf::registerTagCallback(pb_size_t tag, TagCallback callback)
{
    tag_callbacks_[tag] = callback;
}

void SerialProtocolProtobuf::registerCommandCallback(PB_SmartKnobCommand command, CommandCallback callback)
{
    command_callbacks_[command] = callback;
}

void SerialProtocolProtobuf::sendKnobInfo(PB_Knob knob)
{
    pb_tx_buffer_ = {};
    pb_tx_buffer_.which_payload = PB_FromSmartKnob_knob_tag;
    pb_tx_buffer_.payload.knob = knob;
    sendPBTxBuffer();
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
        LOGE("Bad CRC (%u byte packet). Expected %08x but got %08x.", size - 4, expected_crc, provided_crc);
        return;
    }

    pb_istream_t stream = pb_istream_from_buffer(buffer, size - 4);
    if (!pb_decode(&stream, PB_ToSmartknob_fields, &pb_rx_buffer_))
    {
        LOGE("Decoding failed: %s", PB_GET_ERROR(&stream));
        return;
    }

    if (pb_rx_buffer_.protocol_version != PROTOBUF_PROTOCOL_VERSION)
    {
        LOGE("Invalid protocol version. Expected %u, received %u", PROTOBUF_PROTOCOL_VERSION, pb_rx_buffer_.protocol_version);
        return;
    }

    // Always ACK immediately
    ack(pb_rx_buffer_.nonce);
    if (pb_rx_buffer_.nonce == last_nonce_)
    {
        LOGD("Already handled nonce %u", pb_rx_buffer_.nonce);
        return;
    }
    last_nonce_ = pb_rx_buffer_.nonce;

    if (tag_callbacks_.find(pb_rx_buffer_.which_payload) != tag_callbacks_.end())
    {
        TagHandlerParams *params = new TagHandlerParams{
            new std::function<void(const PB_ToSmartknob &)>(tag_callbacks_[pb_rx_buffer_.which_payload]),
            pb_rx_buffer_};

        xTaskCreate(
            [](void *param)
            {
                TagHandlerParams *params = reinterpret_cast<TagHandlerParams *>(param);

                (*params->handler)(params->pb_rx_buffer_copy);

                delete params->handler;
                delete params;

                vTaskDelete(NULL);
            },
            "tag_handler_task", 1024 * 8, params, 5, NULL);
    }
    else if (pb_rx_buffer_.which_payload == PB_ToSmartknob_smartknob_command_tag)
    {
        if (command_callbacks_.find(pb_rx_buffer_.payload.smartknob_command) != command_callbacks_.end())
        {
            auto handler = new std::function<void()>(command_callbacks_[pb_rx_buffer_.payload.smartknob_command]);
            xTaskCreate(
                [](void *param)
                {
                    auto handler = reinterpret_cast<std::function<void()> *>(param);
                    (*handler)();
                    delete handler;
                    vTaskDelete(NULL);
                },
                "key_handler_task", 1024 * 8, handler, 5, NULL); // TODO stack size and priority?
        }
        else
        {
            LOGE("Unknown command");
        }
    }
    else
    {
        LOGE("Unknown payload");
    }
}

void SerialProtocolProtobuf::sendPBTxBuffer()
{
    // Encode protobuf message to byte buffer

    pb_ostream_t stream = pb_ostream_from_buffer(tx_buffer_, sizeof(tx_buffer_));
    pb_tx_buffer_.protocol_version = PROTOBUF_PROTOCOL_VERSION;

    stream.bytes_written = 0;
    if (!pb_encode(&stream, PB_FromSmartKnob_fields, &pb_tx_buffer_))
    {
        stream_.println(stream.errmsg);
        stream_.flush();
        LOGE("PB Encoding failed: %s", stream.errmsg);
        LOGE("PB Bytes written when failed: %d", stream.bytes_written);
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

void SerialProtocolProtobuf::ack(uint32_t nonce)
{
    pb_tx_buffer_ = {};
    pb_tx_buffer_.which_payload = PB_FromSmartKnob_ack_tag;
    pb_tx_buffer_.payload.ack.nonce = nonce;
    sendPBTxBuffer();
}

void SerialProtocolProtobuf::readSerial()
{
    do
    {
        packet_serial_.update();
    } while (stream_.available());
}