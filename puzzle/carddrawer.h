#ifndef CARDDRAWER_H
#define CARDDRAWER_H

#include <QObject>
#include <QtGui>

#ifndef CONSTANTS_H
    #include "Constants.h"
#endif

#include "Card.h"

/**
*  Class for simplifying draving operations
*/

class CardDrawer : public QObject
{
    Q_OBJECT
public:
    explicit CardDrawer(QObject *parent = 0);

    void setPainter(QPainter *pntr);

    void drawCard(int x, int y, int value,int type, bool haveShadow, bool haveGlow);

    void drawCardGlow   ( int x, int y, int value,int type);
    void drawCardShadow ( int x, int y, int value,int type);
    void drawCardFull   ( int x, int y, int value,int type);
    void drawCardEmpty  ( int x, int y );
    void drawGlow       ( int x, int y );
    void drawBlueGlow   ( int x, int y );

    QPixmap pixmap(int value, int type);
    QPixmap stackPixmap(QList<Card*> List);

protected:
    QImage cardImages[numCardValues][numCardTypes];
    QImage shadow;
    QImage glow;
    QImage glow_blue;
    QImage empty;

    void setImages(const QString &path=":/images/cards");

signals:

public slots:

};

#endif // CARDDRAWER_H
