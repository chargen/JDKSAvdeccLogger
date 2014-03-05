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

    if( option_jdkslog==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_AECP ) {
        jdksavdecc_logger_print_jdkslog_frame(&print,&tv,buf,len);
    } else if( option_acmp==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_ACMP ) {
        jdksavdecc_logger_print_acmp_frame(&print,&tv,buf,len);
    }
    else if( option_adp==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_ADP ) {
        jdksavdecc_logger_print_adp_frame(&print,&tv,buf,len);
    } else if( option_aecp==1 && buf[JDKSAVDECC_FRAME_HEADER_LEN+0]==0x80+JDKSAVDECC_SUBTYPE_AECP ) {
        jdksavdecc_logger_print_aecp_frame(&print,&tv,buf,len);
    }
    if( *text!=0 ) {
        QString timestamp = QString("%1.%2").arg(tv.tv_sec).arg(tv.tv_usec);
        ui->commonLogText->append(QString("%1:%2").arg(timestamp,17).arg(text));
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
