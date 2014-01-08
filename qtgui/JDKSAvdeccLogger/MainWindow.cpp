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
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    initializing(true),
    global_enable(true),
    entity_id_enabled(jdksavdecc_eui64_is_set(option_entity_eui64)),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->checkBoxADP->setChecked(option_adp);
    ui->checkBoxACMP->setChecked(option_acmp);
    ui->checkBoxAECP->setChecked(option_aecp);
    ui->checkBoxJDKSLOG->setChecked(option_jdkslog);
    ui->checkBoxEntityID->setChecked( entity_id_enabled );
    if( entity_id_enabled ) {
        ui->lineEditEntityID->setText( QString::fromLatin1( "0x%1" ).arg( option_entity, 16, 16, QChar::fromLatin1('0')));
    }
    timer.start(100,this);
    initializing=false;
}

MainWindow::~MainWindow() {
    timer.stop();
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *) {
    if( !initializing ) {
        time_t cur_time = time(0);
        us_rawnet_multi_rawnet_poll_incoming(
                    &multi_rawnet,
                    cur_time,
                    1024,
                    this,
                    incomingAvdeccPacketCallback );
    }
}

void MainWindow::settingsChanged() {
    if( !initializing ) {
        global_enable = ui->pushButtonEnable->isChecked();
        option_adp = ui->checkBoxADP->isChecked();
        option_acmp = ui->checkBoxACMP->isChecked();
        option_aecp = ui->checkBoxAECP->isChecked();
        option_jdkslog = ui->checkBoxJDKSLOG->isChecked();
        if( ui->checkBoxEntityID->isChecked() ) {
            bool ok=false;
            option_entity = ui->lineEditEntityID->text().toULongLong(&ok,16);
            if( ok ) {
                jdksavdecc_eui64_init_from_uint64(&option_entity_eui64,option_entity);
                ui->lineEditEntityID->setText( QString::fromLatin1( "0x%1" ).arg( option_entity, 16, 16, QChar::fromLatin1('0')));
            }
        } else {
            jdksavdecc_eui64_init(&option_entity_eui64);
        }
        // TODO: Store config
    }
}


void MainWindow::incomingAvdeccPacket(
        us_rawnet_multi_t *multiport,
        int ethernet_port,
        uint8_t *buf,
        uint16_t len ) {
    (void)multiport;
    (void)ethernet_port;
    char text[4096]="";
    struct timeval tv;
    us_gettimeofday(&tv);
    struct jdksavdecc_printer print;
    jdksavdecc_printer_init(&print, text, sizeof(text));

    if( option_jdkslog==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_AECP
        && memcmp( &buf[JDKSAVDECC_FRAME_HEADER_DA_OFFSET], jdksavdecc_jdks_multicast_log.value, 6 )==0 ) {
        struct jdksavdecc_aecpdu_aem aem;
        if( jdksavdecc_aecpdu_aem_read(&aem, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            if( aem.aecpdu_header.header.message_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE
                && aem.command_type == 0x8000 + JDKSAVDECC_AEM_COMMAND_SET_CONTROL ) {
                bool allow=true;

                // filter out non-interesting entity id's if we are asked to
                if( jdksavdecc_eui64_is_set(option_entity_eui64) ) {
                    allow=false;
                    if( jdksavdecc_eui64_compare(&aem.aecpdu_header.header.target_entity_id, &option_entity_eui64 )==0) {
                        allow=true;
                    }
                }
                if( jdksavdecc_eui64_compare(&aem.aecpdu_header.controller_entity_id, &jdksavdecc_jdks_notifications_controller_entity_id )==0) {
                    allow=true;
                }
                if( allow ) {
                    struct jdksavdecc_jdks_log_control log_msg;
                    if( jdksavdecc_jdks_log_control_read(&log_msg,buf,JDKSAVDECC_FRAME_HEADER_LEN,len)>0 ) {
                        const char *level;
                        uint64_t target_entity_id = jdksavdecc_eui64_convert_to_uint64( &log_msg.cmd.aem_header.aecpdu_header.header.target_entity_id);
                        switch (log_msg.log_detail) {
                        case JDKSAVDECC_JDKS_LOG_ERROR:
                            level = "ERROR";
                            break;
                        case JDKSAVDECC_JDKS_LOG_WARNING:
                            level = "WARNING";
                            break;
                        case JDKSAVDECC_JDKS_LOG_INFO:
                            level = "INFO";
                            break;
                        case JDKSAVDECC_JDKS_LOG_DEBUG1:
                            level = "DEBUG1";
                            break;
                        case JDKSAVDECC_JDKS_LOG_DEBUG2:
                            level = "DEBUG2";
                            break;
                        case JDKSAVDECC_JDKS_LOG_DEBUG3:
                            level = "DEBUG3";
                            break;
                        default:
                            level = "unknown";
                            break;
                        }
                        //snprintf(text,sizeof(text),"JDKSLOG:%-8s:0x%016" PRIx64 ":%04" PRIx16 ":%04" PRIx16 ":%04" PRIx16 ":%s",
                        snprintf(text,sizeof(text),"JDKSLOG:%-8s:0x%016lx:%04x:%04x:%04x:%s",
                            level,
                            target_entity_id,
                            log_msg.source_descriptor_type,
                            log_msg.source_descriptor_index,
                            log_msg.log_sequence_id,
                            log_msg.text );
                    }
                }
            }
        }
    } else if( option_acmp==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_ACMP ) {
        struct jdksavdecc_acmpdu acmp;
        if( jdksavdecc_acmpdu_read(&acmp, buf, JDKSAVDECC_FRAME_HEADER_LEN, len )>0 ) {
            bool allow=true;
            // filter out non-interesting entity id's if we are asked to
            if( jdksavdecc_eui64_is_set(option_entity_eui64) ) {
                allow=false;
                if( jdksavdecc_eui64_compare(&acmp.controller_entity_id, &option_entity_eui64 )==0) {
                    allow=true;
                } else if( jdksavdecc_eui64_compare(&acmp.talker_entity_id, &option_entity_eui64 )==0) {
                    allow=true;
                } else if( jdksavdecc_eui64_compare(&acmp.listener_entity_id, &option_entity_eui64 )==0) {
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
                if( jdksavdecc_eui64_compare(&adp.header.entity_id, &option_entity_eui64 )==0) {
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
                if( jdksavdecc_eui64_compare(&aecpdu.controller_entity_id, &option_entity_eui64 )==0) {
                    allow=true;
                } else if( jdksavdecc_eui64_compare(&aecpdu.header.target_entity_id, &option_entity_eui64 )==0) {
                    allow=true;
                }
            }
            if( allow ) {
                jdksavdecc_printer_print(&print,"AECP:");
                jdksavdecc_printer_print_eol(&print);
                jdksavdecc_aecp_print(&print, &aecpdu, buf, JDKSAVDECC_FRAME_HEADER_LEN, len );
            }
        }
    }
    if( *text!=0 ) {
        ui->commonLogText->append(QString("%1.%2:%3").arg(tv.tv_sec,10).arg(tv.tv_usec,7).arg(text));
    }

}

void MainWindow::incomingAvdeccPacketCallback(
        us_rawnet_multi_t *self,
        int ethernet_port,
        void *context,
        uint8_t *buf,
        uint16_t len ) {
    MainWindow *win = (MainWindow *)context;
    if( win->global_enable ) {
        win->incomingAvdeccPacket(
                    self,
                    ethernet_port,
                    buf,
                    len );
    }
}
