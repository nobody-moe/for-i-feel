#include <QtGui>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <stdlib.h>

#include "mainwindow.h"
//#include "puzzlewidget.h"
#include "cardswidget.h"

/** num divisions by which puzzle image devided equally Horizontal & Vertical*/
const int numPieces = 5;
const int maxSize =1024;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupMenus();
    setupWidgets();

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setWindowTitle(tr("Пасьянс"));
}

void MainWindow::setCompleted()
{
    QMessageBox::information(this, tr("Puzzle Completed"),
        tr("Congratulations! You have completed the puzzle!\n"
           "Click OK to start again."),
        QMessageBox::Ok);

    newGame();
}



QSize MainWindow::calculateSize(){
  /* QSize realSize = puzzleImage.size();
   int smax = qMax(puzzleImage.width(), puzzleImage.height());

   if(smax > maxSize){
       qreal factor;
       if(puzzleImage.width() > puzzleImage.height())
            factor = puzzleImage.width()/maxSize;
       else
           factor = puzzleImage.height()/maxSize;
      realSize/= factor;
   }
   return realSize;*/
   return QSize(300,300);
}

void MainWindow::setupPuzzle()
{
    puzzleWidget->clear();
}

void MainWindow::newGame(){
    puzzleWidget->newGame();
}

void MainWindow::undo(){
    puzzleWidget->undo();
}

void MainWindow::testVictory(){
    puzzleWidget->testVictory();
}

void MainWindow::autoComplete(){
    puzzleWidget->autoComplete();
}


void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcuts(QKeySequence::Open);

    QAction *exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcuts(QKeySequence::Quit);

    QMenu *gameMenu = menuBar()->addMenu(tr("&Game"));

    QAction *restartAction = gameMenu->addAction(tr("&Restart"));
    restartAction->setShortcuts(QKeySequence::New);

    QAction *undoAction = gameMenu->addAction(tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    QAction *testAction = gameMenu->addAction(tr("&Test"));
    testAction->setShortcut(QKeySequence(Qt::Key_T));

    QAction *autocompleteAction = gameMenu->addAction(tr("&Auto"));
    autocompleteAction->setShortcut(QKeySequence(Qt::Key_A));

    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(restartAction, SIGNAL(triggered()), this, SLOT(newGame()));
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));
    connect(testAction, SIGNAL(triggered()), this, SLOT(testVictory()));
    connect(autocompleteAction, SIGNAL(triggered()), this, SLOT(autoComplete()));
}

void MainWindow::setupWidgets()
{
    puzzleWidget = new CardsWidget;
    puzzleWidget->setBackground(":/images/fon");

    connect(puzzleWidget, SIGNAL(puzzleCompleted()),
            this, SLOT(setCompleted()), Qt::QueuedConnection);
    setCentralWidget(puzzleWidget);
}
