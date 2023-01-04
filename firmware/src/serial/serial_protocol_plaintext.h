#pragma once

#include "../proto_gen/smartknob.pb.h"

#if SK_MOTOR
#include "motor_task.h"
#endif
#include "serial_protocol.h"
#include "uart_stream.h"
#include "led_handler.h"

typedef std::function<void(void)> DemoConfigChangeCallback;

class SerialProtocolPlaintext : public SerialProtocol {
    public:
    #if SK_MOTOR
        SerialProtocolPlaintext(Stream& stream, MotorTask& motor_task) : SerialProtocol(), stream_(stream), motor_task_(motor_task) {}
    #else
        SerialProtocolPlaintext(Stream& stream) : SerialProtocol(), stream_(stream) {}
    #endif    
        ~SerialProtocolPlaintext(){}
        void log(const char* msg) override;
        void loop() override;
        void handleState(const PB_SmartKnobState& state) override;

        void init(DemoConfigChangeCallback cb);
    
    private:
        Stream& stream_;
        #if SK_MOTOR
        MotorTask& motor_task_;
        #endif
        PB_SmartKnobState latest_state_ = {};
        DemoConfigChangeCallback demo_config_change_callback_;
};
