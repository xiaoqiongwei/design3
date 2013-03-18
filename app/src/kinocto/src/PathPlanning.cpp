#include "PathPlanning.h"

using namespace std;
using namespace cv;

PathPlanning::PathPlanning() {
    white = Scalar(255, 255, 255);
    blue = Scalar(255, 0, 0);
    black = Scalar(0, 0, 0);

    obstacle1.x = 0;
    obstacle1.y = 0;
    obstacle2.x = 0;
    obstacle2.y = 0;

    startNode = 0;
    destinationNode = 0;

    for (int y = 0; y <= TABLE_Y; y++) {
        for (int x = 0; x <= TABLE_X; x++) {
            table[x][y] = 0;
        }
    }
}

PathPlanning::~PathPlanning() {
    cleanGoalNodes();
    deleteAllNodes();
}

void PathPlanning::deleteAllNodes() {
    for (int i = 0; i < listOfNodes.size(); i++) {
        Node * node = listOfNodes[i];
        listOfNodes[i] = 0;

        delete node;
        node = 0;
    }

    listOfNodes.clear();
}

void PathPlanning::setObstacles(Position o1, Position o2) {
    deleteAllNodes();

    if (obstaclesPositionsOK(o1, o2)) {
        if (o2.x < o1.x) {
            obstacle1.x = o2.x;
            obstacle1.y = o2.y;
            obstacle2.x = o1.x;
            obstacle2.y = o1.y;
        }
        obstacle1.x = o1.x;
        obstacle1.y = o1.y;
        obstacle2.x = o2.x;
        obstacle2.y = o2.y;
    }
}

bool PathPlanning::obstaclesPositionsOK(Position obstacle1, Position obstacle2) {
    if (obstacle1.x < DRAWING_ZONE || obstacle2.x < DRAWING_ZONE) {
//        cout << "Error : An obstacle is in the drawing zone" << endl;
    	ROS_ERROR("Error : An obstacle is in the drawing zone");
        return false;
    }
    if (obstacle1.x > TABLE_X - OBSTACLE_RADIUS || obstacle2.x > TABLE_X - OBSTACLE_RADIUS) {
//        cout << "Error : An obstacle is in the west wall (x too high)" << endl;
        ROS_ERROR("Error : An obstacle is in the west wall (x too high)");
        return false;
    }
    if (obstacle1.y < OBSTACLE_RADIUS || obstacle2.y < OBSTACLE_RADIUS) {
//        cout << "Error : An obstacle is in the north wall (y too low)" << endl;
        ROS_ERROR("Error : An obstacle is in the north wall (y too low)");
        return false;
    }
    if (obstacle1.y > TABLE_Y - OBSTACLE_RADIUS || obstacle2.y > TABLE_Y - OBSTACLE_RADIUS) {
//        cout << "Error : An obstacle is in the south wall (y too high)" << endl;
        ROS_ERROR("Error : An obstacle is in the south wall (y too high)");
        return false;
    }
    return true;
}

vector<Position> PathPlanning::getPath(Position start, Position destination) {
    cleanGoalNodes();
    startNode = new Node(start);
    startNode->setCost(0.0f);
    destinationNode = new Node(destination);
    destinationNode->setCost(10000.0f);

    constructGraph();

    vector<Position> positions = findPathInGraph();

    return positions; //convertToMoves(positions, startAngle, destinationAngle);
}

void PathPlanning::cleanGoalNodes() {
    if (startNode != 0) {
        startNode->resetConnexions();
        delete startNode;
        startNode = 0;
    }
    if (destinationNode != 0) {
        destinationNode->resetConnexions();
        delete destinationNode;
        destinationNode = 0;
    }
}

vector<Position> PathPlanning::findPathInGraph() {
    applyDijkstra();

//	cout << "PATH  --  total cost : " << destinationNode->getCost() << endl;
    vector<Position> nodePositions;
    Node* current = destinationNode;
    Node* predecessor = current->getPredecessor();

    Position destinationPosition;
    destinationPosition.x = destinationNode->getPosition().x;
    destinationPosition.y = destinationNode->getPosition().y;
    nodePositions.push_back(destinationPosition);
    while (predecessor != 0) {
        current = predecessor;
        predecessor = current->getPredecessor();
//		cout << "(" << current->getPosition().x << "," << current->getPosition().y << ")" << endl;
        Position currentPosition(current->getPosition().x, current->getPosition().y);
        nodePositions.push_back(currentPosition);
    }
//	cout << "(" << current->getPosition().x << "," << current->getPosition().y << ")" << endl;
    return nodePositions;
}

