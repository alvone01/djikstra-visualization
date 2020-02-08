#ifndef CANVAS_H
#define CANVAS_H

#include <QLabel>
#include "component.h"
#include <QStack>
#include "avl.h"

enum ActionType {
    DrawVertex, DrawEdge, MoveVertex, PickSource, PickDestination
};

class Canvas : public QLabel
{
    Q_OBJECT
public:
    static ActionType actionType;

    Canvas(QWidget* = nullptr);

    std::vector<Component*> adjList;
    QStack<std::pair<Component*, Edge*>> path;

    int totalVertices = 0;
    void clearDjikstra();
    void cleanPath(QColor color);
    void randGenerate();
    void cleanGraph();
    void resetCreateEdge();

    Component** my_hashtable;
    AVL<Component*> my_priorityQueue;
    std::vector<std::pair<Component*, Edge*>>::iterator my_it;
    Edge* currTestedEdge = nullptr;
    Component* currTestedVertex = nullptr;
    Component* updatedVertex = nullptr;

    bool djikstraDone = false;
    bool djikstraStarted = false;

private:
    Component* firstPointForPotentialEdge = nullptr;
    Component* currVertex = nullptr;
    Component* sourceVertex = nullptr;
    QString generateName(const QString& type);
    int generateIndex();
    void fillUnivisted(Component**);

protected slots:
    void mousePressEvent(QMouseEvent *event) override;
    void addVertexToPotentialEdge(Component*);
    void onDeleteEdge(Edge*);
    void onDeleteVertex(Component*);
    void startCalculation(Component*);
    void showPath(Component*);

public slots:
    void stepCalculation(Component* = nullptr);

signals:
    void calculationFinished();
    void calculationCleared();
    void calculationStarted();
};

#endif // CANVAS_H
