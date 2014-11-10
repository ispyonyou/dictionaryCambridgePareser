QT       += core gui sql widgets network webkitwidgets multimedia

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dictionaryCambridgePareser
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    wordsmodel.cpp \
    cambridgedictionaryparser.cpp \
    ContentProviders/wordscontentprovider.cpp \
    dbutils.cpp \
    ContentProviders/FieldDesc.cpp \
    ContentProviders/sensescontentprovider.cpp \
    ContentProviders/examplescontentprovider.cpp \
    webviewwitheditablesizehint.cpp \
    mp3wrap.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    wordsmodel.h \
    cambridgedictionaryparser.h \
    ContentProviders/wordscontentprovider.h \
    dbutils.h \
    ContentProviders/FieldDesc.h \
    CopyClearBase.h \
    ContentProviders/sensescontentprovider.h \
    ContentProviders/wordsdata.h \
    ContentProviders/sencesdata.h \
    ContentProviders/examplescontentprovider.h \
    ContentProviders/examplesdata.h \
    webviewwitheditablesizehint.h \
    mp3wrap.h

FORMS    += \
    settings.ui \
    mainwindow.ui

RESOURCES += \
    resources.qrc
