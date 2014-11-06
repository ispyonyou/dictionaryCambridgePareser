#-------------------------------------------------
#
# Project created by QtCreator 2014-11-03T16:45:44
#
#-------------------------------------------------

QT       += core gui sql network webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dictionaryCambridgePareser
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    settingsdialog.cpp \
    wordsmodel.cpp \
    cambridgedictionaryparser.cpp

HEADERS  += widget.h \
    settingsdialog.h \
    wordsmodel.h \
    cambridgedictionaryparser.h

FORMS    += widget.ui \
    settings.ui
