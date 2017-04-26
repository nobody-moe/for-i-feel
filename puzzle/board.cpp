#include "board.h"
#include <QtDebug>
#include <QMap>

#include <QSound>

QSound move_sound("move_card.wav");
QSound take_sound("take_card.wav");


CardDrawer drawer;

QList<Site*> sites;             // list of all sites (for simplifying iteration)

QList<QRect> updateRects;

Card         *inAir = 0;        // card currently dragged (in air)
Site         *from  = 0;        // from where inAir taken
QList<Card*> flying;            // List of cards currently dragged (in air)
int          takeIndex = 0;     // index of taken card when moving group

QMap<int,Site*> siteIdMap;      // Maps place id and Site for undo functionality
QStack<Step>    undoes;         // stack of undoes

Site    *currentSite;           // Site for current operations (move, drop, press, double click)

QPoint  currentStartPoint;      // StartPoint for current Site drag operation

bool noTestVictory = false;     // Flag to no test victory conditions when victory already detected


/*********************************************************
  SITE
*********************************************************/


/** draw() ------------------------------------
***********************************************/
void Site::draw(){
  //  qDebug() << "draw()";
    int index = 0;                          //---- for blue highlited
    int Y = rect.top();
    drawer.drawCardEmpty(rect.left(), Y);

    if(place->empty()){
        if(hilighted) drawer.drawGlow(rect.left(), Y);
        return;
        }

    Card * c = place->getFirst();
    if(c)
        do{
           if(b_highlited && index == b_highlited_index)
               drawer.drawGlow(rect.left(), Y);
           index++;
           if(!place->hasNext() && hilighted)
              drawer.drawBlueGlow(rect.left(), Y);
           drawer.drawCard(rect.left(),Y,c->value,c->type,0,0);
           Y += stacksYOffset;
        }while(c = place->getNext());
}

/** in(QPoint pt) -----------------------------
***********************************************/
bool Site::in(QPoint pt){
  //  qDebug() << "in(QPoint pt)";
    return factRect().contains(pt);
}

/** startPoint() ------------------------------
***********************************************/
QPoint Site::startPoint(){
  //  qDebug() << "startPoint()";

    if(Stack *st = dynamic_cast<Stack*>(place))
    {
        QPoint pt = topRect().topLeft() + QPoint(3,3);
        if(st->size())
            pt.ry() += stacksYOffset;
        return pt;
    }
    return factRect().topLeft() + QPoint(3,3);
}

/** factRect() --------------------------------
***********************************************/
QRect Site::factRect(){
 //   qDebug() << "factRect()";
    if(Stack *st = dynamic_cast<Stack*>(place)){
        QRect r = rect;
        if(st->size())
           // r.moveTop( r.top() + (st->size()-1) * stacksYOffset );
            r.setBottom( r.bottom() + (st->size()-1) * stacksYOffset );
        return r;
    }
    return rect;
}

/** topRect() ---------------------------------
***********************************************/
QRect Site::topRect(){
 //   qDebug() << "topRect()";
    if(Stack *st = dynamic_cast<Stack*>(place)){
        QRect r = rect;
        if(st->size())
            r.moveTop( r.top() + (st->size()-1) * stacksYOffset );
        return r;
    }
    return rect;
}

/** Test if it's 'Target' and is full---------
***********************************************/
bool Site::completed(){
 //   qDebug() << "completed()";
    if(Target *tg = dynamic_cast<Target*>(place))
        return tg->completed();
    return false;
}

/** canTake() ---------------------------------
***********************************************/
bool Site::canTake(){
 //   qDebug() << "canTake()";
    takeIndex = 0;
    if( flying.size()) return 0;
    return place->canTake();
}

