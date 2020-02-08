#include "component.h"
#include <QPainter>
#include <QBitmap>
#include <QEvent>
#include <QMouseEvent>
#include "canvas.h"
#include <QDebug>
#include "edge.h"
#include <QPixmap>

Component::Component(const QString& name, int x, int y, int distance, int label, QWidget* parent) :
    QLabel(parent), name(name), distance(distance), label(label)
{
    setStyleSheet("border : none");
    setStyleSheet("background-color : none");
    setAutoFillBackground(false);
    setGeometry(QRect(x, y, 50, 50));
    setVisible(true);
    installEventFilter(this);
    center = QPoint(x+25, y+25);
}

void Component::setImage(const QString& path) {
    this->imagePath = path;
    update();
}

void Component::paintEvent(QPaintEvent* e) {

    QLabel::paintEvent(e);

//    qDebug() << "Vertex Paint";
//    QImage image(QString(":/icons/yellow_vertex.png"));

    QPixmap image(size());
    image.fill(Qt::transparent);

    QPainter p(&image);
    p.setBrush(color);
    p.drawEllipse(QRect(rect().x()+5, rect().y()+5, 40, 40));
    p.setPen(QPen(Qt::red));
    p.setFont(QFont("Arial", 15, QFont::Bold));
    if (distanceDisplay)
        p.drawText(image.rect(), Qt::AlignCenter, QString::number(distance));

    setPixmap(image);
    setMask(image.mask());
    QWidget::raise();

}

bool Component::eventFilter(QObject*, QEvent* event)
{
    static QPoint lastPoint;
    static bool isHover = false;

    if (event->type() == QEvent::MouseButtonPress) {

        if (Canvas::actionType == ActionType::DrawEdge) {
            emit sendVertex(this);
            qDebug() << "SENDDDDDDDDD";
            return false;
        }

        if (Canvas::actionType == ActionType::PickSource) {
            emit pickVertexAsSource(this);
            qDebug() << "PICKED";
            return false;
        }

        QMouseEvent *e = static_cast<QMouseEvent*>(event);

        if (Canvas::actionType == ActionType::PickDestination) {
            emit pickVertexAsDestination(this);
            lastPoint = e->pos();
            isHover = true;
            qDebug() << "START PRESS";
            return false;
        }

        if (rect().contains(e->pos())
                && e->button() == Qt::LeftButton) {
            lastPoint = e->pos();
            isHover = true;
            qDebug() << "START PRESS";
        }

        if (rect().contains(e->pos()) && e->button() == Qt::RightButton) {
            emit deleteVertex(this);
        }

    } else if (event->type() == QEvent::MouseMove && isHover) {

        QMouseEvent* e = static_cast<QMouseEvent*>(event);

        int dx = e->pos().x() - lastPoint.x();
        int dy = e->pos().y() - lastPoint.y();
        int new_x = x() + dx;
        int new_y = y() + dy;

        qDebug() << "MOVE !!!";
        move(new_x, new_y);
        center = QPoint(new_x+25, new_y+25);
        moveVertex(this);

    } else if (event->type() == QEvent::MouseButtonRelease && isHover) {

        isHover = false;
    }

    return false;
}

void Component::addConnection(Component *vertex, Edge *edge) {
    connections.push_back(std::make_pair(vertex, edge));
}

void Component::removeConnection(Component *vertex) {
    std::vector<std::pair<Component*, Edge*>>::iterator it;
    for (it = connections.begin(); it != connections.end();) {
        if ((*it).first == vertex) {
            connections.erase(it);
        } else {
            ++it;
        }
    }
}

void Component::removeConnection(Edge *edge) {
    std::vector<std::pair<Component*, Edge*>>::iterator it;
    for (it = connections.begin(); it != connections.end();) {
        if ((*it).second == edge) {
            connections.erase(it);
        } else {
            ++it;
        }
    }
}

void Component::removeSelf() {
    std::vector<std::pair<Component*, Edge*>>::iterator it;
    for (it = connections.begin(); it != connections.end(); ++it) {
        (*it).first->removeConnection(this);
        (*it).second->deleteLater();
    }
}
