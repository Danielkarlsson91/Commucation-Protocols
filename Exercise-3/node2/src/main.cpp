#include <CAN.h>
#include <Arduino.h>
#include <CAN_config.h>

#define ON '1'
#define OFF '0'

CAN_device_t CAN_cfg;

void setup()
{
    pinMode(GPIO_NUM_21, OUTPUT); // Connect a LED to pin 21

    // Config the communication
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_35;
    CAN_cfg.speed = CAN_SPEED_500KBPS;
    CAN_cfg.rx_queue = xQueueCreate(1, sizeof(CAN_frame_t));

    (void)CAN_init(); // initialize CAN Module
}

void loop()
{
    CAN_frame_t frame{0};

    if (pdTRUE == xQueueReceive(CAN_cfg.rx_queue, &frame, portMAX_DELAY))
    {
        if ((frame.data.u8[0] == ON) || (frame.data.u8[0] == OFF))
        {
            digitalWrite(GPIO_NUM_21, (frame.data.u8[0] == ON) ? HIGH : LOW);
            frame.data.u8[0] = (HIGH == digitalRead(GPIO_NUM_21)) ? ON : OFF;
            (void)CAN_write_frame(&frame);
        }
    }
}