/** canTake(QPoint pos) -----------------------
***********************************************/
bool Site::canTake(QPoint pos){
 //   qDebug() << "canTake(QPoint pos)";
    takeIndex = 0;
    if(flying.size()) return 0;
    if(!pos.isNull())
        if(Stack *st = dynamic_cast<Stack*>(place)){
            QPoint localPt = pos - rect.topLeft();
            takeIndex = localPt.y()/stacksYOffset;
            if( takeIndex >= st->size() )
            takeIndex = st->size()-1;
            bool res = st->canTake(takeIndex);
        //    qDebug() << "Can Take ?:" << "localPt:" << localPt << "num:" << takeIndex << res;
            return res;
        }
    return place->canTake();
}

/**  size() -----------------------------------
***********************************************/
int Site::size(){
    return place->size();
}

/** predictTakeSize() -------------------------
***********************************************/
int Site::predictTakeSize(){
   //qDebug() << "predictTakeSize():"  << place->size() - takeIndex ;
   return place->size() - takeIndex;
}

/** predictTakeCard() -------------------------
***********************************************/
Card * Site::predictTakeCard(){
    return place->predictTakeCard(takeIndex);
}

void Site::Take(){
 //   qDebug() << "Take():"  << takeIndex ;               // --- ERROR when index = 0
    if(Stack *st = dynamic_cast<Stack*>(place)){
        flying = st->Take(takeIndex);
    }
    else
        flying.append(place->pop());
    from = this;
}

/** Take without 'inAir' and 'from' -----------
***********************************************/
Card * Site::pop(){
 //   qDebug() << "pop()";
   return place->pop();
}

Card *  Site::top(){
    return place->getLast();
}

/** pop(int num) ------------------------------
***********************************************/
QList<Card*>  Site::pop(int num){
 //   qDebug() << "Site::pop(int num)";
   QList<Card*> result;

   if(num >= place->size()) return result;

   if(Stack *st = dynamic_cast<Stack*>(place)){
       return st->Take(num);
   }
   result.append(place->pop());
   return result;
}

/** popReverce(int num) -----------------------
***********************************************/
QList<Card*>  Site::popReverce(int num){
    int sz = place->size();
    if( (num > 0) && (sz >= num))
        return pop(sz-num);
}

/** canPush() ---------------------------------
***********************************************/
bool Site::canPush(){
 //   qDebug() << "canPush()";
    if(flying.empty()) return 0;
    if(Stack *st = dynamic_cast<Stack*>(place)){
        return st->canPush(flying.front());
    }
    if(flying.size() == 1)
        return place->canPush(flying.front());
    return false;
}

/** canPush(Card *card) -----------------------
***********************************************/
bool Site::canPush(Card *card){
 //   qDebug() << "canPush(Card *card)";
    return place->canPush(card);
}

/** push()) -----------------------------------
***********************************************/
void Site::push(){
//    qDebug() << "push()";
    int n = flying.size();
    if( !n ) return;

    place->push(flying);
    flying.clear();
    hilighted=0;

    if(from->id()!=id())                    // ----- handle self-undo situations
        undoes.push(Step(from->id(),id(), n));

    updateRects.push_back(inset(factRect(), -15));
}

/** Site::push(Card *card) --------------------
***********************************************/
void Site::push(Card *card){
 //   qDebug() << "push(Card *card)";
    place->push(card);
    updateRects.push_back(inset(factRect(), -15));
}

/** forcePush(Card *card) ---------------------
***********************************************/
void Site::forcePush(Card *card){
 //   qDebug() << "forcePush(Card *card)";
    if(canPush(card))
        place->push(card);
    else {
        Stack *st = dynamic_cast<Stack*>(place);
        st->forcePush(card);
    }
}

/** forcePush(QList<Card *> cList) ------------
***********************************************/
void Site::forcePush(QList<Card *> cList){
 //   qDebug() << "QList<Card *> cList";

    int n = cList.size();
    if( !n ) return;

    if(n==1){
        forcePush(cList.takeFirst());
    }
    else {
        Stack *st = dynamic_cast<Stack*>(place);
        if(!st)
            {qDebug() << "Error in Site::forcePush(QList<Card *> cList): n>1 and dynamic_cast<Stack*>(place) == null" << n;
             return;}
        st->forcePush(cList);
    }
}

