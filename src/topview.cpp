#include "topview.h"
#include <QGraphicsEllipseItem>

TopView::TopView(QWidget *parent): QGraphicsView(parent)
{
    scene.setSceneRect(-250, -250, 500, 500);
    scene.addEllipse(-50, -50, 100, 100);
    scene.addEllipse(-100, -100, 200, 200);
    scene.addEllipse(-150, -150, 300, 300);
    scene.addEllipse(-200, -200, 400, 400);
    scene.addEllipse(-250, -250, 500, 500);
    scene.addLine(-250, 0, 250, 0);
    scene.addLine(0, -250, 0, 250);
    point = scene.addEllipse(0, 0, 0, 0, QPen(Qt::GlobalColor::red), QBrush(Qt::GlobalColor::red));
    set_source(1, 0);
    setScene(&scene);
}

void TopView::set_source(float x, float y)
{
    point->setRect(double(-y * 50 - 5), double(-x * 50 - 5), 10, 10);
}

void TopView::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
}

void TopView::mousePressEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
}
