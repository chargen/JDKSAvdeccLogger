/*
  Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   3. Neither the name of J.D. Koftinoff Software, Ltd. nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "jdksavdecc_sender_common.h"
#include "jdksavdecc_aem_print.h"

#ifdef __cplusplus
extern "C" {
#endif

us_rawnet_multi_t multi_rawnet;
bool option_help=false;
bool option_help_default=false;
bool option_dump=false;
bool option_dump_default=false;

uint64_t option_target_entity_id=0xffffffffffffffffULL;
struct jdksavdecc_eui64 option_target_entity_eui64;
uint64_t option_target_entity_id_default=0xffffffffffffffffULL;

uint64_t option_controller_entity_id=0xffffffffffffffffULL;
struct jdksavdecc_eui64 option_controller_entity_eui64;
uint64_t option_controller_entity_id_default=0xffffffffffffffffULL;

us_malloc_allocator_t jdksavdecc_logger_allocator;
us_getopt_t options;
us_getopt_option_t option[] = {
    {"dump","Dump settings to stdout", US_GETOPT_FLAG, &option_dump_default, &option_dump },
    {"help","Show help", US_GETOPT_FLAG, &option_help_default, &option_help },
    {"entity","Limit to only receive messages involving this entity", US_GETOPT_HEX64, &option_target_entity_id_default, &option_target_entity_id },
    {0,0,US_GETOPT_NONE,0,0}
};

bool jdksavdecc_logger_init(const char **argv) {
    us_platform_init_sockets();
    us_malloc_allocator_init(&jdksavdecc_sender_allocator);
    us_getopt_init(&options, &jdksavdecc_sender_allocator.m_base);
    us_getopt_add_list(&options, option, 0, "JDKSAvdeccSender options");
    us_getopt_fill_defaults(&options);
    us_getopt_parse_args(&options,argv+1);
    jdksavdecc_eui64_init_from_uint64(&option_target_entity_eui64, option_target_entity_id);

    if( option_dump ) {
        us_print_file_t p;
        us_print_file_init(&p, stdout);
        us_getopt_dump(&options, &p.m_base, "dump" );
        return false;
    }

    if( option_help ) {
        us_print_file_t p;
        us_print_file_init(&p, stdout);
        us_getopt_dump(&options, &p.m_base, 0 );
        return false;
    }

    us_logger_stdio_start(stdout, stderr);
    us_log_set_level(US_LOG_LEVEL_WARN);

    return us_rawnet_multi_open(
        &multi_rawnet,
        JDKSAVDECC_AVTP_ETHERTYPE,
        jdksavdecc_multicast_adp_acmp.value,
        jdksavdecc_jdks_multicast_log.value)>0;
}

void jdksavdecc_sender_destroy(void) {
    us_rawnet_multi_close(&multi_rawnet);
}


bool jdksavdecc_sender_received_aecp_frame(
        uint16_t expected_sequence_id,
        struct jdksavdecc_printer *print,
        struct timeval *tv,
        uint8_t const *buf,
        uint16_t len ) {
    (void)tv;
    if( buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_AECP ) {
        struct jdksavdecc_aecpdu_common aecpdu;
        if( jdksavdecc_aecpdu_common_read(&aecpdu, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            bool allow=true;

            allow=false;
            if( (aecpdu.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE) &&
                (jdksavdecc_eui64_compare(
                    &aecpdu.controller_entity_id, &option_controller_entity_eui64 )==0) &&
                (jdksavdecc_eui64_compare(&aecpdu.header.target_entity_id, &option_target_entity_eui64)==0)
            ) {
                if( aecpdu.sequence_id == expected_sequence_id ) {
                    allow=true;
                }
            }

            if( allow ) {
                jdksavdecc_printer_print(print,"AECP:");
                jdksavdecc_printer_print_eol(print);
                jdksavdecc_aecp_print(print, &aecpdu, buf, JDKSAVDECC_FRAME_HEADER_LEN, len );
            }
        }
    }
}


bool jdksavdecc_sender_received_acmp_frame(
        uint16_t expected_sequence_id,
        struct jdksavdecc_printer *print,
        struct timeval *tv,
        uint8_t const *buf,
        uint16_t len ) {
    (void)tv;
    if( buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_ACMP ) {
        struct jdksavdecc_acmpdu acmp;
        if( jdksavdecc_acmpdu_read(&acmp, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            bool allow=false;

            if( (jdksavdecc_eui64_compare(
                    &acmp.controller_entity_id,
                    &option_controller_entity_eui64 )==0) && acmp.sequence_id == expected_sequence_id ) {
                if( (acmp.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_RESPONSE) ||
                    (acmp.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_RESPONSE) ||
                    (acmp.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_RESPONSE) )
                {
                    if( jdksavdecc_eui64_compare( &acmp.listener_entity_id, &option_target_listener_entity_eui64 )==0 ) {
                        if( acmp.talker_unique_id == option_listener_unique_id ) {
                            allow=true;
                        }
                    }
                } else if( (acmp.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_TX_RESPONSE) ||
                    (acmp.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_TX_RESPONSE) ||
                    (acmp.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_RESPONSE) ||
                    (acmp.header.message_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_RESPONSE) )
                {
                    if( jdksavdecc_eui64_compare( &acmp.talker_entity_id, &option_target_talker_entity_eui64 )==0 ) {
                        if( acmp.talker_unique_id == option_talker_unique_id ) {
                            allow=true;
                        }
                    }
                }
            }

            if( allow ) {
                jdksavdecc_printer_print(print,"ACMP:");
                jdksavdecc_printer_print_eol(print);
                jdksavdecc_acmpdu_print(print, &acmp);
                jdksavdecc_printer_print_eol(print);
            }
        }
    }
}


#ifdef __cplusplus
}
#endif

