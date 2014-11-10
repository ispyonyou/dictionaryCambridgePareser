#include "webviewwitheditablesizehint.h"

WebViewWithEditableSizeHint::WebViewWithEditableSizeHint( QWidget* parent )
    : QWebView( parent )
{
    m_SizeHint = QSize( 200, 200 );
}

QSize WebViewWithEditableSizeHint::sizeHint() const
{
    return m_SizeHint;
}

void WebViewWithEditableSizeHint::setSizeHint( QSize sizeHint )
{
    m_SizeHint = sizeHint;
}
