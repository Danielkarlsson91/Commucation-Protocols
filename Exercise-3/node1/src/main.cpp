#include <CAN.h>
#include <Arduino.h>
#include <CAN_config.h>

#define ON '1'
#define OFF '0'

static bool show_message = true;

CAN_device_t CAN_cfg;

void setup()
{
    Serial.begin(115200);

    // Config the communication
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_35;
    CAN_cfg.speed = CAN_SPEED_500KBPS;
    CAN_cfg.rx_queue = xQueueCreate(1, sizeof(CAN_frame_t));

    (void)CAN_init(); // initialize CAN Module
}

void loop()
{
    if (show_message)
    {
        show_message = false;
        Serial.print("Enter a command [0: Off, 1: On]: ");
    }

    if (Serial.available())
    {
        char chr = Serial.read();
        if ((chr == OFF) || (chr == ON))
        {
            Serial.print(chr);

            CAN_frame_t frame{0};

            frame.MsgID = 0;
            frame.FIR.B.DLC = 1;
            frame.data.u8[0] = chr;
            frame.FIR.B.FF = CAN_frame_std;

            (void)CAN_write_frame(&frame);

            if (pdTRUE == xQueueReceive(CAN_cfg.rx_queue, &frame, portMAX_DELAY))
            {
                if ((frame.data.u8[0] == ON) || (frame.data.u8[0] == OFF))
                {
                    Serial.println((chr == frame.data.u8[0]) ? " => done" : " => failed");
                    show_message = true;
                }
            }
            else
            {
                Serial.println("Failed to receive!");
            }
        }
        else if (chr == '\n')
        {
            show_message = true;
            Serial.println();
        }
    }
}
