#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class WordsModel;

class MainWidgetPrivate;

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    int currentRow();

public Q_SLOTS:
    void getItClicked();
    void settingsClicked();
    void addWordClicked();
    void playClicked();
    void wordsTableSelectionChanged();

private:
    Ui::MainWindow *ui;
    WordsModel* wordsModel;

    MainWidgetPrivate& d;
};

#endif // WIDGET_H
