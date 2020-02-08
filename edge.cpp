#include "edge.h"

#include <QPainter>
#include <QDebug>
#include <QBitmap>
#include <QMouseEvent>
#include "component.h"
#include "canvas.h"

Edge::Edge(Component* startVertex, Component* endVertex, int weight, const QString& name, QWidget* parent) :
    QLabel(parent), startVertex(startVertex), endVertex(endVertex), weight(weight), name(name)
{
//    setGeometry(templateLabel->geometry());
    qDebug() << "Edge Constructor";
    setAutoFillBackground(false);
    setStyleSheet("QLabel { background-color : none; }");
    setVisible(true);
//    setGeometry(QRect(10, 10, 50, 50));
//    setMouseTracking(true);
    installEventFilter(this);

    isPainting = true;


    lefttop.setX(std::max(0, std::min(startVertex->center.x(), endVertex->center.x())) - 10);
    lefttop.setY(std::max(0, std::min(startVertex->center.y(), endVertex->center.y())) - 10);

    int width = abs(startVertex->center.x() - endVertex->center.x()) + 20;
    int height = abs(startVertex->center.y() - endVertex->center.y()) + 20;

    setGeometry(QRect(lefttop.x(), lefttop.y(), width, height));

    update();
}

void Edge::paintEvent(QPaintEvent* e) {

    QLabel::paintEvent(e);

    QPixmap image(size());
    image.fill(Qt::transparent);

//    if (isPainting) {
//        qDebug() << "Edge Paint";
        QPainter painter(&image);
        QColor color = this->color;
        color.setAlpha(150);
        QPen pen(color);

        pen.setWidth(10);
        painter.setPen(pen);
        painter.drawPoint(startVertex->center - lefttop);

        QLineF line(startVertex->center - lefttop, endVertex->center - lefttop);
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawLine(line);

        painter.setPen(QPen(Qt::red));
        painter.setFont(QFont("Arial", 15, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, QString::number(weight));

        setPixmap(image);
        setMask(image.mask());
//    }
//    isPainting = false;
}

void Edge::vertexMoved(Component* vertex) {
    lefttop.setX(std::max(0, std::min(startVertex->center.x(), endVertex->center.x())) - 10);
    lefttop.setY(std::max(0, std::min(startVertex->center.y(), endVertex->center.y())) - 10);

    int width = abs(startVertex->center.x() - endVertex->center.x()) + 20;
    int height = abs(startVertex->center.y() - endVertex->center.y()) + 20;

    setGeometry(QRect(lefttop.x(), lefttop.y(), width, height));

    update();
}

bool Edge::eventFilter(QObject*, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);
        if (rect().contains(e->pos()) && e->button() == Qt::RightButton) {
            emit deleteEdge(this);
        }
    }
    return false;
}

void Edge::removeSelf() {
    startVertex->removeConnection(this);
    endVertex->removeConnection(this);
}
