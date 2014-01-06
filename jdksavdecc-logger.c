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
#include <stdint.h>
#include <time.h>
#include "us_allocator.h"
#include "us_rawnet_multi.h"
#include "us_logger_stdio.h"
#include "us_getopt.h"

bool option_help;
bool option_help_default=false;
bool option_dump;
bool option_dump_default=false;
int16_t option_adp;
int16_t option_adp_default=true;
int16_t option_acmp;
int16_t option_acmp_default=true;
int16_t option_aecp;
int16_t option_aecp_default=true;
int16_t option_jdkslog;
int16_t option_jdkslog_default=true;
uint64_t option_entity;
struct jdksavdecc_eui64 option_entity_eui64;
uint64_t option_entity_default=0xffffffffffffffffULL;

us_malloc_allocator_t allocator;
us_getopt_t options;
us_getopt_option_t option[] = {
    {"dump","Dump settings to stdout", US_GETOPT_FLAG, &option_dump_default, &option_dump },
    {"help","Show help", US_GETOPT_FLAG, &option_help_default, &option_help },
    {"adp","Enable logging ADP", US_GETOPT_INT16, &option_adp_default, &option_adp },
    {"acmp","Enable logging ACMP", US_GETOPT_INT16, &option_acmp_default, &option_acmp },
    {"aecp","Enable logging AECP", US_GETOPT_INT16, &option_aecp_default, &option_aecp },
    {"jdkslog","Enable logging of JDKS Logging messages", US_GETOPT_INT16, &option_jdkslog_default, &option_jdkslog },
    {"entity","Limit to only receive messages involving this entity", US_GETOPT_HEX64, &option_entity_default, &option_entity },
    {0,0,0,0}
};

us_rawnet_multi_t multi_rawnet;





