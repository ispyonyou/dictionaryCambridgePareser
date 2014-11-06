#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class WordsModel;

class MainWidgetPrivate;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public Q_SLOTS:
    void getItClicked();
    void settingsClicked();
    void addWordClicked();

private:
    Ui::Widget *ui;
    WordsModel* wordsModel;

    MainWidgetPrivate& d;
};

#endif // WIDGET_H
