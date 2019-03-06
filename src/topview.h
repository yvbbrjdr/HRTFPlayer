#ifndef TOPVIEW_H
#define TOPVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>

class TopView : public QGraphicsView {
    Q_OBJECT
public:
    explicit TopView(QWidget *parent = nullptr);
    void set_source(float x, float y);
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
private:
    QGraphicsScene scene;
    QGraphicsEllipseItem *point;
signals:
    void mouseMoved(QMouseEvent *event);
};

#endif // TOPVIEW_H
