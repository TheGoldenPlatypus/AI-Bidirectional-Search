#include "glut.h" // open gl library for windows GUI
#include <time.h>
#include <vector>
#include "Cell.h"
#include <iostream>

using namespace std;

/*
 ___________________________________
| _____ |   | ___ | ___ ___ | |   | |
| |   | |_| |__ | |_| __|____ | | | |
| | | |_________|__ |______ |___|_| |
| |_|   | _______ |______ |  | ___ _|
| ___ | |____ | |______ | |_| |____ |
|___|_|____ | |   ___ | |________ | |
|   ________| | |__ | |______ | | | |
| | | ________| | __|____ | | | __| |
|_| |__ |   | __|__ | ____| | |_| __|
|   ____| | |____ | |__ |   |__ |__ |
| |_______|_______|___|___|___|_____|

*/

const int MSZ = 100; // maze size

const int WALL = 0;
const int SPACE = 1;
const int START = 2;
const int TARGET = 3;
const int BLACK = 4;
const int GRAY = 5;
const int PATH = 6;

int maze[MSZ][MSZ] = {0};
vector<Cell*> grays_start; // list of grey vertexes
vector<Cell*> grays_end; // list of grey vertexes
bool biStart = false; // for idle function

void initMaze();


// Searches for a specific cell in the Cells queue and if exists return it
Cell* getCell(int row, int col, vector <Cell*>* grays)\
{
	for (Cell* cell : *grays)
	{
		if (cell->getRow() == row && cell->getCol() == col) { return cell; }
	}
	return nullptr;
}

void restorePath(Cell* ps) {
	
	Cell* targetCell;
	Cell* intersectCell = ps;
	while (ps->getParent() != nullptr)
	{
		maze[ps->getRow()][ps->getCol()] = PATH;
		ps = ps->getParent();
	}

	if (ps->getRow() == (MSZ / 2) && ps->getCol() == (MSZ / 2)) // If the current cell is the START cell
	{
		ps = getCell(intersectCell->getRow(), intersectCell->getCol(), &grays_end);
	}
	else
	{
		ps = getCell(intersectCell->getRow(), intersectCell->getCol(), &grays_start);
	}

	if (ps)
	{
		while (ps->getParent() != nullptr)
		{
			maze[ps->getRow()][ps->getCol()] = PATH;
			ps = ps->getParent();
		}
	}
}

int isIntersected(int row, int col, vector <Cell*>* grays)
{
	for (Cell* pcurrent : *grays)
	{
		if (pcurrent->getRow() == row && pcurrent->getCol() == col) { return 1; } // The cell already exists in the grays queue (not an intersection)
	}
	return 0;
}

void checkNeighbor(Cell* pCurrent, int row, int col, vector <Cell*>* grays) { // gets te pointer to current cell and the coordinates of its neighbor 

	Cell* nextCell;
	// check if the neigbor is not the target
	if (maze[row][col] == GRAY) {

		// there is  intesection
		if (isIntersected(row, col,grays) == 0)
		{
			biStart = false;
			nextCell = new Cell(row, col, pCurrent);
			restorePath(nextCell);
		}
	}
	else if (maze[row][col] == SPACE)
	{
			nextCell = new Cell(row, col, pCurrent);
			maze[row][col] = GRAY;
			grays->push_back(nextCell);
	}
	
	
}

void BFSIteration(vector <Cell*>* grays) {

	Cell* pCurrent;
	int row, col; // current row&col

	//check the grays queue
	if (grays->empty()) {

		biStart = false;
		cout << "There is no solution\n";
	}
	else {

		// pick and remove from the queue the first cell and check its neighbors
		pCurrent = *(grays->begin());
		grays->erase(grays->begin());
		//paint it black
		row = pCurrent->getRow();
		col = pCurrent->getCol();

		// scan all the (white) neighbors or target vertex and add them to queue (if it's not a target)
		
		if (maze[row][col] != START && maze[row][col] != TARGET)
			maze[row][col] = BLACK;

		 // check UP
			checkNeighbor(pCurrent, row + 1, col,grays);

		if (biStart) // check DOWN
			checkNeighbor(pCurrent, row - 1, col, grays);

		if (biStart) // check LEFT
			checkNeighbor(pCurrent, row, col - 1, grays);

		if (biStart) // check RIGHT
			checkNeighbor(pCurrent, row, col + 1, grays);

	}

}

