/*
Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "jdksavdecc-logger.h"
#include "us_rawnet_multi.h"
#include "us_logger_stdio.h"



bool incoming_packet_handler( us_rawnet_multi_t *self, void *context, uint8_t *buf, uint16_t len ) {
#if 1
#if 0
    fprintf( stdout, "RX len:%4d DA:%02x:%02x:%02x:%02x:%02x:%02x SA:%02x:%02x:%02x:%02x:%02x:%02x EtherType:%04x Subtype:%02x\n",
        len,
        buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+0],
        buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+1],
        buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+2],
        buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+3],
        buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+4],
        buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET+5],

        buf[JDKSAVDECC_FRAME_HEADER_SA_OFFSET+0],
        buf[JDKSAVDECC_FRAME_HEADER_SA_OFFSET+1],
        buf[JDKSAVDECC_FRAME_HEADER_SA_OFFSET+2],
        buf[JDKSAVDECC_FRAME_HEADER_SA_OFFSET+3],
        buf[JDKSAVDECC_FRAME_HEADER_SA_OFFSET+4],
        buf[JDKSAVDECC_FRAME_HEADER_SA_OFFSET+5],

        jdksavdecc_uint16_get(buf,JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET),
        buf[JDKSAVDECC_FRAME_HEADER_LEN+0]
        );
#endif
    char text[4096]="";
    struct jdksavdecc_printer print;
    jdksavdecc_printer_init(&print, text, sizeof(text));

    if( buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_ACMP ) {
        struct jdksavdecc_acmpdu acmp;
        if( jdksavdecc_acmpdu_read(&acmp, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            jdksavdecc_printer_print(&print,"ACMP:");
            jdksavdecc_printer_print_eol(&print);
            jdksavdecc_acmpdu_print(&print, &acmp);
            jdksavdecc_printer_print_eol(&print);
        }
    }
    else if( buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_ADP ) {
        struct jdksavdecc_adpdu adp;
        if( jdksavdecc_adpdu_read(&adp, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            jdksavdecc_printer_print(&print,"ADP:");
            jdksavdecc_printer_print_eol(&print);
            jdksavdecc_adpdu_print(&print, &adp);
            jdksavdecc_printer_print_eol(&print);
        }
    }
    fprintf( stdout, "%s", text );

#endif
    return true;
}

int main(int argc, char **argv ) {
    us_platform_init_sockets();
    us_rawnet_multi_t multi_rawnet;
    us_logger_stdio_start(stdout, stderr);
    us_log_set_level(US_LOG_LEVEL_DEBUG);

    if( us_rawnet_multi_open(&multi_rawnet,JDKSAVDECC_AVTP_ETHERTYPE,jdksavdecc_multicast_adp_acmp.value,0) ) {

        while(!us_platform_sigint_seen && !us_platform_sigterm_seen ) {
            us_rawnet_multi_rawnet_poll_incoming( &multi_rawnet, 256, 0, incoming_packet_handler );
            usleep(10000);
        }

        us_rawnet_multi_close(&multi_rawnet);
    }
    return 0;
}
