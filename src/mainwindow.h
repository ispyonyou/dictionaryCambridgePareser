#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>

class MainWindowPrivate;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public Q_SLOTS:
    void generateMp3andHtml();
    void showSettings();
    void addWord( const QString& word );
    void playCurrentWord();
    void removeCurrentWord();
    void wordsTableSelectionChanged();

private:
    MainWindowPrivate& d;
};

#endif // WIDGET_H
