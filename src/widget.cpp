#include "widget.h"
#include "ui_widget.h"
#include <cmath>
#include <stdexcept>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
using namespace std;

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget), progressSliderTimer(this), rotateTimer(this)
{
    ui->setupUi(this);
    connect(ui->openButton, &QPushButton::clicked, this, &Widget::openButtonClicked);
    connect(ui->openHRTFButton, &QPushButton::clicked, this, &Widget::openHRTFButtonClicked);
    connect(ui->recordButton, &QPushButton::clicked, this, &Widget::recordButtonClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &Widget::playButtonClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &Widget::pauseButtonClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &Widget::stopButtonClicked);
    connect(ui->applyButton, &QPushButton::clicked, this, &Widget::applyButtonClicked);
    connect(ui->rotateCheckBox, &QCheckBox::clicked, this, &Widget::rotateCheckBoxClicked);
    connect(ui->progressSlider, &QSlider::sliderPressed, this, &Widget::progressSliderDown);
    connect(ui->progressSlider, &QSlider::sliderReleased, this, &Widget::progressSliderChanged);
    connect(ui->heightSlider, &QSlider::valueChanged, this, &Widget::heightSliderChanged);
    connect(&this->progressSliderTimer, &QTimer::timeout, this, &Widget::progressSliderTimeout);
    connect(&this->rotateTimer, &QTimer::timeout, this, &Widget::rotateTimeout);
    connect(ui->topView, &TopView::mouseMoved, this, &Widget::topViewMouseMoved);
    progressSliderTimer.setInterval(100);
    progressSliderTimer.start();
    rotateTimer.setInterval(10);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::openButtonClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, QString("Open Audio File"));
    if (filename != "") {
        try {
            p.open(filename.toStdString());
            ui->playButton->setEnabled(true);
            ui->pauseButton->setEnabled(true);
            ui->stopButton->setEnabled(true);
            ui->recordButton->setEnabled(true);
            ui->progressSlider->setEnabled(true);
        } catch (const runtime_error &e) {
            QMessageBox::critical(this, QString("Error"), e.what());
            ui->playButton->setEnabled(false);
            ui->pauseButton->setEnabled(false);
            ui->stopButton->setEnabled(false);
            ui->recordButton->setEnabled(false);
            ui->progressSlider->setEnabled(false);
        }
    }
}

void Widget::openHRTFButtonClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, QString("Open HRTF File"), QString(), QString("SOFA Files (*.sofa)"));
    if (filename != "") {
        try {
            p.open_hrtf(filename.toStdString());
        } catch (const runtime_error &e) {
            QMessageBox::critical(this, QString("Error"), e.what());
        }
    }
}

void Widget::recordButtonClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, QString("Save WAV File"), QString(), QString("Wave Files (*.wav)"));
    if (filename != "")
        p.set_record(filename.toStdString());
}

void Widget::playButtonClicked()
{
    p.play();
}

void Widget::pauseButtonClicked()
{
    p.pause();
}

void Widget::stopButtonClicked()
{
    p.stop();
}

void Widget::applyButtonClicked()
{
    bool x_ok, y_ok, z_ok;
    float x = ui->xEdit->text().toFloat(&x_ok),
          y = ui->yEdit->text().toFloat(&y_ok),
          z = ui->zEdit->text().toFloat(&z_ok);
    if (!x_ok || !y_ok || !z_ok) {
        ui->rotateCheckBox->setChecked(false);
        rotateCheckBoxClicked();
        QMessageBox::critical(this, QString("Error"), QString("please enter valid numbers"));
        return;
    }
    ui->heightSlider->blockSignals(true);
    ui->heightSlider->setValue(int(z * ui->heightSlider->maximum() / 5));
    ui->heightSlider->blockSignals(false);
    ui->topView->set_source(x, y);
    p.set_source(x, y, z);
}

void Widget::rotateCheckBoxClicked()
{
    if (ui->rotateCheckBox->isChecked())
        rotateTimer.start();
    else
        rotateTimer.stop();
}

void Widget::progressSliderDown()
{
    progressSliderTimer.stop();
}

void Widget::progressSliderChanged()
{
    p.set_position(ui->progressSlider->value() / 100.0);
    progressSliderTimer.start();
}

void Widget::heightSliderChanged()
{
    ui->zEdit->setText(QString::number(ui->heightSlider->value() * 5.0 / ui->heightSlider->maximum()));
    applyButtonClicked();
}

void Widget::progressSliderTimeout()
{
    ui->progressSlider->setMaximum(int(p.get_length() * 100));
    ui->progressSlider->setValue(int(p.get_position() * 100));
}

void Widget::rotateTimeout()
{
    bool x_ok, y_ok;
    float x = ui->xEdit->text().toFloat(&x_ok),
          y = ui->yEdit->text().toFloat(&y_ok);
    if (!x_ok || !y_ok) {
        ui->xEdit->setText(QString("1"));
        ui->yEdit->setText(QString("0"));
        applyButtonClicked();
        return;
    }
    double dist = sqrt(double(x * x + y * y));
    double theta = atan2(double(y), double(x));
    theta += M_PI / 500;
    ui->xEdit->setText(QString::number(dist * cos(theta)));
    ui->yEdit->setText(QString::number(dist * sin(theta)));
    applyButtonClicked();
}

void Widget::topViewMouseMoved(QMouseEvent *event)
{
    QPointF c = ui->topView->mapToScene(event->pos());
    ui->xEdit->setText(QString::number(-c.y() / 50));
    ui->yEdit->setText(QString::number(-c.x() / 50));
    applyButtonClicked();
}
