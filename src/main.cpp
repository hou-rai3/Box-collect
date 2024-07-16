#include "mbed.h"

InterruptIn button_stop(PC_8);
InterruptIn button_stert(PC_9);

CAN can(PA_11, PA_12, (int)1e6);
CANMessage msg;
uint8_t DATA[8] = {0};
uint8_t DATA_1[8] = {0};
int DJI_ID = 0x1FF;
int16_t speed = 0;

bool flag = false;

void stop_motor(int zero)
{
    printf("STOP\n");
    for (int i = 0; i < 4; i += 2)
    {
        DATA[i] = (zero >> 8) & 0xFF;
        DATA[i + 1] = zero & 0xFF;
    }
}

int main()
{
    button_stop.mode(PullUp);
    button_stert.mode(PullUp);
    BufferedSerial pc(USBTX, USBRX, 115200);
    auto pre = HighResClock::now();

    while (1)
    {
        auto now = HighResClock::now();

        bool sw_stop = button_stop.read();
        if (!sw_stop)
        {
            printf("Stert\n");
            speed = 16000;

            int16_t signed_speed = static_cast<int16_t>(-speed);
            DATA[2] = (signed_speed >> 8) & 0xFF;
            DATA[3] = signed_speed & 0xFF;

            signed_speed = static_cast<int16_t>(-speed);
            DATA[4] = (signed_speed >> 8) & 0xFF;
            DATA[5] = signed_speed & 0xFF;

            signed_speed = static_cast<int16_t>(speed);
            DATA_1[6] = (signed_speed >> 8) & 0xFF;
            DATA_1[7] = signed_speed & 0xFF;
        }
        else
        {
            speed = 0;
            int16_t signed_speed = static_cast<int16_t>(speed);
            DATA[2] = (signed_speed >> 8) & 0xFF;
            DATA[3] = signed_speed & 0xFF;

            signed_speed = static_cast<int16_t>(-speed);
            DATA[4] = (signed_speed >> 8) & 0xFF;
            DATA[5] = signed_speed & 0xFF;

            signed_speed = static_cast<int16_t>(speed);
            DATA_1[6] = (signed_speed >> 8) & 0xFF;
            DATA_1[7] = signed_speed & 0xFF;
        }

        if (now - pre > 30ms)
        {
            CANMessage msg(DJI_ID, DATA, 8);
            CANMessage msg1(0x200, DATA_1, 8);
            if (can.write(msg) && can.write(msg1))
            {
            }
            else
            {
                printf("Can't send Message\n");
                printf("CAN Bus Error Status: %d\n", can.rderror());
                printf("CAN Bus Write Error Count: %d\n", can.tderror());
                can.reset();
            }
            pre = now;
        }
    }
}
