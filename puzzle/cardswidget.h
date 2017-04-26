#ifndef CARDSWIDGET_H
#define CARDSWIDGET_H

#include <QList>
#include <QPoint>
#include <QPixmap>
#include <QWidget>
#include "board.h"

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
QT_END_NAMESPACE
/**
* Main Graphics Space for Drawing
*/
class CardsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardsWidget(QWidget *parent = 0);
    void clear();
    void setSize(int w, int h);
    void newGame();
    void undo();

    void testVictory(); // temp
    void autoComplete();
// ----- Cards Functions
    void setBackground(const QString &path);

signals:
    void puzzleCompleted(); // Not used
private slots:
    void setCompleted();
    void trackCardMoved();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent (QMouseEvent * event);
    void paintEvent(QPaintEvent *event);

    void mouseDoubleClickEvent ( QMouseEvent * event );

    void updateBoardRects();
    void testRollback();

private:
    int hSize;
    int wSize;

    Board board;

    QPixmap background;

};

#endif // CARDSWIDGET_H