vector<Move> PathPlanning::convertToMoves(vector<Position> positions, float startAngle, float destinationAngle) {
    vector<Move> moves;
    float robotCurrentAngle = startAngle;
    Position startPosition;
    Position endPosition;

    for (unsigned int i = (positions.size() - 1); i > 0; i--) {
        startPosition = positions[i];
        endPosition = positions[i - 1];

        Move move(calculateAngle(robotCurrentAngle, startPosition, endPosition), calculateCost(startPosition, endPosition), endPosition);
        robotCurrentAngle += move.angle;

        moves.push_back(move);
    }
    Move move((destinationAngle - robotCurrentAngle), 0);
    moves.push_back(move);
    return moves;
}

void PathPlanning::applyDijkstra() {
    std::queue<Node*> nodesToVisit;
    bool goingRight = true;

    if (startNode->getPosition().x < destinationNode->getPosition().x) {
        goingRight = true;
    } else {
        goingRight = false;
    }

    nodesToVisit.push(startNode);

    do {
        Node* node = nodesToVisit.front();
        nodesToVisit.pop();

        vector<Node*> neighbors;
        if (goingRight) {
            neighbors = node->getRightNeighbors();
        } else {
            neighbors = node->getLeftNeighbors();
        }

        for (unsigned int i = 0; i < neighbors.size(); i++) {
            float cost = node->getCost();
            cost += calculateCost(node->getPosition(), neighbors[i]->getPosition());
            if (cost < neighbors[i]->getCost()) {
                neighbors[i]->setPredecessor(node);
                neighbors[i]->setCost(cost);
                nodesToVisit.push(neighbors[i]);
            }
        }
        neighbors.clear();
    } while (nodesToVisit.size() > 0);
}

float PathPlanning::calculateCost(Position p1, Position p2) {
    int x = p1.x - p2.x;
    if (x < 0) {
        x *= -1;
    }
    int y = p1.y - p2.y;
    if (y < 0) {
        y *= -1;
    }
    return sqrt(pow(x, 2) + pow(y, 2));
}

float PathPlanning::calculateAngle(float robotCurrentAngle, Position p1, Position p2) {
//	cout << "robot current angle : " << robotCurrentAngle << endl;
//	cout << "    position : (" << p1.x << "," << p1.y << ") (" << p2.x << "," << p2.y << ")" << endl;

    int x = p1.x - p2.x;
    int multFactor = -1;
    int addFactor = -180;
//	cout << "    x : " << x << endl;
    if (x < 0) {
        addFactor = 0;
        multFactor *= -1;
        x *= -1;
    }
    int y = p1.y - p2.y;
//	cout << "    y : " << y << endl;
    if (y < 0) {
        multFactor *= -1;
        y *= -1;
    }
    float ratio = (float) y / (float) x;
//	cout << "    ratio : " << ratio << endl;
    float angle = atan(ratio) * 180 / PI;
//	cout << "    angle : " << angle << endl;
//	cout << "    addfactor : " << addFactor << endl;
//	cout << "    multfactor : " << multFactor << endl;
    angle += addFactor;
    angle *= multFactor;
//	cout << "    angle : " << angle << endl;
//	cout << "    robot current angle : " << robotCurrentAngle << endl;
    float result = (angle - robotCurrentAngle);
//	cout << "    result angle move : " << result << endl;

    if (result > 180) {
        result -= 360;
    }
    return result;
}

void PathPlanning::constructGraph() {
    updateMatrixTable();

    if (listOfNodes.empty()) {
        createNodes();
    }
    clearConnections();
    connectNodes();
}

void PathPlanning::createNodes() {
    updateMatrixTable();

    listOfNodes.clear();

    vector<Position> obstacleCornerValues = getObstacleCorners();

    for (unsigned int i = 0; i < obstacleCornerValues.size(); i++) {
        if (table[obstacleCornerValues[i].x][(obstacleCornerValues[i].y + 1)] == 1) {
            int nextObstacleY = obstacleCornerValues[i].y + 1;
            do {
                nextObstacleY++;
            } while (table[obstacleCornerValues[i].x][nextObstacleY] == 1);
            int nodeY = obstacleCornerValues[i].y + ((nextObstacleY - obstacleCornerValues[i].y) / 2);
            Node* newNode = new Node(obstacleCornerValues[i].x, nodeY);
            addNode(newNode);
        } else if (table[obstacleCornerValues[i].x][obstacleCornerValues[i].y - 1] == 1) {
            int nextObstacleY = obstacleCornerValues[i].y - 1;
            do {
                nextObstacleY--;
            } while (table[obstacleCornerValues[i].x][nextObstacleY] == 1);
            int nodeY = obstacleCornerValues[i].y - ((obstacleCornerValues[i].y - nextObstacleY) / 2);
            Node* newNode = new Node(obstacleCornerValues[i].x, nodeY);
            addNode(newNode);
        }
    }
}

