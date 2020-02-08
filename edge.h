#ifndef EDGE_H
#define EDGE_H

#include <QLabel>


class Component;

class Edge : public QLabel
{
    Q_OBJECT
public:
    Edge(Component*, Component*, int, const QString& name, QWidget* parent = nullptr);
    QString getName() {return name;}
    void removeSelf();
    int getWeight() { return this->weight; }
    QColor color = Qt::green;

private:
    QString name;
    Component* startVertex;
    Component* endVertex;
    int weight;
    bool isPainting = false;
    QPoint lefttop;

protected slots:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *, QEvent *event) override;

public slots:
    void vertexMoved(Component*);

signals:
    void deleteEdge(Edge*);

};

#endif // EDGE_H
