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

#include "JDKSAvdeccLogger.hpp"
#include "MainWindow.hpp"
#include <QApplication>

us_rawnet_multi_t multi_rawnet;
bool option_help=false;
bool option_help_default=false;
bool option_dump=false;
bool option_dump_default=false;
int16_t option_adp=true;
int16_t option_adp_default=true;
int16_t option_acmp=true;
int16_t option_acmp_default=true;
int16_t option_aecp=true;
int16_t option_aecp_default=true;
int16_t option_jdkslog=true;
int16_t option_jdkslog_default=true;
uint64_t option_entity=0xffffffffffffffffULL;
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
    {0,0,US_GETOPT_NONE,0,0}
};


int main(int argc, char const *argv[])
{
    int r=255;
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
    us_log_set_level(US_LOG_LEVEL_WARN);

    if( us_rawnet_multi_open(
        &multi_rawnet,
        JDKSAVDECC_AVTP_ETHERTYPE,
        jdksavdecc_multicast_adp_acmp.value,
        jdksavdecc_jdks_multicast_log.value)>0 ) {

        QApplication a(argc, (char **)argv);
        MainWindow w;
        w.show();
        r=a.exec();
        us_rawnet_multi_close(&multi_rawnet);
    }
    return r;
}
