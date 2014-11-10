#ifndef WEBVIEWWITHEDITABLESIZEHINT_H
#define WEBVIEWWITHEDITABLESIZEHINT_H

#include <QWebView>

class WebViewWithEditableSizeHint : public QWebView
{
public:
    explicit WebViewWithEditableSizeHint( QWidget* parent = 0 );

    QSize sizeHint() const override;

    void setSizeHint( QSize sizeHint );

private:
    QSize m_SizeHint;
};

#endif // WEBVIEWWITHEDITABLESIZEHINT_H
