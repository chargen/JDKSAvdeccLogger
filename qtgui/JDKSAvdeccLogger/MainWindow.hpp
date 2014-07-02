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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "JDKSAvdeccLogger.hpp"

#include <QMainWindow>
#include <QBasicTimer>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

  public
slots:
    void settingsChanged();

  protected:
    void timerEvent( QTimerEvent *event );

    virtual void incomingAvdeccPacket( us_rawnet_multi_t *self, int ethernet_port, uint8_t *buf, uint16_t len );

  private:
    static void
        incomingAvdeccPacketCallback( us_rawnet_multi_t *self, int ethernet_port, void *context, uint8_t *buf, uint16_t len );

    bool initializing;
    bool global_enable;
    bool entity_id_enabled;
    Ui::MainWindow *ui;
    QBasicTimer timer;
};

#endif // MAINWINDOW_HPP
