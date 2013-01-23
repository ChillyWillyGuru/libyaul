/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2.h>
#include <smpc.h>
#include <smpc/peripheral.h>

#include <cons/vdp2.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_BUTTON_PRESSED(p, s) ((p) ? "[1;37;0;44m"s"[m" : "*")

char *
id_port(int type, int size)
{
        int id = (type << 4) | size;
        switch (id)
        {
            case 0x02:
                return "digital ";
            case 0x13:
                return "racing  ";
            case 0x16:
                return "analog  ";
            case 0x23:
            case 0xE3:
                return "mouse   ";
            case 0x34:
                return "keyboard";
        }

        return "unknown ";
}

void
get_buttons(struct smpc_peripheral_port *port, char *text)
{
        if (!port->info.connected)
                (void)sprintf(text, "%s               ", text);
        else if ((port->info.type == 2 || port->info.type == 14) && port->info.size == 3)
        {
                /* controller is mouse - buttons are positive */
                struct smpc_peripheral_mouse *mouse = (struct smpc_peripheral_mouse *)&port->info;
                (void)sprintf(text, "%s%s%s%s%s           ",
                    text,
                    IS_BUTTON_PRESSED(mouse->button.start, "S"),
                    IS_BUTTON_PRESSED(mouse->button.m_btn, "M"),
                    IS_BUTTON_PRESSED(mouse->button.r_btn, "R"),
                    IS_BUTTON_PRESSED(mouse->button.l_btn, "L"));
        }
        else
        {
                /* all other controllers return same buttons states as digital pad */
                struct smpc_peripheral_digital *digital = (struct smpc_peripheral_digital *)&port->info;
                (void)sprintf(text, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s  ",
                    text,
                    IS_BUTTON_PRESSED(!digital->button.right, "R"),
                    IS_BUTTON_PRESSED(!digital->button.left, "L"),
                    IS_BUTTON_PRESSED(!digital->button.down, "D"),
                    IS_BUTTON_PRESSED(!digital->button.up, "U"),
                    IS_BUTTON_PRESSED(!digital->button.start, "S"),
                    IS_BUTTON_PRESSED(!digital->button.a_trg, "A"),
                    IS_BUTTON_PRESSED(!digital->button.c_trg, "C"),
                    IS_BUTTON_PRESSED(!digital->button.b_trg, "B"),
                    IS_BUTTON_PRESSED(!digital->button.r_trg, ">"),
                    IS_BUTTON_PRESSED(!digital->button.x_trg, "X"),
                    IS_BUTTON_PRESSED(!digital->button.y_trg, "Y"),
                    IS_BUTTON_PRESSED(!digital->button.z_trg, "Z"),
                    IS_BUTTON_PRESSED(!digital->button.l_trg, "<"));
        }
}

int
main(void)
{
        uint16_t blcs_color[] = {
                0x9C00
        };

        char *text;

        struct smpc_peripheral_port *port[2];

        struct cons cons;

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();

        cons_vdp2_init(&cons);

        port[0] = smpc_peripheral_raw_port(1);
        port[1] = smpc_peripheral_raw_port(2);

        text = (char *)malloc(8192);
        assert(text != NULL);

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                (void)sprintf(text,
                    "[1;32;1;45m    *** Console (cons component) ***    [m\n"
                    "\n"
                    "PORT: %d        PORT: %d\n"
                    "TYPE: 0x%02X     TYPE: 0x%02X\n"
                    "SIZE: %d Bytes  SIZE: %d Bytes\n"
                    "ID: %s   ID: %s\n"
                    "DATA[ 0]: [1;37;1;40m0x%02X[m DATA[ 0]: [1;37;1;40m0x%02X[m\n"
                    "DATA[ 1]: [1;37;1;44m0x%02X[m DATA[ 1]: [1;37;1;44m0x%02X[m\n"
                    "DATA[ 2]: [1;37;1;40m0x%02X[m DATA[ 2]: [1;37;1;40m0x%02X[m\n"
                    "DATA[ 3]: [1;37;1;44m0x%02X[m DATA[ 3]: [1;37;1;44m0x%02X[m\n"
                    "DATA[ 4]: [1;37;1;40m0x%02X[m DATA[ 4]: [1;37;1;40m0x%02X[m\n"
                    "DATA[ 5]: [1;37;1;44m0x%02X[m DATA[ 5]: [1;37;1;44m0x%02X[m\n"
                    "DATA[ 6]: [1;37;1;40m0x%02X[m DATA[ 6]: [1;37;1;40m0x%02X[m\n"
                    "DATA[ 7]: [1;37;1;44m0x%02X[m DATA[ 7]: [1;37;1;44m0x%02X[m\n"
                    "DATA[ 8]: [1;37;1;40m0x%02X[m DATA[ 8]: [1;37;1;40m0x%02X[m\n"
                    "DATA[ 9]: [1;37;1;44m0x%02X[m DATA[ 9]: [1;37;1;44m0x%02X[m\n"
                    "DATA[10]: [1;37;1;40m0x%02X[m DATA[10]: [1;37;1;40m0x%02X[m\n"
                    "DATA[11]: [1;37;1;44m0x%02X[m DATA[11]: [1;37;1;44m0x%02X[m\n"
                    "DATA[12]: [1;37;1;40m0x%02X[m DATA[12]: [1;37;1;40m0x%02X[m\n"
                    "DATA[13]: [1;37;1;44m0x%02X[m DATA[13]: [1;37;1;44m0x%02X[m\n"
                    "DATA[14]: [1;37;1;40m0x%02X[m DATA[14]: [1;37;1;40m0x%02X[m\n",
                    port[0]->info.port_no, port[1]->info.port_no,
                    port[0]->info.type, port[1]->info.type,
                    port[0]->info.size, port[1]->info.size,
                    id_port(port[0]->info.type, port[0]->info.size), id_port(port[1]->info.type, port[1]->info.size),
                    port[0]->info.data[0], port[1]->info.data[0],
                    port[0]->info.data[1], port[1]->info.data[1],
                    port[0]->info.data[2], port[1]->info.data[2],
                    port[0]->info.data[3], port[1]->info.data[3],
                    port[0]->info.data[4], port[1]->info.data[4],
                    port[0]->info.data[5], port[1]->info.data[5],
                    port[0]->info.data[6], port[1]->info.data[6],
                    port[0]->info.data[7], port[1]->info.data[7],
                    port[0]->info.data[8], port[1]->info.data[8],
                    port[0]->info.data[9], port[1]->info.data[9],
                    port[0]->info.data[10], port[1]->info.data[10],
                    port[0]->info.data[11], port[1]->info.data[11],
                    port[0]->info.data[12],port[1]->info.data[12],
                    port[0]->info.data[13], port[1]->info.data[13],
                    port[0]->info.data[14], port[1]->info.data[14]);

                (void)sprintf(text, "%s[2B", text);
                get_buttons(port[0], text);
                get_buttons(port[1], text);
                (void)sprintf(text, "%s[H", text);
                cons_write(&cons, text);
        }

        return 0;
}
