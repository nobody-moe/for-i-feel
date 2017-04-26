#ifndef UTIL_H
#define UTIL_H

#include <QtGui>



/*********************************************************
  RECT UTILITIES
*********************************************************/
inline
void inset(QRect *r, int x){
    r->setCoords( r->left()+x, r->top()+x, r->right()-x, r->bottom()-x);
}

inline
QRect  inset(QRect r, int x){
    return QRect(r.left()+x , r.top()+x ,r.width() - x*2 ,r.height() - x*2 );
}


#endif // UTIL_H
