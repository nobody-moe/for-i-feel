#ifndef DECK_H
#define DECK_H

#include <QObject>

#include "card.h"


class Deck : public QObject
{
    Q_OBJECT
public:
    explicit Deck(QObject *parent = 0);
    ~Deck();

    void shuffle(int seed=0);
    QList<Card*> *getShuffled();

    Card ** testV();

signals:

public slots:

};

#endif // DECK_H
