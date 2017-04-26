#ifndef CARD_H
#define CARD_H

#include <QList>
#include <QStack>

#include <QtDebug>

#ifndef CONSTANTS_H
    #include "Constants.h"
#endif

class Card
{
public:
    Card(int type,int value){this->type=type; this->value = value;}

    int type;
    int value;

    operator QString();
};

class Place {
protected:

    Card * first;
public:
    int id;
    Place();

    bool operator ==(Place a){return id == a.id;}
    bool operator !=(Place a){return id != a.id;}

    virtual bool empty()             {return !first;}
    virtual int  size()              {return first ? 1 : 0;}
    virtual void clear()             {first = 0;}
    virtual bool canPush(Card *card) {return 0;}
    virtual bool canTake()           {return 0;}
    virtual void push(Card *card)    {}
    virtual void push(QList<Card*> cList){}
    virtual Card *pop()              {return 0;}

    virtual bool hasNext()           {if( counter ) return 0; return first;}
    virtual Card *getFirst()         {counter = 1; return first;}
    virtual Card *getNext()          {return 0;}
    virtual Card *getLast()          {return first;}
    virtual Card *predictTakeCard(int index){return first;}

    virtual QString name()           {return "Place";}

protected:
    /** counter of 'getNext()' for drawing purposes */
    int counter;
};


class Target : public  Place{
    QStack<Card*> cards;
public:
    virtual bool canTake()  { return 0; }
    bool canPush(Card *card);
    void push(Card *card);
    void push(QList<Card*> cList);
    Card *pop();

    bool empty()    {return cards.empty();}
    void clear()    {cards.clear();}
    virtual Card *getFirst()         {counter = 1; return getLast();}
    virtual Card *getNext()          {return 0;}
    Card *getLast() {return cards.size() ? cards.top() : 0; }
    Card *predictTakeCard(int index){return 0;}

    /** test if 'Target' have all 13 cards*/
    bool    completed(){return cards.size() >= numCardValues;}

    QString name(){ return "Target";}
};

class Slot : public  Place{
public:
    bool canPush(Card *card);
    virtual bool canTake()  { return first; }
    void push(Card *card);
    void push(QList<Card*> cList);
    Card *pop();

    QString name()          { return "Slot"; }
};

class Stack : public  Place{
QList<Card*> cards;

public:
    bool empty()            {return cards.empty();}
    int  size()             {return cards.size();}
    virtual bool canTake()  { return first; }
    bool canTake(int cur);
    bool canPush(Card *card);
    void push(Card *card);
    void push(QList<Card*> cList);
    void forcePush(Card *card);
    void forcePush(QList<Card*> cList);
    Card *pop();
    QList<Card*> Take(int cur);

    void clear();

    bool hasNext();
    Card *getNext();
    Card *getLast();
    Card *get(int i);
    Card *predictTakeCard(int index){return get(index);}

    QString name(){ return "Stack";}

};

struct Step {
    /** source Place.id*/
    int src;
    /** destination Place.id*/
    int dst;

    int num;

    Step(){src=0; dst=0; num =0;}
    Step(int s, int d){src=s; dst=d; num=1;}
    Step(int s, int d, int n){src=s; dst=d; num=n;}
    bool valid(){return src && dst && ( num > 0 );}
};

class State {
public:
    Stack   stacks[8];
    Slot    _slots[4];
    Target  targets[4];

    Step lastStep;

};


#endif // CARD_H