void incoming_packet_handler( us_rawnet_multi_t *self, int ethernet_port, void *context, uint8_t *buf, uint16_t len ) {
    char text[4096]="";
    struct jdksavdecc_printer print;
    jdksavdecc_printer_init(&print, text, sizeof(text));

    if( option_acmp==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_ACMP ) {
        struct jdksavdecc_acmpdu acmp;
        if( jdksavdecc_acmpdu_read(&acmp, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            bool allow=true;
            // filter out non-interesting entity id's if we are asked to
            if( jdksavdecc_eui64_is_set(option_entity_eui64) ) {
                allow=false;
                if( jdksavdecc_eui64_compare(&acmp.controller_entity_id, &option_entity_eui64 )) {
                    allow=true;
                } else if( jdksavdecc_eui64_compare(&acmp.talker_entity_id, &option_entity_eui64 )) {
                    allow=true;
                } else if( jdksavdecc_eui64_compare(&acmp.listener_entity_id, &option_entity_eui64 )) {
                    allow=true;
                }
            }

            if( allow ) {
                jdksavdecc_printer_print(&print,"ACMP:");
                jdksavdecc_printer_print_eol(&print);
                jdksavdecc_acmpdu_print(&print, &acmp);
                jdksavdecc_printer_print_eol(&print);
            }
        }
    }
    else if( option_adp==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_ADP ) {
        struct jdksavdecc_adpdu adp;
        if( jdksavdecc_adpdu_read(&adp, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            bool allow=true;
            // filter out non-interesting entity id's if we are asked to
            if( jdksavdecc_eui64_is_set(option_entity_eui64) ) {
                allow=false;
                if( jdksavdecc_eui64_compare(&adp.header.entity_id, &option_entity_eui64 )) {
                    allow=true;
                }
            }
            if( allow ) {
                jdksavdecc_printer_print(&print,"ADP:");
                jdksavdecc_printer_print_eol(&print);
                jdksavdecc_adpdu_print(&print, &adp);
                jdksavdecc_printer_print_eol(&print);
            }
        }
    } else if( option_aecp==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_AECP ) {
        struct jdksavdecc_aecpdu_common aecpdu;
        if( jdksavdecc_aecpdu_common_read(&aecpdu, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            bool allow=true;

            // filter out non-interesting entity id's if we are asked to
            if( jdksavdecc_eui64_is_set(option_entity_eui64) ) {
                allow=false;
                if( jdksavdecc_eui64_compare(&aecpdu.controller_entity_id, &option_entity_eui64 )) {
                    allow=true;
                } else if( jdksavdecc_eui64_compare(&aecpdu.header.target_entity_id, &option_entity_eui64 )) {
                    allow=true;
                }
            }
            if( allow ) {
                jdksavdecc_printer_print(&print,"AECP:");
                jdksavdecc_printer_print_eol(&print);
//              jdksavdecc_aecpdu_common_print(&print, &aecp);
                jdksavdecc_printer_print_block(&print, buf, len, 0, len);
                jdksavdecc_printer_print_eol(&print);
            }
        }
    } else if( option_jdkslog==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_AECP
        && memcmp( &buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET], jdksavdecc_jdks_multicast_log.value, 6 )==0 ) {
        struct jdksavdecc_aecpdu_aem aem;
        if( jdksavdecc_aecpdu_aem_read(&aem, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            if( aem.aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE
                && aem.command_type == 0x8000 + JDKSAVDECC_AEM_COMMAND_SET_CONTROL ) {
                bool allow=true;

                // filter out non-interesting entity id's if we are asked to
                if( jdksavdecc_eui64_is_set(option_entity_eui64) ) {
                    allow=false;
                    if( jdksavdecc_eui64_compare(&aem.controller_entity_id, &option_entity_eui64 )) {
                        allow=true;
                    } else if( jdksavdecc_eui64_compare(&aem.aecpdu_header.header.target_entity_id, &option_entity_eui64 )) {
                        allow=true;
                    }
                }
                if( allow ) {
                    struct jdksavdecc_jdks_log_control log_msg;
                    if( jdksavdecc_jdks_log_control_read(&log_msg,buf,JDKSAVDECC_FRAME_HEADER_LEN,len)>0 ) {
                        fprintf(stdout,"%" PRIx64 ":%" PRIx16 ":%" PRIx16 ":%" PRIx8 ":%s\n",
                            jdksavdecc_eui64_convert_to_uint64( &log_msg.cmd.aem_header.aecpdu_header.header.target_entity_id ),
                            log_msg.source_descriptor_type,
                            log_msg.source_descriptor_index,
                            log_msg.log_detail,
                            log_msg.text );
                    }
                }
            }
        }
    }
    if( *text!=0 ) {
        fprintf( stdout, "%s", text );
    }
}

int main(int argc, const char **argv ) {
    us_platform_init_sockets();
    us_malloc_allocator_init(&allocator);
    us_getopt_init(&options, &allocator.m_base);
    us_getopt_add_list(&options, option, 0, "JDKSAvdeccLogger options");
    us_getopt_fill_defaults(&options);
    us_getopt_parse_args(&options,argv+1);
    jdksavdecc_eui64_init_from_uint64(&option_entity_eui64, option_entity);

    if( option_dump ) {
        us_print_file_t p;
        us_print_file_init(&p, stdout);
        us_getopt_dump(&options, &p.m_base, "dump" );
        return 0;
    }

    if( option_help ) {
        us_print_file_t p;
        us_print_file_init(&p, stdout);
        us_getopt_dump(&options, &p.m_base, 0 );
        return 0;
    }

    us_logger_stdio_start(stdout, stderr);
    us_log_set_level(US_LOG_LEVEL_DEBUG);

    if( us_rawnet_multi_open(
        &multi_rawnet,
        JDKSAVDECC_AVTP_ETHERTYPE,
        jdksavdecc_multicast_adp_acmp.value,
        jdksavdecc_jdks_multicast_log.value)>0 ) {

        while(true) {
            time_t cur_time = time(0);
            if( us_platform_sigint_seen || us_platform_sigterm_seen ) {
                break;
            }
#if defined(WIN32)
            Sleep(100);
#else
            fd_set readable;
            int largest_fd=-1;
            int s;
            struct timeval tv;
            FD_ZERO(&readable);
            largest_fd=us_rawnet_multi_set_fdset(&multi_rawnet, &readable);
            tv.tv_sec = 0;
            tv.tv_usec = 200000; // 200 ms

            do {
                s=select(largest_fd+1, &readable, 0, 0, &tv );
            } while( s<0 && (errno==EINTR || errno==EAGAIN) );

            if( s<0 ) {
                us_log_error( "Unable to select" );
                break;
            }
#endif
            // poll even if select thinks there are no readable sockets
            us_rawnet_multi_rawnet_poll_incoming( &multi_rawnet, cur_time, 1, 0, incoming_packet_handler );
        }

        us_rawnet_multi_close(&multi_rawnet);
    }
    return 0;
}
