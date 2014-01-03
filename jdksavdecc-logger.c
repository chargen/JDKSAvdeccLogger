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

bool open_all_network_ports(void);
void close_all_network_ports(void);

#define MAX_ETHERNET_PORTS (16)

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


int main(int argc, char **argv ) {
    if( open_all_network_ports() ) {
        close_all_network_ports();
    }
    return 0;
}
