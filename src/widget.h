#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include "player.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget {
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    Ui::Widget *ui;
    Player p;
    QTimer progressSliderTimer;
    QTimer rotateTimer;
private slots:
    void openButtonClicked();
    void openHRTFButtonClicked();
    void recordButtonClicked();
    void playButtonClicked();
    void pauseButtonClicked();
    void stopButtonClicked();
    void applyButtonClicked();
    void rotateCheckBoxClicked();
    void progressSliderDown();
    void progressSliderChanged();
    void heightSliderChanged();
    void progressSliderTimeout();
    void rotateTimeout();
    void topViewMouseMoved(QMouseEvent *event);
};

#endif // WIDGET_H