/** hilight -----------------------------------
***********************************************/
void   Site::hilight(bool hl){
 //   qDebug() << "hilight(bool hl)";
    if(hilighted!=hl){
        updateRects.push_back(inset(factRect(), -15));
        hilighted=hl;
    }
}

void   Site::hilightBlue(int index, bool hl){
  //   qDebug() << "hilightBlue(int index, bool hl=true)";
    if(b_highlited!=hl){
        QRect r = rect;
        r.moveTop( r.top() + (hl ? index : b_highlited_index) * stacksYOffset );
        updateRects.push_back(inset(r, -18));
    }
    if(b_highlited && hl && (index!=b_highlited_index)){
        QRect r = rect;
        r.moveTop( r.top() + b_highlited_index * stacksYOffset );
        updateRects.push_back(inset(r, -18));
        r = rect;
        r.moveTop( r.top() + index * stacksYOffset );
        updateRects.push_back(inset(r, -18));
    }
    b_highlited = hl;
    b_highlited_index = index;
}


/** clear() -----------------------------------
***********************************************/
void   Site::clear(){
 //   qDebug() << "clear()";
    place->clear();
    hilighted = 0;
}

/** pixmap() ----------------------------------
***********************************************/
QPixmap Site::pixmap(){
    return drawer.stackPixmap(flying);
}

/*********************************************************
  BOARD
*********************************************************/

Board::Board(QObject *parent) : QObject(parent)
{   setGeometry();
    newGame();
}


void Board::setGeometry(){
    for(int i = 0; i < 4; i++){
        _slots[i].place = new Slot();
        _slots[i].rect  = QRect(slotsStartX+i*(cardWidth+slotsXOffset),
                                slotsStartY,cardWidth, cardHeight);

        targets[i].place = new Target();
        targets[i].rect = QRect(slotsStartX+(5+i)*(cardWidth+slotsXOffset),
                                slotsStartY,cardWidth, cardHeight);
        sites.append(&(_slots[i]));
        sites.append(&(targets[i]));

        siteIdMap[_slots[i].id()] = &(_slots[i]);
        siteIdMap[targets[i].id()] = &(targets[i]);
    }
    createStacks();
}


void Board::createStacks(){

   // QList<Card*> *shuffled = deck.getShuffled();
    for(int i=0;i<8;i++){
        Stack *st = new Stack();
        stacks[i].place = st;
       // if(Stack *st = dynamic_cast<Stack*>(stacks[i].place))
        st->clear();
        stacks[i].rect = QRect(stacksStartX+i*stacksXOffset, stacksStartY,
                               cardWidth, cardHeight);
        sites.append(&(stacks[i]));
        siteIdMap[stacks[i].id()] = &(stacks[i]);
    }

}

void Board::newGame(int seed){
    deck.shuffle(seed);
    updateRects.clear();
    flying.clear();
    from  = 0;
    for(int i=0; i<sites.size(); i++)
        sites[i]->clear();

    dealCards();
    undoes.clear();
    noTestVictory = false;
}

void Board::dealCards(){
    QList<Card*> *shuffled = deck.getShuffled();

    for(int i=0;i<numCards;i++){
        if(Stack *st = dynamic_cast<Stack*>(stacks[i%8].place))
                st->forcePush(shuffled->front());
        shuffled->pop_front();
    }
}

void Board::draw(QPainter *painter){
     drawer.setPainter(painter);
     for(int i=0; i<sites.size(); i++)
         sites[i]->draw();
}

Site * Board::inSite(QPoint pt){
    Site *result = 0;
    updateRects.clear();
    for(int i=0; i<sites.size(); i++)
        if(sites[i]->in(pt)){
            result = sites[i];
          //  sites[i]->hilight();
        }
        else {
            sites[i]->hilight(false);
            sites[i]->hilightBlue(0,false);
        }
    return result;
}

QList<QRect> Board::getUpdateRects(){
    return updateRects;
}

