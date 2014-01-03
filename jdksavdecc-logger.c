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
#include "us_rawnet.h"
#include <ifaddrs.h>


us_rawnet_context_t ethernet_ports[MAX_ETHERNET_PORTS];
int ethernet_port_count=0;

#if defined(__APPLE__) || defined( __linux__ )
bool open_all_network_ports(void) {
    struct ifaddrs *net_interfaces;
    if( getifaddrs(&net_interfaces)==0 ) {
        struct ifaddrs *cur = net_interfaces;
        while ( cur && ethernet_port_count<MAX_ETHERNET_PORTS ) {
            if( us_rawnet_socket(
                    &ethernet_ports[ ethernet_port_count ],
                    JDKSAVDECC_AVTP_ETHERTYPE,
                    cur->ifa_name,
                    jdksavdecc_multicast_adp_acmp.value )>=0 ) {
                ethernet_port_count++;
                fprintf(stdout,"Opened ethernet port %s\n", cur->ifa_name);
            }
            cur=cur->ifa_next;
        }
        freeifaddrs(net_interfaces);
    } else {
        fprintf(stderr,"Unable to query interface address list");
    }
    return ethernet_port_count>0;
}

void close_all_network_ports(void) {
    int i;
    for( i=0; i<ethernet_port_count; ++i ) {
        us_rawnet_close(&ethernet_ports[i]);
    }
}

#elif defined(WIN32)
// TODO: windows version of open_all_network_ports
#endif

int receive_avdecc_from_all_ports(
    bool (*handler)(uint8_t *buf, uint16_t len )
    ) {
    int receive_count=0;
    int i;
    for( i=0; i<ethernet_port_count; ++i ) {
        uint8_t data[JDKSAVDECC_AECP_FRAME_MAX_SIZE];
        int r = us_rawnet_recv(
                &ethernet_ports[i],
                &data[JDKSAVDECC_FRAME_HEADER_SA_OFFSET],
                &data[JDKSAVDECC_FRAME_HEADER_DA_OFFSET],
                &data[JDKSAVDECC_FRAME_HEADER_LEN],
                sizeof(data) - JDKSAVDECC_FRAME_HEADER_LEN);

        if (r > 1) {
            // returned length includes frame header
            r += JDKSAVDECC_FRAME_HEADER_LEN;
            // manually stick ethertype in there as well
            data[JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET + 0] = (JDKSAVDECC_AVTP_ETHERTYPE >> 8) & 0xff;
            data[JDKSAVDECC_FRAME_HEADER_ETHERTYPE_OFFSET + 1] = (JDKSAVDECC_AVTP_ETHERTYPE >> 0) & 0xff;
            handler(data,r);
            receive_count++;
        }
    }
    return receive_count;
}

void send_avdecc_to_all_ports(
    uint8_t *data,
    uint16_t len,
    uint8_t const *data1,
    uint16_t len1,
    uint8_t const *data2,
    uint16_t len2
     ) {
    int frames_sent=0;
    int i;
    uint8_t buf[JDKSAVDECC_AECP_FRAME_MAX_SIZE];

    // Put pieces together into one buffer, with frame header
    memcpy(buf, data, len);
    if (data1 && len1) {
        memcpy(buf + len, data1, len1);
    }
    if (data2 && len2) {
        memcpy(buf + len + len1, data2, len2);
    }
    uint16_t total_len = len + len1 + len2;
    for( i=0; i<ethernet_port_count; ++i ) {
        ssize_t sent = us_rawnet_send(
                            &ethernet_ports[i],
                            &buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET],
                            &buf[JDKSAVDECC_FRAME_HEADER_LEN],
                            total_len - JDKSAVDECC_FRAME_HEADER_LEN);
        if( sent>0 ) {
            frames_sent++;
        }
    }

}

void send_avdecc_response_to_all_ports(
    uint8_t *data,
    uint16_t len,
    uint8_t const *data1,
    uint16_t len1,
    uint8_t const *data2,
    uint16_t len2
     ) {
    // Send reply to whoever sent me this frame
    // Put pieces together into one buffer, with frame header
    int frames_sent=0;
    int i;
    uint8_t buf[JDKSAVDECC_AECP_FRAME_MAX_SIZE];

    // Put pieces together into one buffer, with frame header
    memcpy(buf, data, len);
    if (data1 && len1) {
        memcpy(buf + len, data1, len1);
    }
    if (data2 && len2) {
        memcpy(buf + len + len1, data2, len2);
    }
    uint16_t total_len = len + len1 + len2;
    for( i=0; i<ethernet_port_count; ++i ) {
        ssize_t sent = us_rawnet_send(
                            &ethernet_ports[i],
                            &buf[JDKSAVDECC_FRAME_HEADER_SA_OFFSET],
                            &buf[JDKSAVDECC_FRAME_HEADER_LEN],
                            total_len - JDKSAVDECC_FRAME_HEADER_LEN);
        if( sent>0 ) {
            frames_sent++;
        }
    }
}

void poll_incoming(
        bool (*handler)(uint8_t *buf, uint16_t len ) );

void poll_incoming(
        bool (*handler)(uint8_t *buf, uint16_t len ) ) {
    int max_poll_count=128;
    int poll_count=0;
    for( poll_count=0; poll_count<max_poll_count; ++poll_count ) {
        if( receive_avdecc_from_all_ports(handler)==0 ) {
            break;
        }
    }
}

bool incoming_packet_handler( uint8_t *buf, uint16_t len ) {
#if 1
    if( buf[JDKSAVDECC_FRAME_HEADER_LEN+0]!=0x80+JDKSAVDECC_SUBTYPE_ADP ) {
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
        struct jdksavdecc_acmpdu acmp;
        if( jdksavdecc_acmpdu_read(&acmp, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            char text[2048]="";
            struct jdksavdecc_printer print;
            print.buf = text;
            print.max_len = sizeof(text);
            print.pos = 0;
            jdksavdecc_acmpdu_print(&print, &acmp);
            fprintf( stdout, "%s\n", text );
        }
    }
#endif
    return true;
}

int main(int argc, char **argv ) {
    us_platform_init_sockets();
    if( open_all_network_ports() ) {

        while(!us_platform_sigint_seen && !us_platform_sigterm_seen ) {
            poll_incoming( incoming_packet_handler );
            usleep(10000);
        }

        close_all_network_ports();
    }
    return 0;
}
