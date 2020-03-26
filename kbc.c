#include "kbc.h"

#include "asm.h"
#include "pic.h"

/* keyboard controller */

#define DATA_PORT 0x0060
#define STATUS_REGISTER 0x0064
#define COMMAND_REGISTER 0x0064

#define READY_TO_SEND(status) ((status & (1 << 1)) == 0)

#define WRITE_TO_CONTROLLER_CONFIGURATION_BYTE 0x60
#define WRITE_TO_CONTROLLER_OUTPUT_PORT 0xd4
/* generate interrupt for both keyboard and mouse */
#define CONTROLLER_CONFIGURATION_BYTE 0x47
#define CONTROLLER_OUTPUT_PORT 0xf4

void wait_for_ready(void)
{
    while (!READY_TO_SEND(in8(STATUS_REGISTER))) {
        ;
    }
}

void init_kbc(void)
{
    wait_for_ready();
    out8(COMMAND_REGISTER, WRITE_TO_CONTROLLER_CONFIGURATION_BYTE);
    wait_for_ready();
    out8(DATA_PORT, CONTROLLER_CONFIGURATION_BYTE);

    wait_for_ready();
    out8(COMMAND_REGISTER, WRITE_TO_CONTROLLER_OUTPUT_PORT);
    wait_for_ready();
    out8(DATA_PORT, CONTROLLER_OUTPUT_PORT);
}

char read_keyint(void) { return in8(DATA_PORT); }

char read_mouseint(void) { return in8(DATA_PORT); }
