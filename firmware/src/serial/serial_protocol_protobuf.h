#pragma once

#include <PacketSerial.h>

#include "../proto_gen/smartknob.pb.h"
#if SK_MOTOR
#include "motor_task.h"
#endif
#include "serial_protocol.h"
#include "uart_stream.h"

class SerialProtocolProtobuf : public SerialProtocol {
    public:
    #if SK_MOTOR
        SerialProtocolProtobuf(Stream& stream, MotorTask& motor_task);
    #else
        SerialProtocolProtobuf(Stream& stream);
    #endif
        ~SerialProtocolProtobuf(){}
        void log(const char* msg) override;
        void loop() override;
        void handleState(const PB_SmartKnobState& state) override;
    
    private:
        Stream& stream_;
        #if SK_MOTOR
        MotorTask& motor_task_;
        #endif
        
        PB_FromSmartKnob pb_tx_buffer_;
        PB_ToSmartknob pb_rx_buffer_;

        uint8_t tx_buffer_[PB_FromSmartKnob_size + 4]; // Max message size + CRC32

        PacketSerial_<COBS, 0, (PB_ToSmartknob_size + 4) * 2 + 10> packet_serial_;

        uint32_t last_nonce_;

        PB_SmartKnobState latest_state_ = {};
        PB_SmartKnobState last_sent_state_ = {};
        uint32_t last_sent_state_millis_ = 0;

        bool state_requested_;

        void sendPbTxBuffer();
        void handlePacket(const uint8_t* buffer, size_t size);
        void ack(uint32_t nonce);
};