void PathPlanning::addNode(Node* newNode) {
    std::vector<Node*>::iterator it = find(listOfNodes.begin(), listOfNodes.end(), newNode);
    if (it == listOfNodes.end()) {
        listOfNodes.push_back(newNode);
    } else {
        delete newNode;
    }
}

vector<Position> PathPlanning::getObstacleCorners() {
    vector<Position> corners;

    Position p;
    p.set(obstacle1.x - TOTAL_OBSTACLE_RADIUS, obstacle1.y + TOTAL_OBSTACLE_RADIUS);
    corners.push_back(p);
    p.set(obstacle1.x - TOTAL_OBSTACLE_RADIUS, obstacle1.y - TOTAL_OBSTACLE_RADIUS);
    corners.push_back(p);
    p.set(obstacle1.x + TOTAL_OBSTACLE_RADIUS, obstacle1.y + TOTAL_OBSTACLE_RADIUS);
    corners.push_back(p);
    p.set(obstacle1.x + TOTAL_OBSTACLE_RADIUS, obstacle1.y - TOTAL_OBSTACLE_RADIUS);
    corners.push_back(p);
    p.set(obstacle2.x - TOTAL_OBSTACLE_RADIUS, obstacle2.y + TOTAL_OBSTACLE_RADIUS);
    corners.push_back(p);
    p.set(obstacle2.x - TOTAL_OBSTACLE_RADIUS, obstacle2.y - TOTAL_OBSTACLE_RADIUS);
    corners.push_back(p);
    p.set(obstacle2.x + TOTAL_OBSTACLE_RADIUS, obstacle2.y + TOTAL_OBSTACLE_RADIUS);
    corners.push_back(p);
    p.set(obstacle2.x + TOTAL_OBSTACLE_RADIUS, obstacle2.y - TOTAL_OBSTACLE_RADIUS);
    corners.push_back(p);

    return corners;
}

void PathPlanning::clearConnections() {
    for (unsigned int i = 0; i < listOfNodes.size(); i++) {
        listOfNodes[i]->resetConnexions();
        listOfNodes[i]->setCost(10000.0f);
    }
}

void PathPlanning::connectNodes() {
    for (unsigned int i = 0; i < listOfNodes.size(); i++) {
        for (unsigned int j = 0; j < listOfNodes.size(); j++) {
            if (i < j) {
                if (!linePassesThroughObstacle(listOfNodes[i]->getPosition(), listOfNodes[j]->getPosition())) {
//					cout << "    adding connection between : (" << listOfNodes[i]->getPosition().x << "," << listOfNodes[i]->getPosition().y << ") and (" << listOfNodes[j]->getPosition().x << "," << listOfNodes[j]->getPosition().y << ")" << endl;
                    listOfNodes[i]->addNeighbor(listOfNodes[j]);
                    listOfNodes[j]->addNeighbor(listOfNodes[i]);
                }
            }
        }
        if (!linePassesThroughObstacle(listOfNodes[i]->getPosition(), startNode->getPosition())) {
//			cout << "    adding connection between : (" << listOfNodes[i]->getPosition().x << "," << listOfNodes[i]->getPosition().y << ") and (" << startNode->getPosition().x << "," << startNode->getPosition().y << ")" << endl;
            listOfNodes[i]->addNeighbor(startNode);
            startNode->addNeighbor(listOfNodes[i]);
        }
        if (!linePassesThroughObstacle(listOfNodes[i]->getPosition(), destinationNode->getPosition())) {
//			cout << "    adding connection between : (" << listOfNodes[i]->getPosition().x << "," << listOfNodes[i]->getPosition().y << ") and (" << destinationNode->getPosition().x << "," << destinationNode->getPosition().y << ")" << endl;
            listOfNodes[i]->addNeighbor(destinationNode);
            destinationNode->addNeighbor(listOfNodes[i]);
        }
        if (!linePassesThroughObstacle(startNode->getPosition(), destinationNode->getPosition())) {
//			cout << "    adding connection between : (" << startNode->getPosition().x << "," << startNode->getPosition().y << ") and (" << destinationNode->getPosition().x << "," << destinationNode->getPosition().y << ")" << endl;
            startNode->addNeighbor(destinationNode);
            destinationNode->addNeighbor(startNode);
        }
    }
}

