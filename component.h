#ifndef COMPONENT_H
#define COMPONENT_H

#include <QLabel>
#include <QString>

class Edge;

class Component : public QLabel
{
    Q_OBJECT
public:
    Component(const QString& name, int, int, int, int, QWidget* = nullptr);

    QPoint center;
    bool distanceDisplay = false;
    QColor color = Qt::white;
    std::vector<std::pair<Component*, Edge*>> connections;

    QString getName() {return name;}
    void setImage(const QString&);
    void addConnection(Component*, Edge*);
    void removeConnection(Component*);
    void removeConnection(Edge*);
    void removeSelf();
    int getDistance() const { return this->distance; }
    void setDistance(int distance) { this->distance = distance; }
    int getLabel() { return this->label; }
    Component* getPrevVertex() { return this->prevVertex; }
    void setPrevVertex(Component* prev) { this->prevVertex = prev; }

private:
    QString name;
    QString imagePath;
    int distance;
    int label;
    Component* prevVertex = nullptr;

protected slots:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *, QEvent *event) override;

signals:
    void sendVertex(Component*);
    void moveVertex(Component*);
    void deleteVertex(Component*);
    void pickVertexAsSource(Component*);
    void pickVertexAsDestination(Component*);
};

#endif // COMPONENT_H