QRect Board::handleAir(){
    if(flying.size()){
        if(from->canPush())
           from->push();
        else
           {Stack *st = dynamic_cast<Stack*>(from->place);
            st->forcePush(flying);
            flying.clear();
           }
        QRect rRet=from->factRect();
        inset(&rRet,-12);
        return rRet;
    }
    return QRect();
}

void Board::doubleClick(Site *site){
    updateRects.clear();
    if((!site) || (!site->canTake()) )return;
    Card *card = site->pop();
    qDebug() << "card:" << *card;
    for(int i = 0; i < 4; i++){
        if(targets[i].canPush(card)){
            targets[i].push(card);
            updateRects.push_back(inset(site->factRect(), -18));
            updateRects.push_back(inset(targets[i].factRect(), -15));
            undoes.push(Step(site->id(),targets[i].id()));
            return;
        }
    }
    for(int i = 0; i < 8; i++){
        if((stacks[i].id()!=site->id() ) &&
            stacks[i].canPush(card) )
        {
            stacks[i].push(card);
            updateRects.push_back(inset(site->factRect(), -18));
            updateRects.push_back(inset(stacks[i].factRect(), -15));
            undoes.push(Step(site->id(),stacks[i].id()));
            return;
        }
    }
    if( ! dynamic_cast<Site*>(site->place))   // Possible ERROR ( dynamic_cast<Site*> -> m.b. dynamic_cast<Target*>)
    for(int i = 0; i < 4; i++){
        if(_slots[i].canPush(card) ){
            _slots[i].push(card);
            updateRects.push_back(inset(site->factRect(), -18));
            updateRects.push_back(inset(_slots[i].factRect(), -15));
            undoes.push(Step(site->id(),_slots[i].id()));
            return;
        }
    }

    site->forcePush(card);
}

/** undo() ------------------------------------
***********************************************/
void Board::undo(){
    if(!undoes.size()) return;

    updateRects.clear();
    Step s = undoes.pop();
    updateRects.push_back(inset(siteIdMap[s.dst]->factRect(), -18));
    siteIdMap[s.src]->forcePush(siteIdMap[s.dst]->popReverce(s.num));
    updateRects.push_back(inset(siteIdMap[s.src]->factRect(), -18));
    qDebug() << "n = " <<s.num;
}

bool Board::canUndo(){
    return undoes.size();
}

void Board::testVictory() // temp
{
    deck.shuffle();
    updateRects.clear();
    from  = 0;
    for(int i=0; i<sites.size(); i++)
        sites[i]->clear();

    Card **cards=deck.testV();
    int j;
    for(int i =0; i < 4; i++){
        for(j=0; j < numCardValues-1; j++)
            targets[i].push(cards[j + i*numCardValues]);
        stacks[i].forcePush(cards[j + i*numCardValues]);
    }

    undoes.clear();
    noTestVictory = false;
}

bool Board::mouseEvent(QPoint pos, eventType type){
    Site *s=inSite(pos);
    if(!s) return false;

    switch(type){
    case moveOp: if(canMove(s, pos)){
                    s->hilightBlue(takeIndex);

                }
                 break;

    case dragOp: if( s->canPush())
                    s->hilight();
                 break;

    case dropOp: if( s->canPush())
                    s->push();
                 break;

    case doubleClickOp:
                 doubleClick(s);
                 break;

    case pressOp:
                 return mousePress(s, pos);
                 break;
    }
    testCompleted();
    return true;
}

/** mousePress(Site *s, QPoint pos)) ----------
***********************************************/
bool Board::mousePress(Site *s, QPoint pos){
    if( !canMove(s, pos) ) return false;

    updateRects.push_back(inset(s->factRect(),-20));

    s->Take();
    currentPixmap = s->pixmap();
    if(!currentPixmap) {
        updateRects.pop_back();
        return false;
    }
    take_sound.play();
    currentStartPoint = s->startPoint();

    return true;
}