bool PathPlanning::linePassesThroughObstacle(Position p1, Position p2) {
    if ((p2.x - p1.x) == 0)
        return true;

    Position upperLeftObstacle1;
    upperLeftObstacle1.x = obstacle1.x - TOTAL_OBSTACLE_RADIUS;
    upperLeftObstacle1.y = obstacle1.y + TOTAL_OBSTACLE_RADIUS;
    Position upperRightObstacle1;
    upperRightObstacle1.x = obstacle1.x + TOTAL_OBSTACLE_RADIUS;
    upperRightObstacle1.y = obstacle1.y + TOTAL_OBSTACLE_RADIUS;
    Position lowerLeftObstacle1;
    lowerLeftObstacle1.x = obstacle1.x - TOTAL_OBSTACLE_RADIUS;
    lowerLeftObstacle1.y = obstacle1.y - TOTAL_OBSTACLE_RADIUS;
    Position lowerRightObstacle1;
    lowerRightObstacle1.x = obstacle1.x + TOTAL_OBSTACLE_RADIUS;
    lowerRightObstacle1.y = obstacle1.y - TOTAL_OBSTACLE_RADIUS;
    Position upperLeftObstacle2;
    upperLeftObstacle2.x = obstacle2.x - TOTAL_OBSTACLE_RADIUS;
    upperLeftObstacle2.y = obstacle2.y + TOTAL_OBSTACLE_RADIUS;
    Position upperRightObstacle2;
    upperRightObstacle2.x = obstacle2.x + TOTAL_OBSTACLE_RADIUS;
    upperRightObstacle2.y = obstacle2.y + TOTAL_OBSTACLE_RADIUS;
    Position lowerLeftObstacle2;
    lowerLeftObstacle2.x = obstacle2.x - TOTAL_OBSTACLE_RADIUS;
    lowerLeftObstacle2.y = obstacle2.y - TOTAL_OBSTACLE_RADIUS;
    Position lowerRightObstacle2;
    lowerRightObstacle2.x = obstacle2.x + TOTAL_OBSTACLE_RADIUS;
    lowerRightObstacle2.y = obstacle2.y - TOTAL_OBSTACLE_RADIUS;

    if (linesCrosses(p1, p2, upperLeftObstacle1, upperRightObstacle1))
        return true;
    if (linesCrosses(p1, p2, upperLeftObstacle1, lowerLeftObstacle1))
        return true;
    if (linesCrosses(p1, p2, lowerLeftObstacle1, lowerRightObstacle1))
        return true;
    if (linesCrosses(p1, p2, upperRightObstacle1, lowerRightObstacle1))
        return true;
    if (linesCrosses(p1, p2, upperLeftObstacle2, upperRightObstacle2))
        return true;
    if (linesCrosses(p1, p2, upperLeftObstacle2, lowerLeftObstacle2))
        return true;
    if (linesCrosses(p1, p2, lowerLeftObstacle2, lowerRightObstacle2))
        return true;
    if (linesCrosses(p1, p2, upperRightObstacle2, lowerRightObstacle2))
        return true;

    return false;
}

bool PathPlanning::linesCrosses(Position line1p1, Position line1p2, Position line2p1, Position line2p2) {
    return DoLineSegmentsIntersect(line1p1.x, line1p1.y, line1p2.x, line1p2.y, line2p1.x, line2p1.y, line2p2.x, line2p2.y);
}

void PathPlanning::printTable() {
    updateMatrixTable();

    Mat workspace = Mat(TABLE_X + 1, TABLE_Y + 1, CV_8UC3, white);

    for (int y = TABLE_Y; y >= 0; y--) {
        for (int x = 0; x <= TABLE_X; x++) {
            if (table[x][y] == 9) {
                colorPixel(workspace, black, x, TABLE_Y - y);
            }
            if (table[x][y] == 2) {
                colorPixel(workspace, blue, x, TABLE_Y - y);
            }
        }
    }
    transpose(workspace, workspace);

    Node* current = destinationNode;
    Node* predecessor = current->getPredecessor();
    while (predecessor != 0) {
        Point currentPoint(current->getPosition().x, TABLE_Y - current->getPosition().y);
        Point predecessorPoint(predecessor->getPosition().x, TABLE_Y - predecessor->getPosition().y);
        drawLine(workspace, currentPoint, predecessorPoint);
        current = predecessor;
        predecessor = current->getPredecessor();
    }

    showWindowWith("Workspace", workspace);
}

