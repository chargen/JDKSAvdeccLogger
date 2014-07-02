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

void incoming_packet_handler( us_rawnet_multi_t *self, int ethernet_port, void *context, uint8_t *buf, uint16_t len )
{
    (void)ethernet_port;
    (void)context;
    char text[4096] = "";
    struct timeval tv;
    us_gettimeofday( &tv );
    struct jdksavdecc_printer print;
    jdksavdecc_printer_init( &print, text, sizeof( text ) );

    if ( option_jdkslog == 1 && buf[JDKSAVDECC_FRAME_HEADER_LEN + 0] == 0x80 + JDKSAVDECC_SUBTYPE_AECP )
    {
        jdksavdecc_logger_print_jdkslog_frame( &print, &tv, buf, len );
    }
    else if ( option_acmp == 1 && buf[JDKSAVDECC_FRAME_HEADER_LEN + 0] == 0x80 + JDKSAVDECC_SUBTYPE_ACMP )
    {
        jdksavdecc_logger_print_acmp_frame( &print, &tv, buf, len );
    }
    else if ( option_adp == 1 && buf[JDKSAVDECC_FRAME_HEADER_LEN + 0] == 0x80 + JDKSAVDECC_SUBTYPE_ADP )
    {
        jdksavdecc_logger_print_adp_frame( &print, &tv, buf, len );
    }
    else if ( option_aecp == 1 && buf[JDKSAVDECC_FRAME_HEADER_LEN + 0] == 0x80 + JDKSAVDECC_SUBTYPE_AECP )
    {
        jdksavdecc_logger_print_aecp_frame( &print, &tv, buf, len );
    }
    if ( *text != 0 )
    {
        fprintf( stdout, "%d.%d:%s\n", (int)tv.tv_sec, (int)tv.tv_usec, text );
    }
}

int main( int argc, const char **argv )
{
    if ( jdksavdecc_logger_init( argv ) )
    {
        while ( true )
        {
            time_t cur_time = time( 0 );
            if ( us_platform_sigint_seen || us_platform_sigterm_seen )
            {
                break;
            }
#if defined( WIN32 )
            Sleep( 100 );
#else
            fd_set readable;
            int largest_fd = -1;
            int s;
            struct timeval tv;
            FD_ZERO( &readable );
            largest_fd = us_rawnet_multi_set_fdset( &multi_rawnet, &readable );
            tv.tv_sec = 0;
            tv.tv_usec = 200000; // 200 ms

            do
            {
                s = select( largest_fd + 1, &readable, 0, 0, &tv );
            } while ( s < 0 && ( errno == EINTR || errno == EAGAIN ) );

            if ( s < 0 )
            {
                us_log_error( "Unable to select" );
                break;
            }
#endif
            // poll even if select thinks there are no readable sockets
            us_rawnet_multi_rawnet_poll_incoming( &multi_rawnet, cur_time, 128, 0, incoming_packet_handler );
        }

        jdksavdecc_logger_destroy();
    }
    return 0;
}
