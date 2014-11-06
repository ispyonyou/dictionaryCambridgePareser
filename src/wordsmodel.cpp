#include "wordsmodel.h"

class WordsModelPrivate
{
public:
    WordsModelPrivate( WordsModel* _q )
        : q( *_q )
    { }

public:
    WordsModel& q;
    QList< std::shared_ptr< WordData > > Words;
};

WordsModel::WordsModel( QObject* parent )
    : QAbstractTableModel( parent )
    , d( *new WordsModelPrivate( this ) )
{}

WordsModel::~WordsModel()
{
    delete &d;
}

int WordsModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    return d.Words.size();
}

int WordsModel::columnCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    return 3;
}

QVariant WordsModel::data( const QModelIndex& index, int role ) const
{
    if( !index.isValid() )
        return QVariant();

    if( index.row() >= d.Words.size() || index.row() < 0 )
        return QVariant();

    if( Qt::DisplayRole == role ) {
        std::shared_ptr< WordData > wordData = d.Words.at( index.row() );

        switch( index.column() ) {
            case Hid_WordModel_Word:
                return wordData->word;
            case Hid_WordModel_Transcription:
                return wordData->transcription;
            case Hid_WordModel_IsLearned:
                return wordData->isLearned;
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QVariant WordsModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if( Qt::DisplayRole != role )
        return QVariant();

    if( Qt::Horizontal == orientation ) {
        switch( section ) {
            case Hid_WordModel_Word:
                return tr( "Word" );
            case Hid_WordModel_Transcription:
                return tr( "Transcription" );
            case Hid_WordModel_IsLearned:
                return tr( "Is Learned" );
            default:
                return QVariant();
        }
    }

    return QVariant();
}

Qt::ItemFlags WordsModel::flags( const QModelIndex& index ) const
{
    if( !index.isValid() )
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags( index ) | Qt::ItemIsEditable;
}

bool WordsModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    Q_UNUSED( value );
    if (index.isValid() && role == Qt::EditRole) {
//        int row = index.row();
//
//        QPair<QString, QString> p = listOfPairs.value(row);
//
//        if (index.column() == 0)
//            p.first = value.toString();
//        else if (index.column() == 1)
//            p.second = value.toString();
//        else
//            return false;
//
//        listOfPairs.replace(row, p);
//        emit(dataChanged(index, index));

        return true;
    }

    return false;
}

bool WordsModel::insertRows( int position, int rows, const QModelIndex& index )
{
    Q_UNUSED( index );
    beginInsertRows( QModelIndex(), position, position + rows - 1 );

    for( int row = 0; row < rows; ++row ) {
        std::shared_ptr< WordData > word( new WordData() );
        d.Words.insert( position, word );
    }

    endInsertRows();
    return true;
}

bool WordsModel::removeRows( int position, int rows, const QModelIndex& index )
{
    Q_UNUSED( index );
    beginRemoveRows( QModelIndex(), position, position + rows - 1 );

    for (int row = 0; row < rows; ++row) {
        d.Words.removeAt( position );
    }

    endRemoveRows();
    return true;
}

void WordsModel::appendWord( std::shared_ptr< WordData > wordData )
{
    beginInsertRows( QModelIndex(), d.Words.size(), d.Words.size() );
    d.Words.append( wordData );
    endInsertRows();
}

std::shared_ptr< WordData > WordsModel::getWordData( long row )
{
    return d.Words[ row ];
}