void BidirectionalBFS()
{
	if (grays_start.empty() && grays_end.empty())
	{
		biStart = false;
	}
	else
	{
		// start BFS from START to TARGET and vice versa by sending pointers to the corresponding queues 
		BFSIteration(&grays_start);
		BFSIteration(&grays_end);
	}
}

void initMaze() {

	int i, j;

	for (int i = 1; i < MSZ - 1; i++)
	{
		for (int j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 0) { // even rows - mainly walls

				if (rand() % 100 > 35)
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;

			}
			else { 	// odd - mainly spaces

				if (rand() % 100 > 20)
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
		}

	}
	maze[MSZ / 2][MSZ / 2] = START;
	Cell* ps = new Cell(MSZ / 2, MSZ / 2, nullptr); // start vertex
	grays_start.push_back(ps); // add start vertex to end of the vector
	int x, y;
	x = rand() % MSZ;
	y = rand() % MSZ;
	maze[x][y] = TARGET;
	Cell* pt = new Cell(x, y, nullptr); // end vertex
	grays_end.push_back(pt); // add start vertex to end of the vector

}

void showMaze() {

	int i, j;

	for (i = 0; i < MSZ; i++)
	{
		for (j = 0; j < MSZ; j++)
		{
			// set color for cell [i][j]
			switch (maze[i][j]) {

			case WALL:
				glColor3d(0.5, 0, 0); // green
				break;

			case SPACE:
				glColor3d(1, 1, 1); // black
				break;

			case START:
				glColor3d(0.5, 1, 1); // yellow
				break;

			case TARGET:
				glColor3d(1, 0, 0); // red
				break;

			case BLACK:
				glColor3d(1, 1, 0); // yellow
				break;

			case GRAY:
				glColor3d(1, 0, 1); // purple
				break;

			case PATH:
				glColor3d(0, 0.7, 0); // orange
				break;
			}
			/*
			NOTE:
			  i+1,j  _______  i+1,j+1
					|       |
					|       |
					|       |
				i,j	|_______| i,j+1

			*/
			// display cell as polygon

			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i + 1);
			glVertex2d(j + 1, i + 1);
			glVertex2d(j + 1, i);
			glEnd();


		}

	}
}

void menu(int choice)
{
	if (choice == 0) //Bidirectional
	{
		biStart = true;
	}
}

void init()
{	//params: red,green,blue,alpha(color clarity)
	glClearColor(0, 0, 0.4, 0);// color of window background

	// set the main axes
	glOrtho(0,MSZ, 0, MSZ,-1,1); // no depth  - maze is 2D

	srand(time(0)); // random seed

	initMaze();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer OF previous images

	// drawings logic -  all we want to display
	
	showMaze();

	glutSwapBuffers(); // show all
}

void idle()
{
	if (biStart)
	{
		BidirectionalBFS();
		
		
	}
		
	glutPostRedisplay(); // indirect call to refresh function
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv); // initialize 
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // set the window to corespond with RGB & double buffer to manage 2 and more images (for animation)
	glutInitWindowSize(600, 600); // set window scale
	glutInitWindowPosition(200, 100); // set window position on the screen
	glutCreateWindow("Unimformed Search - BFS Example"); // set title to the window

	glutDisplayFunc(display); // display function call to refresh the window 
	glutIdleFunc(idle); // running all the time when nothing happening

	glutCreateMenu(menu); // menu creation 
	glutAddMenuEntry("Run BD BFS",0); // if this option selected -->  it will sends 0
	glutAttachMenu(GLUT_RIGHT_BUTTON); // right buuton of mouse


	init(); // call to set the background colors
	 
	glutMainLoop(); // starts window queue of the events
}