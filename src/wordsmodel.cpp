#include "wordsmodel.h"
#include "ContentProviders/wordscontentprovider.h"

class WordsModelPrivate
{
public:
    WordsModelPrivate( WordsModel* _q )
        : q( *_q )
    { }

public:
    WordsModel& q;
    QList< std::shared_ptr< WordsData > > Words;
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
        std::shared_ptr< WordsData > wordData = d.Words.at( index.row() );

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
    else if( Qt::CheckStateRole == role ) {
        std::shared_ptr< WordsData > wordData = d.Words.at( index.row() );

        switch( index.column() ) {
            case Hid_WordModel_IsLearned:
                return wordData->isLearned ? Qt::Checked : Qt::Unchecked;

            default:
                return QVariant();
        }
    }
    else if( WordIdRole == role ) {
        return d.Words.at( index.row() )->id;
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

    Qt::ItemFlags ret = QAbstractTableModel::flags( index );

    if( index.column() == Hid_WordModel_IsLearned )
        ret |= Qt::ItemIsUserCheckable;

    return ret;
}

bool WordsModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if (index.isValid() && role == Qt::EditRole) {
        return true;
    }

    if( index.isValid() && role == Qt::CheckStateRole ) {
        if( index.column() == Hid_WordModel_IsLearned ) {
            std::shared_ptr< WordsData > wordData = d.Words.at( index.row() );
            wordData->isLearned = ( value.toInt() == Qt::Checked );

            WordsContentProvider wordsProvider;
            if( wordsProvider.updateIsLearned( wordData ) )
                emit dataChanged( index, index );
        }

        return true;
    }

    return false;
}

bool WordsModel::insertRows( int position, int rows, const QModelIndex& index )
{
    Q_UNUSED( index );
    beginInsertRows( QModelIndex(), position, position + rows - 1 );

    for( int row = 0; row < rows; ++row ) {
        std::shared_ptr< WordsData > word( new WordsData() );
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

int WordsModel::appendWord( std::shared_ptr< WordsData > wordData )
{
    beginInsertRows( QModelIndex(), d.Words.size(), d.Words.size() );
    d.Words.append( wordData );
    endInsertRows();

    return d.Words.size() - 1;
}

std::shared_ptr< WordsData > WordsModel::getWordData( long row )
{
    return d.Words[ row ];
}

std::shared_ptr< WordsData > WordsModel::getWordDataById( int id )
{
    auto it = std::find_if( d.Words.begin(), d.Words.end(), [=]( const std::shared_ptr< WordsData >& w ){ return w->id == id; } );
    if( it != d.Words.end() )
        return *it;

    return std::shared_ptr< WordsData >( nullptr );
}

///////////////////////////////////////////////////////////////////////////////
WordsSortFilterProxyModel::WordsSortFilterProxyModel( QObject* parent )
    : QSortFilterProxyModel( parent )
    , needShowLearned( true )
{}

void WordsSortFilterProxyModel::setNeedShowLearned( bool val )
{
    if( needShowLearned == val )
        return;

    needShowLearned = val;
    invalidateFilter();
}

bool WordsSortFilterProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const
{
    QModelIndex index = sourceModel()->index(sourceRow, Hid_WordModel_IsLearned, sourceParent);
    if( !needShowLearned && sourceModel()->data( index ).toBool() )
        return false;

    return true;
}
