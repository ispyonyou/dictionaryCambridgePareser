#ifndef WORDSMODEL_H
#define WORDSMODEL_H

#include <QAbstractTableModel>
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

    void appendWord( std::shared_ptr< WordsData > wordData );

    std::shared_ptr< WordsData > getWordData( long row );

private:
    WordsModelPrivate& d;
};

enum enWordModelHids {
    Hid_WordModel_Word          = 0,
    Hid_WordModel_Transcription = 1,
    Hid_WordModel_IsLearned     = 2,
};


#endif // WORDSMODEL_H