/** Board::haveMoveSlot(Site *source) ---------
***********************************************/
bool Board::haveMoveSlot(Site *source){
    Card *temp = source->predictTakeCard();
    if(!temp) return false;

    for(int i=0; i < 8; i++)
        if(stacks[i].id()!=source->id() && stacks[i].canPush(temp))
            return true;
    if(source->predictTakeSize() > 1) return false;

    for(int i=0; i < 4; i++)
        if(_slots[i].empty()) return true;
    for(int i=0; i < 4; i++)
        if(targets[i].canPush(temp)) return true;

    return false;
}

bool Board::haveNotEmptyMoveStack(Site *source){
    Card *temp = source->predictTakeCard();
    for(int i=0; i < 8; i++)
        if(stacks[i].id()!=source->id() &&
           stacks[i].canPush(temp)      &&
          !stacks[i].empty()               )
            return true;
}

/** emptySites() ---------------------
***********************************************/
int Board::emptySites(){
    return emptySlots()+ emptyStacks();
}

int Board::emptySlots(){
    int counter = 0;
    for(int i=0; i < 4; i++)
        if(_slots[i].empty()) counter++;
    return counter;
}

int Board::emptyStacks(){
    int counter = 0;
    for(int i=0; i < 8; i++)
        if(stacks[i].empty()) counter++;
    return counter;
}

int Board::calculateNumCanMove(bool toNotEmpty){
    int n = emptySlots();
    int m = emptyStacks();
    if(!m) return n+1;

    int sum = n + m;
    int result = ((sum + sum - m +1 ) * m)>>1;

    return toNotEmpty ? result+n+1 : result;
}


/** canMove(Site *source) ---------------------
***********************************************/
bool Board::canMove(Site *s, QPoint pos){
    if(!s->canTake(pos))    return false;
    if(!haveMoveSlot(s))    return false;
    int canMove = calculateNumCanMove(haveNotEmptyMoveStack(s));
    if(canMove < s->predictTakeSize()) return false;
    return true;
}


QPixmap Board::getDragPixmap()  { return currentPixmap;       }
QPoint  Board::getDragPoint()   { return currentStartPoint;   }
void    Board::victory()        { emit   solitaireCompleted();}

/** testCompleted() ---------------------------
***********************************************/
void Board::testCompleted(){
    if( noTestVictory ) return;

    for(int i=0; i <4 ; i++)
        if(!targets[i].completed()) return;

    noTestVictory = true;
    QTimer::singleShot(100, this, SLOT(victory()));
}

/** autoComplete() ----------------------------
***********************************************/
void Board::autoComplete(){

    Step *s  = possibleAuto();
    if(s) perform (s);
}

void Board::perform(Step* s){

    updateRects.push_back(inset(siteIdMap[s->src]->topRect(), -18));
    updateRects.push_back(inset(siteIdMap[s->dst]->topRect(), -18));
    siteIdMap[s->dst]->push(siteIdMap[s->src]->pop());
    undoes.push(Step(*s));
    emit trackCardMoved();
    move_sound.play();
    QTimer::singleShot(200, this, SLOT(autoComplete()));

}

/** possibleAuto() ----------------------------
***********************************************/
Step* Board::possibleAuto(){
    for(int i = 0; i < 4; i++){
        Step*  step = canMoveToTarget(&(_slots[i]));
        if(step) return step;
    }

    for(int i = 0; i < 8; i++){
        Step*  step = canMoveToTarget(&(stacks[i]));
        if(step) return step;
    }
    return 0;
}

/** canMoveToTarget(Site *site) ---------------
***********************************************/
Step* Board::canMoveToTarget(Site *site){

    int id = targetCanAccept(site->top());
    if(id != -1)
        return new Step(site->id(), id, 1);
    return 0;
}
/** targetCanAccept(Card* card) ---------------
***********************************************/
int   Board::targetCanAccept(Card* card){  // id of Target to accept if have one or -1
    if(card)
    for(int i = 0; i < 4; i++)
        if(targets[i].canPush(card))
            return targets[i].id();
    return -1;
}

