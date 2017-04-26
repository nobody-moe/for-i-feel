#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include "deck.h"
#include "carddrawer.h"

enum eventType{
    moveOp,
    dragOp,
    dropOp,
    doubleClickOp,
    pressOp
};


class Site {

public:

    QRect  rect;
    Place *place;

    void    draw();
    bool    in(QPoint pt);
    QPoint  startPoint();
    /** Entire rect for 'inSite()' */
    QRect   factRect();
    /** Only top card rect for draving */
    QRect   topRect();

    bool    canTake();
    bool    canTake(QPoint pos);
    void    Take();
    QList<Card *> pop(int num);

    QList<Card *> popReverce(int num)/** test*/;
    Card *  pop();              //  get top card and remove it from stack
    Card *  top();              //  get top card without removing it from stack

    int     size();             //  returns size of stack or 1 if isn't stack or 0 if empty
    int     predictTakeSize();  //  predict size of list of taken cards if take right now
    Card *  predictTakeCard();  //  predict front card that will be taken if take right now

    bool    empty(){return place->empty();}
    bool    canPush();
    void    push();
    bool    canPush(Card *card);
    void    push(Card *card);
    void    forcePush(Card *card);
    void    forcePush(QList<Card *> cList);

    void    hilight(bool hl=true);
    void    hilightBlue(int index, bool hl=true);
    void    clear();

    /** Test if it's 'Target' and is full */
    bool    completed();

     int const &  id(){return place->id;}



    QPixmap pixmap();



protected:
    bool hilighted;         // can push
    bool b_highlited;       // can take (blue)
    int  b_highlited_index; // index where draw hilight

};


class Board : public QObject
{
    Q_OBJECT
public:
    explicit Board(QObject *parent = 0);

    void    setState(State st);


    void newGame(int seed = 0);
//     History?
//     seed save ?

     void draw(QPainter *pntr);  //(current state)
     Site * inSite(QPoint pt);

     void doubleClick(Site *site);

     QList<QRect> getUpdateRects();

     QRect handleAir();

     bool       mouseEvent(QPoint pos, eventType type);
     QPixmap    getDragPixmap();
     QPoint     getDragPoint();

     void undo();
     bool canUndo();


     void testVictory(); // temp


protected:
    Deck  deck;

    Site  stacks[8];
    Site  _slots[4];
    Site  targets[4];

    QPixmap currentPixmap;          // Pixmap     for current Site drag operation

    void createStacks();
    void createSlots();
    void createTargets();

    void setGeometry();
    void dealCards();

    bool mousePress(Site *s, QPoint pos);
    bool haveMoveSlot(Site *source);
    bool haveNotEmptyMoveStack(Site *source);
    int  emptySites();
    int  emptyStacks();
    int  emptySlots();
    int  calculateNumCanMove(bool toNotEmpty);
    bool canMove(Site *source, QPoint pos);

    void testCompleted();

    Step* possibleAuto();               // return step if exist auto-step or 0 otherwise
    Step* canMoveToTarget(Site *site);
    int   targetCanAccept(Card* card);  // id of Target to accept if have one or -1
    void  perform(Step* s);             // perform auto-step

signals:
    void solitaireCompleted();
    void trackCardMoved();

public slots:
    void victory();
    void autoComplete();
};


QRect  inset(QRect r, int x);


#endif // BOARD_H
