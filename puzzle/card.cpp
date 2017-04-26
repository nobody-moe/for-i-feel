#include "card.h"

#include <QtGui>

#include <QtDebug>

#include <QString>
QString  cardNames[13] = {"Ace", "2", "3", "4", "5", "6", "7", "8",
                          "9", "10", "Valet", "Damme", "King"};
QString  cardTypes[4]  = {"hearts", "spades", "diamonds", "baptize"};

int idCounter = 0;


/***************************************************************/
/** Util. return true if 'next' can be placed on top of 'current' in 'Stack'*/
inline bool validStack(Card *current, Card *next){
    return current && next && (((current->type+next->type)&1) && (current->value == next->value+1));
}

Card::operator QString(){ return cardNames[value] + " " + cardTypes[type];}

Place::Place(){
   counter = 0;
   id = ++idCounter;
   first = 0;
}

bool Target::canPush(Card *card){
    if(cards.empty())
        return card->value == 0; // 'Ace'
    else
        return (card->type == cards.top()->type) && (card->value == (cards.top()->value+1));
}

void Target::push(Card *card){
    if(canPush(card)){
        cards.empty() ? first = card : 0;
        cards.push(card);
    }
}

void Target::push(QList<Card*> cList){
    push(cList.front());
}

Card *Target::pop(){
    Card * c = cards.size() ? cards.pop() : 0;
    if(cards.empty())
        first = 0;
    return c;
}

bool Slot::canPush(Card *card){
    if(!first) return 1;
    return 0;
}

void Slot::push(Card *card){
    if(canPush(card))
        first=card;
}

void Slot::push(QList<Card*> cList){
    push(cList.front());
}

Card * Slot::pop(){
    Card *temp = first;
    first=0;
    return temp;
}

bool Stack::canPush(Card *card){
    if(empty()) return true;

    Card *tmp = cards.back();
    return validStack(tmp,card);
}

void Stack::push(Card *card){
    if(!canPush(card)) return;

    if(!cards.size()) first=card;
    cards.push_back(card);
}

void Stack::push(QList<Card*> cList){

    if(cList.empty() || (!canPush(cList.front()))) return;
    forcePush(cList);
}

void Stack::forcePush(Card *card){

    if(!cards.size()) first=card;
    cards.push_back(card);
}

void Stack::forcePush(QList<Card*> cList){
    if(cList.empty()) return;
    if(!cards.size()) first = cList.front();
    while(cList.size())
        cards.push_back(cList.takeFirst());
}

bool Stack::hasNext()  {
    return ((!cards.isEmpty()) && (counter < cards.size())) ? 1 : 0;
}

Card *Stack::getNext() {
    return  counter < cards.size() ? cards[counter++] : 0;
}

Card *Stack::getLast(){
    if(cards.size())
        return cards.last();
    return first;
}

Card * Stack::pop(){
    if(empty()) return 0;
    Card *temp = cards.back();
    cards.pop_back();

    if(cards.isEmpty())
        {first=0;
     //   qDebug() << "Card * Stack::pop():" << cards.size();
    }
    return temp;
}

/** get i-th card (one) */
Card * Stack::get(int i){
    return ((i < cards.size()) && (i >= 0)) ?  cards[i]: first;
}

/** can take stack of cards from position cur (current)*/
bool Stack::canTake(int cur){
  // // qDebug() << "Stack::canTake ----------------------" << "cur" << cur << "size" << cards.size();
    if(cur >= cards.size()) return 0;

    for(QList<Card*>::const_iterator i = cards.constBegin()+cur;;){
      Card *back = *i++;
      if(i==cards.constEnd()) return true;
      Card *top = *i;
      if(!validStack(back,top)) return false;
    }

    qDebug() << "Error in Stack::canTake";
    return false;
}

/** take all stack of cards from position cur (current)*/
QList<Card*> Stack::Take(int cur){
    QList<Card*> out;

    if(cur == 0)
        qDebug() << "IS 0!" << cards.size();
    while(cards.size()> cur)
        out.push_back(cards.takeAt(cur));
  //  qDebug() << "QList<Card*> Stack::Take(int cur):" << cards.size();
    if(cards.empty()) first=0;
      //  qDebug() << "QList<Card*> Stack::Take(int cur) return size:" << out.size();
    return out;
}


void Stack::clear(){
    cards.clear();
    first = 0;
}

