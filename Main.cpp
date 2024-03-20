#include <iostream>
#include <string>

#include "API.h"

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

char headingResolution(char heading, char turnDirection);
void fastTurn(char heading, char goal);
void wallDetection(int x, int y, char heading);
char returnToStart(int x, int y, char heading);
char flipDirection(char heading);

struct Cell {
    int visited{0};
    char heading;
    char returnHeading;
    bool wallNorth{false};
    bool wallEast{false};
    bool wallSouth{false};
    bool wallWest{false};
};

enum Direction { NORTH, EAST, SOUTH, WEST };

Cell maze[5][7];

int main(int argc, char* argv[]) {
    // coordinates assume we start in the bottom left of a x by y maze, zero indexed.
    int x {0};
    int y {0};
    bool atGoal {false};
    char heading {'n'}; // assuming we start facing north
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");
    while (!atGoal) {
        std::string coordinates = (std::to_string(x) + ", " + std::to_string(y));
        API::setText(x, y, coordinates);
        // mark walls around the current cell.
        wallDetection(x, y, heading);

        // mark visited cells red.
        if (maze[x][y].visited == 0) {
            API::setColor(x, y, 'R');
            maze[x][y].visited++;
        }

        // left-hand rule for maze traversal
        if (!API::wallLeft()) {
            API::turnLeft();
            heading = headingResolution(heading, 'L');
        }
        // if there was a wall on the left and there is a wall in front keep turning right
        while (API::wallFront()) {
            API::turnRight();
            heading = headingResolution(heading, 'R');
        }
        // no wall ahead, move forward
        maze[x][y].heading = heading;
        //log("X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Heading: " + std::string(1, heading) + " at " + coordinates);
        log("Maze heading: " + std::string(1, maze[x][y].heading) + " at " + std::to_string(x) + ", " + std::to_string(y));
        API::moveForward();

        // updating coordinates
        switch (heading) {
            case 'n':
                y++;
                break;
            case 'e':
                x++;
                break;
            case 's':
                y--;
                break;
            case 'w':
                x--;
                break;
        }
        if (maze[x][y].visited == 0)
            maze[x][y].returnHeading = flipDirection(heading);

        // marking cells we've been to before
        if (maze[x][y].visited == 1) {
            maze[x][y].visited++;
            API::setColor(x, y, 'G');
        }
        
        // hard coded location of the goal
        if (x == 2 && y == 6) {
            log("Reached goal");
            atGoal = true;
            API::setText(x, y, "Goal");
            API::setColor(x, y, 'Y');
        }
    } // end while

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 5; j++) {
            log("Maze heading: " + std::string(1, maze[i][j].heading) + " at " + std::to_string(i) + ", " + std::to_string(j));
        }
    }

    heading = returnToStart(x, y, heading);

    // resetting variables
    atGoal = false;
    x = 0;
    y = 0;

    //// traverse 2d maze array and log heading value
    //for (int i = 0; i < 7; i++) {
        //for (int j = 0; j < 5; j++) {
            //log("Maze heading: " + std::string(1, maze[i][j].heading) + " at " + std::to_string(i) + ", " + std::to_string(j));
        //}
    //}

    // traverse again without heading into dead ends.
    while (!atGoal) {
        if (heading != maze[x][y].heading) {
            fastTurn(heading, maze[x][y].heading);
            heading = maze[x][y].heading;
        }
        log("Maze heading: " + std::string(1, maze[x][y].heading) + " at " + std::to_string(x) + ", " + std::to_string(y));
        API::moveForward();
        // updating coordinates
        switch (heading) {
            case 'n':
                y++;
                break;
            case 'e':
                x++;
                break;
            case 's':
                y--;
                break;
            case 'w':
                x--;
                break;
        }

        // hard coded location of the goal
        if (x == 2 && y == 6) {
            log("Reached goal");
            atGoal = true;
        }
    }
}

void fastTurn(char currentDirection, char goalDirection) {
    while (currentDirection != goalDirection) {
        if ((currentDirection == 'n' && goalDirection == 'e') ||
            (currentDirection == 'e' && goalDirection == 's') ||
            (currentDirection == 's' && goalDirection == 'w') ||
            (currentDirection == 'w' && goalDirection == 'n')) {
            API::turnRight();
            currentDirection = headingResolution(currentDirection, 'R');
        } else {
            API::turnLeft();
            currentDirection = headingResolution(currentDirection, 'L');
        }
    }
}

void wallDetection(int x, int y, char heading) {
    if (API::wallLeft()) {
        API::setWall(x, y, headingResolution(heading, 'L'));
    }
    if (API::wallFront()) {
        API::setWall(x, y, heading);
    }
    if (API::wallRight()) {
        API::setWall(x, y, headingResolution(heading, 'R'));
    }
}

char headingResolution(char heading, char turnDirection) {
    if (turnDirection == 'L') {
        switch (heading) {
            case 'n':
                return 'w';
            case 'e':
                return 'n';
            case 's':
                return 'e';
            case 'w':
                return 's';
        }
    } else if (turnDirection == 'R') {
        switch (heading) {
            case 'n':
                return 'e';
            case 'e':
                return 's';
            case 's':
                return 'w';
            case 'w':
                return 'n';
        }
    }
    return heading;
}

char returnToStart(int x, int y, char heading) {
    bool atStart{false};
    while (!atStart) {
        // left-hand rule for maze traversal
        log("X " + std::to_string(x) + " Y " + std::to_string(y));
        if (heading != maze[x][y].returnHeading) {
            fastTurn(heading, maze[x][y].returnHeading);
            heading = maze[x][y].returnHeading;
        }
        //if (!API::wallLeft()) {
            //API::turnLeft();
            //heading = headingResolution(heading, 'L');
        //}
        //// if there was a wall on the left and there is a wall in front keep turning right
        //while (API::wallFront()) {
            //API::turnRight();
            //heading = headingResolution(heading, 'R');
        //}
        // no wall ahead, move forward
        API::moveForward();

        // updating coordinates
        switch (heading) {
            case 'n':
                y++;
                break;
            case 'e':
                x++;
                break;
            case 's':
                y--;
                break;
            case 'w':
                x--;
                break;
        }
        if (x == 0 && y == 0) {
            log("Returned to start");
            atStart = true;
        }
    } // end while
    return heading;
}

char flipDirection(char heading) {
    switch (heading) {
        case 'n':
            return 's';
        case 'e':
            return 'w';
        case 's':
            return 'n';
        case 'w':
            return 'e';
    }
    return heading;
}