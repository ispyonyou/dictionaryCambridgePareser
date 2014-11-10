QT       += core gui sql widgets network webkitwidgets multimedia

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dictionaryCambridgePareser
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        widget.cpp \
    settingsdialog.cpp \
    wordsmodel.cpp \
    cambridgedictionaryparser.cpp \
    third-party/mp3wrap/crc.c \
    third-party/mp3wrap/getopt.c \
    third-party/mp3wrap/mp3wrap.c \
    third-party/mp3wrap/wrap.c \
    ContentProviders/wordscontentprovider.cpp \
    dbutils.cpp \
    ContentProviders/FieldDesc.cpp \
    ContentProviders/sensescontentprovider.cpp \
    ContentProviders/examplescontentprovider.cpp \
    webviewwitheditablesizehint.cpp

HEADERS  += widget.h \
    settingsdialog.h \
    wordsmodel.h \
    cambridgedictionaryparser.h \
    third-party/mp3wrap/crc.h \
    third-party/mp3wrap/getopt.h \
    third-party/mp3wrap/mp3wrap.h \
    third-party/mp3wrap/wrap.h \
    ContentProviders/wordscontentprovider.h \
    dbutils.h \
    ContentProviders/FieldDesc.h \
    CopyClearBase.h \
    ContentProviders/sensescontentprovider.h \
    ContentProviders/wordsdata.h \
    ContentProviders/sencesdata.h \
    ContentProviders/examplescontentprovider.h \
    ContentProviders/examplesdata.h \
    webviewwitheditablesizehint.h

FORMS    += widget.ui \
    settings.ui \
    mainwindow.ui
