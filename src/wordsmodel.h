#ifndef WORDSMODEL_H
#define WORDSMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include "ContentProviders/wordsdata.h"
#include <memory>

class WordsModelPrivate;

class WordsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    WordsModel( QObject* parent = nullptr );
    ~WordsModel();

    int rowCount( const QModelIndex& parent ) const;
    int columnCount( const QModelIndex& parent ) const;
    QVariant data( const QModelIndex& index, int role ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );
    bool insertRows( int position, int rows, const QModelIndex& index = QModelIndex() );
    bool removeRows( int position, int rows, const QModelIndex& index = QModelIndex() );

    int appendWord( std::shared_ptr< WordsData > wordData );

    std::shared_ptr< WordsData > getWordData( long row );
    std::shared_ptr< WordsData > getWordDataById( int id );

    enum {
        WordIdRole = Qt::UserRole + 1,
    };

private:
    WordsModelPrivate& d;
};

enum enWordModelHids {
    Hid_WordModel_Word          = 0,
    Hid_WordModel_Transcription = 1,
    Hid_WordModel_IsLearned     = 2,
};

class WordsSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    WordsSortFilterProxyModel( QObject* parent = 0 );

    void setNeedShowLearned( bool val );

protected:
    bool filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent) const;
//    bool lessThan( const QModelIndex& left, const QModelIndex& right) const;

private:
    bool needShowLearned;
};

#endif // WORDSMODEL_H
