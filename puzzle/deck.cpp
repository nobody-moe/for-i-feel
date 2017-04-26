#include <QtGui>
#include "deck.h"


Card * deck_[numCards];
QList<Card*> shuffled;

bool initOK = false;    // deck - singleton чтобы не заполнять deck 2 раза


void initDeck(){
if(!initOK)
    for(int i=0;i<numCards;i++){
        Card *temp = new Card(i/numCardValues,i%numCardValues);
        deck_[i]=temp;
    }
initOK = true;
}

void Deck::shuffle(int seed){
    shuffled.clear();
   for(int i=0;i<numCards;i++)
      shuffled.append(deck_[i]);
   if(!seed)
       qsrand(QCursor::pos().x() ^ QCursor::pos().y());
   else
      qsrand(seed);
   for(int i=0;i<100;i++){
      Card *temp = shuffled.front();
      shuffled.pop_front();
      shuffled.insert(numCards*qrand()/(RAND_MAX+1),temp);
   }
}

/************************************************/

Deck::Deck(QObject *parent) : QObject(parent)
{
     initDeck();
}

Deck::~Deck(){
    shuffled.clear();
}

QList<Card*> *Deck::getShuffled(){
    if(!shuffled.size())
        shuffle();
    return &shuffled;
}

Card ** Deck::testV(){return deck_;}
