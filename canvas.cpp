#include "canvas.h"
#include "component.h"
#include "edge.h"

#include <QMouseEvent>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

#include "avl.h"

ActionType Canvas::actionType = ActionType::MoveVertex;

Canvas::Canvas(QWidget* parent) :
    QLabel(parent)
{
    setAutoFillBackground(false);
    setGeometry(QRect(10, 60, 781, 511));
    setStyleSheet("QLabel { background-color : white; }");
    setVisible(true);
    installEventFilter(this);
    qDebug() << "REKT";
}


void Canvas::mousePressEvent(QMouseEvent* e) {

    if (Canvas::actionType == ActionType::DrawVertex) {
        int x = e->pos().x();
        int y = e->pos().y();
        qDebug() << x << y;
        Component* comp = new Component(generateName("Vertex"), x-25, y-25, INT_MAX, generateIndex(), this);
        connect(comp, &Component::sendVertex, this, &Canvas::addVertexToPotentialEdge);
        connect(comp, &Component::deleteVertex, this, &Canvas::onDeleteVertex);
        //connect(comp, &Component::pickVertexAsSource, this, &Canvas::startCalculation);
        connect(comp, &Component::pickVertexAsSource, this, &Canvas::stepCalculation);
        connect(comp, &Component::pickVertexAsDestination, this, &Canvas::showPath);
        adjList.push_back(comp);
        totalVertices++;
    }
    return;
}

void Canvas::addVertexToPotentialEdge(Component* vertex) {
    if ((firstPointForPotentialEdge && vertex) && (firstPointForPotentialEdge != vertex)) {
        vertex->color = Qt::yellow;
        bool ok = false;
        int weight = QInputDialog::getInt(this, "", tr("Input edge weight"), 2,
                                               1, 2147483647, 1, &ok);
        if (ok) {
            Edge* edge = new Edge(firstPointForPotentialEdge, vertex, weight, generateName("Edge"), this);
            firstPointForPotentialEdge->addConnection(vertex, edge);
            vertex->addConnection(firstPointForPotentialEdge, edge);
            connect(firstPointForPotentialEdge, &Component::moveVertex, edge, &Edge::vertexMoved);
            connect(vertex, &Component::moveVertex, edge, &Edge::vertexMoved);
            connect(edge, &Edge::deleteEdge, this, &Canvas::onDeleteEdge);
        }

        firstPointForPotentialEdge->color = Qt::white;
        vertex->color = Qt::white;
        firstPointForPotentialEdge = nullptr;
        vertex = nullptr;
        return;
    }
    firstPointForPotentialEdge = vertex;
    vertex->color = Qt::yellow;
}

QString Canvas::generateName(const QString& type) {
    static int vertexIndex = 1;
    static int edgeIndex = 1;
    if (type == "Vertex") {
        return QString("Vertex%1").arg(vertexIndex);
    } else if (type == "Edge") {
        return QString("Edge%1").arg(edgeIndex);
    }
    return "";
}

void Canvas::onDeleteEdge(Edge* edge) {
    if (djikstraDone || djikstraStarted) {
        return;
    }
    edge->removeSelf();
    edge->deleteLater();
}

void Canvas::onDeleteVertex(Component* vertex) {
    if (djikstraDone || djikstraStarted) {
        return;
    }
    vertex->removeSelf();
    std::vector<Component*>::iterator it;
    for (it = adjList.begin(); it != adjList.end();) {
        if (vertex == *it) {
            adjList.erase(it);
        } else {
            ++it;
        }
    }
    vertex->deleteLater();
}

