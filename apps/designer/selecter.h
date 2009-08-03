#ifndef SELECTER_H
#define SELECTER_H
#include <QList>
#include "iteminterface.h"
#include "itemselection.h"

class GraphicsItemGroup;
class QGraphicsScene;

struct Item
{
    Report::ItemInterface * item;
    qreal zValue;
    Report::ItemInterface * parent;
    ItemSelection * sel;
};

class Selecter: public QObject
{
    Q_OBJECT
public:
    Selecter( QGraphicsScene * scene);
    ~Selecter();

    void itemSelected(Report::ItemInterface * item, Qt::KeyboardModifiers keys);
    void add (Report::ItemInterface * item);
    void remove (Report::ItemInterface * item);
    void free();

    void store();
    void restore();

public slots:
    void itemMoved(Report::ItemInterface * item, QPointF oldPos);

//private slots:
//    void sceneDestroyed();

private:
    GraphicsItemGroup * sItem;
    QList<Item> items;
    QList<Item> storedItems;
};

class GraphicsItemGroup: public QGraphicsItemGroup
{
    protected:
    void paint ( QPainter * /*painter*/, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/) {};
};


#endif // SELECTER_H
