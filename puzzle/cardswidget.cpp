#include <QtGui>
#include <QMessageBox>
#include "cardswidget.h"

#include "card.h"
#include "carddrawer.h"
#include "deck.h"

#include <QtDebug>

bool ended = false;



CardsWidget::CardsWidget(QWidget *parent) :
    QWidget(parent)
{
    setAcceptDrops(true);
    setBackground(":/images/fon");
    setSize(600, 600);
    //mouseTracking = true;
    setMouseTracking(true);

    connect(&board, SIGNAL(solitaireCompleted()), this, SLOT(setCompleted()));
    connect(&board, SIGNAL(trackCardMoved()),     this, SLOT(trackCardMoved()));

}

void CardsWidget::setCompleted(){
    emit puzzleCompleted();
}

void CardsWidget::trackCardMoved(){
    updateBoardRects();
}

void CardsWidget::newGame(){
    board.newGame();
    update();
}

void CardsWidget::undo(){
    board.undo();
    update();
}

void CardsWidget::testVictory(){
    board.testVictory();
    update();
}

void CardsWidget::autoComplete(){
    board.autoComplete();
    update();
}

void CardsWidget::clear()
{
    ended = false;
    update();
}

void CardsWidget::setSize(int w, int h)
{if(!background.isNull()){
        w = background.width();
        h = background.height();
    }
    setMinimumSize(w, h);
    setMaximumSize(w, h);
}

void CardsWidget::setBackground(const QString &path){
    if (!background.load(path))
        QMessageBox::warning(this, tr("Open Image"),
                              tr("The background could not be loaded."),
                              QMessageBox::Cancel);
}

void CardsWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("image/x-puzzle-piece"))
        event->accept();
    else{
        qDebug() << "Mime: ";
        QStringList dList = event->mimeData()->formats();
        for(int i = 0; i < dList.size(); i++)
            qDebug() << i << " " << dList[i];
        event->ignore();
    }
}

void CardsWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void CardsWidget::dragMoveEvent(QDragMoveEvent *event)
{

    if( board.mouseEvent(event->pos(),dragOp)){
        QRect aroundRect(event->pos().x() - cardWidth, event->pos().y()- cardHeight, cardWidth+30, cardHeight+30 );
        update(aroundRect);

    }
    updateBoardRects();

    /**
    QRect aroundRect(event->pos().x() - cardWidth, event->pos().y()- cardHeight, cardWidth+30, cardHeight+30 );
    Site *s = board.inSite(event->pos());

    if(s)
        if( s->canPush()){
        qDebug() << moveOp << " " << dropOp << " " << doubleClickOp;
            s->hilight();
        }
    update(aroundRect);
    updateBoardRects();*/

//    QRect updateRect = highlightedRect.unite(targetSquare(event->pos()));

//    if (event->mimeData()->hasFormat("image/x-puzzle-piece")
//        && findPiece(targetSquare(event->pos())) == -1) {

//        highlightedRect = targetSquare(event->pos());
//        event->setDropAction(Qt::MoveAction);
//        event->accept();
//    } else {
//        highlightedRect = QRect();
//        event->ignore();
//    }

//    update(updateRect);
}

void CardsWidget::testRollback(){
    QRect backRect = board.handleAir();
    if(!backRect.isNull())
        update(backRect);
}

void CardsWidget::updateBoardRects(){
    QList<QRect> rects = board.getUpdateRects();
    if(rects.size()){
        for(int i = 0; i < rects.size(); i++)
            update(rects[i]);
    }
}

void CardsWidget::dropEvent(QDropEvent *event)
{
    board.mouseEvent(event->pos(),dropOp);
    QRect aroundRect(event->pos().x() - cardWidth, event->pos().y()- cardHeight, cardWidth*2+30, cardHeight*2+30 );
    update(aroundRect);
    updateBoardRects();
    testRollback();

/**
    Site *s = board.inSite(event->pos());

    if(s)
        if( s->canPush()){
            s->push();
           // update(aroundRect);
        }
    QRect aroundRect(event->pos().x() - cardWidth, event->pos().y()- cardHeight, cardWidth*2+30, cardHeight*2+30 );
    update(aroundRect);
    qDebug() << "Rect: " << aroundRect;
    testRollback();
*/

//    QList<QRect> rects = board.getUpdateRects();
//    if(rects.size()){
//        for(int i = 0; i < rects.size(); i++)
//            update(rects[i]);
//    }

//    if (event->mimeData()->hasFormat("image/x-puzzle-piece")
//        && findPiece(targetSquare(event->pos())) == -1) {

//        QByteArray pieceData = event->mimeData()->data("image/x-puzzle-piece");
//        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
//        QRect square = targetSquare(event->pos());
//        QPixmap pixmap;
//        QPoint location;
//        dataStream >> pixmap >> location;

//        pieceLocations.append(location);
//        piecePixmaps.append(pixmap);
//        pieceRects.append(square);

//        highlightedRect = QRect();
//        update(square);

//        event->setDropAction(Qt::MoveAction);
//        event->accept();

//		if (location == QPoint(square.x()/wSize, square.y()/hSize)) {
//            inPlace++;
//            if (inPlace == divNum*divNum){
//                ended = true;
//                QApplication::beep ();

//             //   emit puzzleCompleted();
//            }
//        }
//	} else {
//        highlightedRect = QRect();
//        event->ignore();
//	}
}


void CardsWidget::mouseDoubleClickEvent ( QMouseEvent * event ){
    if(board.mouseEvent(event->pos(),doubleClickOp))
        updateBoardRects();
    /**
    Site *s = board.inSite(event->pos());
    if(s){
        board.doubleClick(s);
        updateBoardRects();
    }
    */
}

void CardsWidget::mouseMoveEvent (QMouseEvent * event){
    QWidget::mouseMoveEvent(event);
    board.mouseEvent(event->pos(),moveOp);
    updateBoardRects();
}

void CardsWidget::mousePressEvent(QMouseEvent *event)
{
    if( !board.mouseEvent(event->pos(),pressOp) ) return;

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << board.getDragPixmap() ;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("image/x-puzzle-piece", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(event->pos() - board.getDragPoint() );
    drag->setPixmap(board.getDragPixmap());
    updateBoardRects();

    if(drag->exec()!= Qt::MoveAction)
       testRollback();

/**
    Site *s = board.inSite(event->pos());

    if(s && s->canTake())
        {
         s->Take();
         QPixmap pixmap = s->pixmap();
         if(!pixmap) return;
         QByteArray itemData;
         QDataStream dataStream(&itemData, QIODevice::WriteOnly);

         dataStream << pixmap ;

         QMimeData *mimeData = new QMimeData;
         mimeData->setData("image/x-puzzle-piece", itemData);

         QDrag *drag = new QDrag(this);
         drag->setMimeData(mimeData);
         drag->setHotSpot(event->pos() - s->startPoint());
         drag->setPixmap(pixmap);
         update(inset(s->factRect(),-20));

         if(drag->exec()!= Qt::MoveAction)
            testRollback();
        }*/
}

void CardsWidget::paintEvent(QPaintEvent *event)
{try{
    QPainter painter;
    painter.begin(this);
    painter.drawPixmap(event->rect(),background,event->rect());

    board.draw(&painter);

    painter.end();
}
    catch(...)
        {qDebug() << "Error in painter";}
}

