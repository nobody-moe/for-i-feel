#include "carddrawer.h"
#include <QtDebug>
#include <QMessageBox>

static QPainter *painter=0;

CardDrawer::CardDrawer(QObject *parent) :
    QObject(parent)
{
    setImages();
}

void CardDrawer::setPainter(QPainter *pntr){
    painter = pntr;
}

void CardDrawer::drawCard(int x, int y, int value,int type, bool haveShadow, bool haveGlow){

    if( haveGlow    ) painter->drawImage(x,y,glow);
    if( haveShadow  ) painter->drawImage(x,y,shadow);

    painter->drawImage(x+3,y+3,cardImages[value][type]);
}

void CardDrawer::drawCardGlow(int x, int y, int value, int type){
    drawCard( x, y, value, type,0,1);
}

void CardDrawer::drawCardShadow(int x, int y, int value, int type){
    drawCard( x, y, value, type,1,0);
}

void CardDrawer::drawCardFull(int x, int y, int value, int type){
    drawCard( x, y, value, type,1,1);
}

void CardDrawer::drawCardEmpty(int x, int y){

    painter->drawImage(x,y,empty);
}

void CardDrawer::drawGlow(int x, int y){
    painter->drawImage(x,y,glow);
}

void CardDrawer::drawBlueGlow(int x, int y){
    painter->drawImage(x,y,glow_blue);
}

void CardDrawer::setImages(const QString &path){
    QImage cardsImage;
    if (!cardsImage.load(path)){
        QMessageBox::warning(0, tr("Open Image"),
                              tr("The image file could not be loaded."),
                              QMessageBox::Cancel);
        return;
    }
    for(int i=0;i<numCardValues;i++)
    for(int j=0;j<numCardTypes;j++)
        cardImages[i][j] = cardsImage.copy(i*cardWidth,j*cardHeight,cardWidth,cardHeight);

    glow.load(":/images/glow");
    shadow.load(":/images/shadow");
    empty.load(":/images/empty");
    glow_blue.load(":/images/glow_blue");
}

QPixmap CardDrawer::pixmap(int value, int type){

    if((value < numCardValues) && (type < numCardTypes)){
        QPixmap *pm = new QPixmap(cardWidth, cardHeight);
        QPainter pr(pm);
        pr.drawImage(0,0,cardImages[value][type]);
        return *pm;
    }
    return QPixmap();

}

QPixmap CardDrawer::stackPixmap(QList<Card*> cList){

    int n = cList.size();
    if(!n) return QPixmap();

    int pH = (n-1)*stacksYOffset+cardHeight;
    QPixmap *pm = new QPixmap(cardWidth, pH);
    QPainter pr(pm);
    for(int i = 0; i < n; i++){
        pr.drawImage(0,i*stacksYOffset,cardImages[cList[i]->value][cList[i]->type]);
    }
    return *pm;
}





