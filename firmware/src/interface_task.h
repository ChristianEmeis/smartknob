#pragma once

#include <AceButton.h>
#include <Arduino.h>

#include "display_task.h"
#include "logger.h"
#if SK_MOTOR
    #include "motor_task.h"
#endif
#include "serial/serial_protocol_plaintext.h"
#include "serial/serial_protocol_protobuf.h"
#include "serial/uart_stream.h"
#include "task.h"

class InterfaceTask : public Task<InterfaceTask>, public Logger {
    friend class Task<InterfaceTask>; // Allow base Task to invoke protected run()

    public:
    #if SK_MOTOR
        InterfaceTask(const uint8_t task_core, MotorTask& motor_task, DisplayTask* display_task);
    #else
        InterfaceTask(const uint8_t task_core, DisplayTask* display_task);
    #endif
        void changeLed(uint8_t led_data[]);
        virtual ~InterfaceTask() {};

        void log(const char* msg) override;

    protected:
        void run();

    private:
        UartStream stream_;
    #if SK_MOTOR
        MotorTask& motor_task_;
    #endif
        DisplayTask* display_task_;
        char buf_[64];

        int current_config_ = 0;

        QueueHandle_t log_queue_;
        QueueHandle_t knob_state_queue_;
        SerialProtocolPlaintext plaintext_protocol_;
        SerialProtocolProtobuf proto_protocol_;

        void changeConfig(bool next);
        void updateHardware();
};