void Canvas::startCalculation(Component* vertex) {
    sourceVertex = vertex;
    vertex->setDistance(0);
    vertex->distanceDisplay = true;
    const int total = totalVertices;
    Component** hashtable = new Component*[total]();
    fillUnivisted(hashtable);

    AVL<Component*> priorityQueue;
    priorityQueue.insert(vertex);
    while (!priorityQueue.is_empty()) {
//        do {
//            if (priorityQueue.is_empty()) {
//                delete [] hashtable;
//                Canvas::actionType = ActionType::PickDestination;
//                return;
//            }
//            currVertex = priorityQueue.find_min();
//            priorityQueue.remove(currVertex);
//            qDebug() << "TRIAL" << currVertex->getLabel();
//        } while (hashtable[currVertex->getLabel()%totalVertices] == nullptr);


        currVertex = priorityQueue.find_min();
        priorityQueue.remove(currVertex);
        hashtable[currVertex->getLabel()%totalVertices] = nullptr;

        qDebug() << currVertex->getName() << currVertex->getLabel();
        std::vector<std::pair<Component*, Edge*>>::iterator it;
        for (it = currVertex->connections.begin(); it != currVertex->connections.end(); ++it) {
            if (hashtable[(*it).first->getLabel()%totalVertices] == (*it).first) {
                if (currVertex->getDistance() + (*it).second->getWeight() < (*it).first->getDistance()) {
                    priorityQueue.remove((*it).first);
                    (*it).first->setDistance(currVertex->getDistance() + (*it).second->getWeight());
                    (*it).first->setPrevVertex(currVertex);
                    (*it).first->distanceDisplay = true;
//                    if (hashtable[(*it).first->getLabel()%totalVertices] == (*it).first)
                    priorityQueue.insert((*it).first);
                    qDebug() << "INSERT" << (*it).first->getLabel();
                }
            }
        }
    }
    delete [] hashtable;

    emit calculationFinished();
}


void Canvas::stepCalculation(Component* vertex) {

    if (djikstraDone) {
        QMessageBox::information(this, "Please clear path first", "Please clear path first");
        emit calculationFinished();
        return;
    }
    if (vertex != nullptr) {
        djikstraStarted = true;
        emit calculationStarted();
        sourceVertex = vertex;
        vertex->setDistance(0);
        vertex->distanceDisplay = true;
        const int total = totalVertices;
        my_hashtable = new Component*[total]();
        fillUnivisted(my_hashtable);

        my_priorityQueue.insert(vertex);
        vertex->color = Qt::blue;

        currVertex = my_priorityQueue.find_min();
        my_priorityQueue.remove(currVertex);
        my_hashtable[currVertex->getLabel()%totalVertices] = nullptr;
        currVertex->color = Qt::blue;

        my_it = currVertex->connections.begin();
        return;
    }

    if (currTestedEdge != nullptr) {
        currTestedEdge->color = Qt::green;
    }
    if (updatedVertex != nullptr) {
        updatedVertex->color = Qt::cyan;
    }
    else if (currTestedVertex != nullptr) {
        currTestedVertex->color = Qt::white;
    }
    if (my_it == currVertex->connections.end() || currVertex->connections.empty()) {
        currVertex->color = Qt::gray;
        if (my_priorityQueue.is_empty()) {
            delete [] my_hashtable;
            emit calculationFinished();
            djikstraDone = true;
            return;
        }
        currVertex = my_priorityQueue.find_min();
        my_priorityQueue.remove(currVertex);
        my_hashtable[currVertex->getLabel()%totalVertices] = nullptr;
        currVertex->color = Qt::blue;
        my_it = currVertex->connections.begin();
        currTestedVertex = nullptr;
        updatedVertex = nullptr;
        return;
    }

    if (my_hashtable[(*my_it).first->getLabel()%totalVertices] == (*my_it).first) {
        (*my_it).second->color = Qt::blue;
        if (currVertex->getDistance() + (*my_it).second->getWeight() < (*my_it).first->getDistance()) {
            (*my_it).first->color = Qt::green;
            my_priorityQueue.remove((*my_it).first);
            (*my_it).first->setDistance(currVertex->getDistance() + (*my_it).second->getWeight());
            (*my_it).first->setPrevVertex(currVertex);
            (*my_it).first->distanceDisplay = true;
            my_priorityQueue.insert((*my_it).first);
            qDebug() << "INSERT" << (*my_it).first->getLabel();
            updatedVertex = (*my_it).first;

            currTestedEdge = (*my_it).second;
            currTestedVertex = (*my_it).first;
        } else {
            (*my_it).first->color = Qt::red;
            currTestedEdge = (*my_it).second;
            currTestedVertex = (*my_it).first;
            updatedVertex = (*my_it).first;
        }
        ++my_it;
    } else {
        currTestedEdge = (*my_it).second;
        currTestedVertex = nullptr;
        updatedVertex = nullptr;
        ++my_it;
        stepCalculation();
    }
}

int Canvas::generateIndex() {
    static int index = 0;
    ++index;
    return index;
}

void Canvas::fillUnivisted(Component** hashtable) {
    std::vector<Component*>::iterator it;
    for (it = adjList.begin(); it != adjList.end(); ++it) {
        hashtable[(*it)->getLabel()%totalVertices] = (*it);
        qDebug() << (*it)->getLabel();
    }
}

