/*
 Copyright (c) 2014, J.D. Koftinoff Software, Ltd. <jeffk@jdkoftinoff.com>
 http://www.jdkoftinoff.com/
 All rights reserved.

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/


#ifndef JDKSAVDECCLOGGER_HPP
#define JDKSAVDECCLOGGER_HPP

#include "us_world.h"
#include "jdksavdecc.h"
#include "jdksavdecc_adp_print.h"
#include "us_rawnet.h"
#include <inttypes.h>
#include <stdint.h>
#include <time.h>
#include "us_allocator.h"
#include "us_rawnet_multi.h"
#include "us_logger_stdio.h"
#include "us_getopt.h"


extern us_rawnet_multi_t multi_rawnet;
extern bool option_help;
extern bool option_dump;
extern int16_t option_adp;
extern int16_t option_acmp;
extern int16_t option_aecp;
extern int16_t option_jdkslog;
extern uint64_t option_entity;
extern struct jdksavdecc_eui64 option_entity_eui64;

#endif // JDKSAVDECCLOGGER_HPP