void PathPlanning::updateMatrixTable() {
//	WALLS IN BLACK
    for (int y = 0; y <= TABLE_Y; y++) {
        for (int x = 0; x <= TABLE_X; x++) {
            if (y <= BUFFER_SIZE || y >= TABLE_Y - BUFFER_SIZE || x <= BUFFER_SIZE || x >= TABLE_X - BUFFER_SIZE) {
                table[x][y] = 9;
            } else {
                table[x][y] = 1;
            }
        }
    }
//	OBSTACLES IN BLACK
    if (obstacle1.x != 0 && obstacle1.y != 0) {
        for (int y = (obstacle1.y - TOTAL_OBSTACLE_RADIUS); y <= (obstacle1.y + TOTAL_OBSTACLE_RADIUS); y++) {
            for (int x = (obstacle1.x - TOTAL_OBSTACLE_RADIUS); x <= (obstacle1.x + TOTAL_OBSTACLE_RADIUS); x++) {
                table[x][y] = 9;
            }
        }
    }
    if (obstacle2.x != 0 && obstacle2.y != 0) {
        for (int y = (obstacle2.y - TOTAL_OBSTACLE_RADIUS); y <= (obstacle2.y + TOTAL_OBSTACLE_RADIUS); y++) {
            for (int x = (obstacle2.x - TOTAL_OBSTACLE_RADIUS); x <= (obstacle2.x + TOTAL_OBSTACLE_RADIUS); x++) {
                table[x][y] = 9;
            }
        }
    }
//	NODES IN BLUE
    for (unsigned int i = 0; i < listOfNodes.size(); i++) {
        table[listOfNodes[i]->getPosition().x][listOfNodes[i]->getPosition().y] = 2;
    }
    if (startNode != 0) {
        table[startNode->getPosition().x][startNode->getPosition().y] = 2;
    }
    if (destinationNode != 0) {
        table[destinationNode->getPosition().x][destinationNode->getPosition().y] = 2;
    }
}

void PathPlanning::colorPixel(Mat &mat, Scalar color, int x, int y) {
    mat.at<cv::Vec3b>(x, y)[0] = color[0];
    mat.at<cv::Vec3b>(x, y)[1] = color[1];
    mat.at<cv::Vec3b>(x, y)[2] = color[2];
}

void PathPlanning::showWindowWith(const char* name, const Mat &mat) {
    namedWindow(name, CV_WINDOW_AUTOSIZE);
    imshow(name, mat);
    waitKey(0);
}

void PathPlanning::drawLine(Mat img, Point start, Point end) {
    int thickness = 2;
    int lineType = 8;
    line(img, start, end, blue, thickness, lineType);
}

//Code from http://ptspts.blogspot.ca/2010/06/how-to-determine-if-two-line-segments.html
bool PathPlanning::IsOnSegment(double xi, double yi, double xj, double yj, double xk, double yk) {
    return (xi <= xk || xj <= xk) && (xk <= xi || xk <= xj) && (yi <= yk || yj <= yk) && (yk <= yi || yk <= yj);
}

char PathPlanning::ComputeDirection(double xi, double yi, double xj, double yj, double xk, double yk) {
    double a = (xk - xi) * (yj - yi);
    double b = (xj - xi) * (yk - yi);
    return a < b ? -1 : a > b ? 1 : 0;
}

/** Do line segments (x1, y1)--(x2, y2) and (x3, y3)--(x4, y4) intersect? */
bool PathPlanning::DoLineSegmentsIntersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
    char d1 = ComputeDirection(x3, y3, x4, y4, x1, y1);
    char d2 = ComputeDirection(x3, y3, x4, y4, x2, y2);
    char d3 = ComputeDirection(x1, y1, x2, y2, x3, y3);
    char d4 = ComputeDirection(x1, y1, x2, y2, x4, y4);
    return (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
            || (d1 == 0 && IsOnSegment(x3, y3, x4, y4, x1, y1)) || (d2 == 0 && IsOnSegment(x3, y3, x4, y4, x2, y2))
            || (d3 == 0 && IsOnSegment(x1, y1, x2, y2, x3, y3)) || (d4 == 0 && IsOnSegment(x1, y1, x2, y2, x4, y4));
}