void Canvas::clearDjikstra() {
    sourceVertex = nullptr;
    cleanPath(Qt::white);
    std::vector<Component*>::iterator it;
    for (it = adjList.begin(); it != adjList.end(); ++it) {
        (*it)->setDistance(INT_MAX);
        (*it)->distanceDisplay = false;
        (*it)->setPrevVertex(nullptr);

        (*it)->color = Qt::white;

    }
    djikstraDone = djikstraStarted = false;
    emit calculationCleared();
}

void Canvas::showPath(Component* destination) {
    if (destination->getPrevVertex() == nullptr && (destination != sourceVertex)) {
        QMessageBox::information(this, "No path found", "No path found");
        return;
    }
    cleanPath(Qt::gray);
    Component* temp = destination;
    temp->color = Qt::blue;
    path.push(std::make_pair(temp, nullptr));
    while (temp->getPrevVertex() != nullptr) {
        std::vector<std::pair<Component*, Edge*>>::iterator it;
        for (it = temp->connections.begin(); it != temp->connections.end(); ++it) {
            if ((*it).first == temp->getPrevVertex()) {
                (*it).first->color = Qt::blue;
                (*it).second->color = Qt::blue;
                path.push((*it));
            }
        }
        temp = temp->getPrevVertex();
    }
}

void Canvas::cleanPath(QColor color) {
    while (!path.isEmpty()) {
        std::pair<Component*, Edge*> set = path.pop();
        set.first->color = color;
        if (set.second != nullptr)
            set.second->color = Qt::green;
    }
}

void Canvas::randGenerate() {

    bool ok1 = false;
    int numVertices = QInputDialog::getInt(this, "", tr("Please enter number of vertices"), 10,
                                           1, 2147483647, 1, &ok1);
    if (!ok1) {
        return;
    }

    bool ok2 = false;
    double density = QInputDialog::getDouble(this, "", "Please enter number of vertices", 0.5,
                                           0, 1, 1, &ok2);
    if (!ok2) {
        return;
    }

    bool ok3 = false;
    int maxWeight = QInputDialog::getInt(this, "", "Please enter number of vertices", 0.5,
                                           1, 2147483647, 1, &ok3);
    if (!ok3) {
        return;
    }

    cleanGraph();
    djikstraDone = djikstraStarted = false;
    emit calculationCleared();

    for (int i = 0; i < numVertices; ++i) {
        int x = (rand() / (double(RAND_MAX))) * 761;
        int y = (rand() / (double(RAND_MAX))) * 491;
        Component* comp = new Component(generateName("Vertex"), x-25, y-25, INT_MAX, generateIndex(), this);
        connect(comp, &Component::sendVertex, this, &Canvas::addVertexToPotentialEdge);
        connect(comp, &Component::deleteVertex, this, &Canvas::onDeleteVertex);
        connect(comp, &Component::pickVertexAsSource, this, &Canvas::stepCalculation);
        connect(comp, &Component::pickVertexAsDestination, this, &Canvas::showPath);
        adjList.push_back(comp);
        totalVertices++;
    }
    int currEdges = 0;
    int maxEdges = numVertices*(numVertices-1)/2;

    int numEdges = density * (double) maxEdges;

    std::vector<Component*>::iterator it;
    std::vector<Component*>::iterator it2;
    for (it = adjList.begin(); it != adjList.end()-1; ++it) {
        for (it2 = it+1; it2 != adjList.end(); ++it2) {

            if (currEdges >= numEdges)
                return;

            int weight = ((rand() / (double(RAND_MAX))) * (maxWeight)) + 1;
            Edge* edge = new Edge(*it, *it2, weight, generateName("Edge"), this);
            (*it2)->addConnection(*it, edge);
            (*it)->addConnection(*it2, edge);
            connect(*it2, &Component::moveVertex, edge, &Edge::vertexMoved);
            connect(*it, &Component::moveVertex, edge, &Edge::vertexMoved);
            connect(edge, &Edge::deleteEdge, this, &Canvas::onDeleteEdge);

            currEdges++;
        }
    }
}

void Canvas::cleanGraph() {
    if (!adjList.empty()) {
        for (std::vector<Component*>::reverse_iterator it = adjList.rbegin(); it != adjList.rend(); ++it) {
            (*it)->removeSelf();
            adjList.erase(std::next(it).base());
            (*it)->deleteLater();
        }
        totalVertices = 0;
    }
}

void Canvas::resetCreateEdge() {
    if (firstPointForPotentialEdge != nullptr) {
        firstPointForPotentialEdge->color = Qt::white;
    }
}
