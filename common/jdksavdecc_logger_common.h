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

#pragma once

#include "jdksavdecc_logger_common.h"
#include "us_world.h"
#include "jdksavdecc.h"
#include "jdksavdecc_adp_print.h"
#include "us_rawnet.h"
#include "us_allocator.h"
#include "us_rawnet_multi.h"
#include "us_logger_stdio.h"
#include "us_getopt.h"

#ifdef __cplusplus
extern "C" {
#endif

extern us_malloc_allocator_t jdksavdecc_logger_allocator;
extern us_rawnet_multi_t multi_rawnet;
extern bool option_help;
extern bool option_dump;
extern int16_t option_adp;
extern int16_t option_acmp;
extern int16_t option_aecp;
extern int16_t option_jdkslog;
extern uint64_t option_entity;
extern struct jdksavdecc_eui64 option_entity_eui64;
extern us_getopt_t options;
extern us_getopt_option_t option[];

bool jdksavdecc_logger_init( const char **argv );
void jdksavdecc_logger_destroy( void );

void jdksavdecc_logger_print_aecp_frame( struct jdksavdecc_printer *print,
                                         struct timeval *tv,
                                         uint8_t const *buf,
                                         uint16_t len );

void
    jdksavdecc_logger_print_adp_frame( struct jdksavdecc_printer *print, struct timeval *tv, uint8_t const *buf, uint16_t len );

void jdksavdecc_logger_print_acmp_frame( struct jdksavdecc_printer *print,
                                         struct timeval *tv,
                                         uint8_t const *buf,
                                         uint16_t len );

bool jdksavdecc_logger_print_jdkslog_frame( struct jdksavdecc_printer *print,
                                            struct timeval *tv,
                                            uint8_t const *buf,
                                            uint16_t len );

#ifdef __cplusplus
}
#endif
