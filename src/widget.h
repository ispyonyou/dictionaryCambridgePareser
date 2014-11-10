#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>

class MainWidgetPrivate;

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    int currentRow();

public Q_SLOTS:
    void generateMp3andHtml();
    void showSettings();
    void addWordClicked();
    void playCurrentWord();
    void wordsTableSelectionChanged();

private:
    MainWidgetPrivate& d;
};

#endif // WIDGET_H
