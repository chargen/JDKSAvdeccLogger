#-------------------------------------------------
#
# Project created by QtCreator 2014-01-07T20:10:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JDKSAvdeccLogger
TEMPLATE = app

macx {
    DEFINES+=US_ENABLE_PCAP=1 TARGET_PLATFORM_POSIX=1 US_ENABLE_RAW_ETHERNET=1
    LIBS+=-lpcap
    ICON = jdks.icns
}

unix:!macx {
    DEFINES+=US_ENABLE_RAW_ETHERNET=1 TARGET_PLATFORM_POSIX=1
}

win32 {
    DEFINES+=US_ENABLE_RAW_ETHERNET=1 US_ENABLE_PCAP=1 TARGET_PLATFORM_WIN32=1
}

win32-msvc*:contains(QMAKE_TARGET.arch, x86_64):{
    message( "Building for 64 bit")
    LIBS+=-L$$(WCAP_DIR)/x64
}

win32-msvc*:contains(QMAKE_TARGET.arch, x86):{
    message( "Building for 64 bit")
    LIBS+=-L$$(WCAP_DIR)/Lib
}

COMMON_DIR=$$_PRO_FILE_PWD_/../../common
JDKSAVDECC_C_DIR=$$_PRO_FILE_PWD_/../../jdksavdecc-c
JDKSAVDECC_C_INCLUDE_DIR=$$JDKSAVDECC_C_DIR/include
JDKSAVDECC_C_SOURCE_DIR=$$JDKSAVDECC_C_DIR/src

MICROSUPPORT_DIR=$$_PRO_FILE_PWD_/../../microsupport
MICROSUPPORT_INCLUDE_DIR=$$MICROSUPPORT_DIR/include
MICROSUPPORT_SOURCE_DIR=$$MICROSUPPORT_DIR/src

INCLUDEPATH+=$$JDKSAVDECC_C_INCLUDE_DIR
INCLUDEPATH+=$$MICROSUPPORT_INCLUDE_DIR
INCLUDEPATH+=$$COMMON_DIR
DEPENDPATH+=$$JDKSAVDECC_C_INCLUDE_DIR $$MICROSUPPORT_INCLUDE_DIR $$COMMON_DIR $$_PRO_FILE_PWD_

JDKSAVDECC_C_SOURCES += \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_acmp_print.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_adp_print.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_aecp_print.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_aem_descriptor.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_aem_print.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_app.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_app_print.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_frame.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_maap.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_maap_print.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_pdu.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_pdu_print.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_print.c \
    $$JDKSAVDECC_C_SOURCE_DIR/jdksavdecc_jdks.c

JDKSAVDECC_C_HEADERS += \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_acmp.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_acmp_print.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_adp.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_adp_print.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aecp.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aecp_aa.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aecp_aem.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aecp_avc.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aecp_hdcp_apm.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aecp_print.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aecp_vendor.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aem_command.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aem_descriptor.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_aem_print.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_app.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_app_print.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_avr.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_descriptor_storage.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_descriptor_storage_gen.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_frame.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_jdks.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_maap.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_maap_print.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_ms.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_msinttypes.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_msstdint.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_pdu.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_pdu_print.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_print.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_util.h \
    $$JDKSAVDECC_C_INCLUDE_DIR/jdksavdecc_world.h

MICROSUPPORT_SOURCES += \
    $$MICROSUPPORT_SOURCE_DIR/us_allocator.c  \
    $$MICROSUPPORT_SOURCE_DIR/us_buffer.c \
    $$MICROSUPPORT_SOURCE_DIR/us_buffer_print.c \
    $$MICROSUPPORT_SOURCE_DIR/us_getopt.c \
    $$MICROSUPPORT_SOURCE_DIR/us_logger.c \
    $$MICROSUPPORT_SOURCE_DIR/us_logger_printer.c \
    $$MICROSUPPORT_SOURCE_DIR/us_logger_stdio.c \
    $$MICROSUPPORT_SOURCE_DIR/us_logger_syslog.c  \
    $$MICROSUPPORT_SOURCE_DIR/us_parse.c \
    $$MICROSUPPORT_SOURCE_DIR/us_platform.c \
    $$MICROSUPPORT_SOURCE_DIR/us_print.c \
    $$MICROSUPPORT_SOURCE_DIR/us_rawnet.c \
    $$MICROSUPPORT_SOURCE_DIR/us_time.c \
    $$MICROSUPPORT_SOURCE_DIR/us_rawnet_multi.c \
    $$MICROSUPPORT_SOURCE_DIR/us_world.c \
    $$MICROSUPPORT_SOURCE_DIR/us_net.c

MICROSUPPORT_HEADERS += \
    $$MICROSUPPORT_INCLUDE_DIR/us_rawnet.h \
    $$MICROSUPPORT_INCLUDE_DIR/us_rawnet_multi.h \
    $$MICROSUPPORT_INCLUDE_DIR/us_net.h \
    $$MICROSUPPORT_INCLUDE_DIR/us_getopt.h \
    $$MICROSUPPORT_INCLUDE_DIR/us_options.h \
    $$MICROSUPPORT_INCLUDE_DIR/us_platform.h \
    $$MICROSUPPORT_INCLUDE_DIR/us_world.h


SOURCES +=\
        MainWindow.cpp \
        $$JDKSAVDECC_C_SOURCES \
        $$MICROSUPPORT_SOURCES \
        JDKSAvdeccLogger.cpp \
        $$COMMON_DIR/jdksavdecc_logger_common.c

HEADERS  += MainWindow.hpp \
        $$JDKSAVDECC_C_HEADERS \
        $$MICROSUPPORT_HEADERS \
        JDKSAvdeccLogger.hpp \
        $$COMMON_DIR/jdksavdecc_logger_common.h

FORMS    += MainWindow.ui
