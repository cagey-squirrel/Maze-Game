#include <iostream>
#include <fstream>
using namespace std;


struct Graph {
	int vertices_num = 0;
	int* edges = nullptr;
	int* indices = nullptr;
	int edges_num = 0;
	int edges_capacity = 20;
	bool graph_exists = 0;
	int rows_num;
	int columns_num;
	int enterX, enterY, outX, outY;

};

Graph* graph = new Graph;

void MakeGraph(Graph* graph, int n, int m) {
	graph->rows_num = n;
	graph->columns_num = m;
	graph->vertices_num = graph->rows_num * graph->columns_num;
	graph->graph_exists = 1;


	//Although graph doesnt have edges in the begging, making room for some at the start can reduce need for reallocating when they are added
	graph->edges = new int[graph->edges_capacity];
	graph->indices = new int[graph->vertices_num + 1];



	for (int i = 0; i <= graph->vertices_num; i++)
		graph->indices[i] = 0;



}

void addEdge(Graph* graph, int first, int second) {

	bool parallel_edge = 0; //Prevents addind a parallel edge

	//It is enough to search if there is such an edge for only one vertex
	for (int i = graph->indices[first]; i < graph->indices[first + 1]; i++)
		if (graph->edges[i] == second) { cout << endl << "This is a parallel edge" << endl; parallel_edge = 1; break; }
	if (!parallel_edge) {
		if ((graph->edges_num + 3 >= graph->edges_capacity)) {
			graph->edges_capacity += 10;
			int* edges_t = new int[graph->edges_capacity];
			for (int i = 0; i < graph->edges_num; i++)
				edges_t[i] = graph->edges[i];
			delete[] graph->edges;
			graph->edges = edges_t;
			edges_t = nullptr;
		}


		//Making room for new edge by shifting others to the right
		for (int i = graph->edges_num; i > graph->indices[first + 1]; i--)
			graph->edges[i] = graph->edges[i - 1];
		//New edge is added at the end
		graph->edges[graph->indices[first + 1]] = second;
		//Updating infices
		for (int i = first + 1; i <= graph->vertices_num; i++)
			graph->indices[i] += 1;
		//Updating number of edges
		graph->edges_num += 1;


		//Doing the same for second vertex
		for (int i = graph->edges_num; i > graph->indices[second + 1]; i--)
			graph->edges[i] = graph->edges[i - 1];
		graph->edges[graph->indices[second + 1]] = first;
		for (int i = second + 1; i <= graph->vertices_num; i++)
			graph->indices[i] += 1;
		graph->edges_num += 1;
	}
}



void findPath(Graph* graph) {
	int* SDFU = new int[graph->vertices_num];
	int* PV = new int[graph->vertices_num];
	int* visited = new int[graph->vertices_num]; //Vector of vertices, 0 means unvisited, 1 means visited
	int unvisited_num = graph->vertices_num;

	for (int i = 0; i < graph->vertices_num; i++) {
		SDFU[i] = 1000000;
		visited[i] = 0;
	}

	SDFU[graph->enterX*graph->columns_num + graph->enterY] = 0;
	PV[graph->enterX*graph->columns_num + graph->enterY] = -1; //This marking makes printing the path to the exit easier once algorithm is done

	bool path_found = false;

	//Going while there are unvisited vertices, loop also breaks if exit is found
	while (unvisited_num) {
		//We find min from SDFU (int min is guaranteed to be found as initial value of SDFU[i] is 1000000)
		int min = 1000100;
		int min_index = 0;
		for (int i = 0; i < graph->vertices_num; i++)
			if (SDFU[i] < min and !visited[i]) {
				min = SDFU[i];
				min_index = i;
			}
		// For that min we visit unvisited neighbors and, if shorter path is found, we update SDFU and PV
		for (int i = graph->indices[min_index]; i < graph->indices[min_index + 1]; i++) {
			if (!visited[graph->edges[i]])
				if (SDFU[graph->edges[i]] > SDFU[min_index] + 1) {
					SDFU[graph->edges[i]] = SDFU[min_index] + 1;
					PV[graph->edges[i]] = min_index;
				}
			//Checking if reached vertex is actually exit
			if (graph->edges[i] == graph->outX*graph->columns_num + graph->outY)
				path_found = true;
		}
		visited[min_index] = 1; //Marking visited vertex as such and updating number of visited vertices
		unvisited_num--;
		if (path_found) break;
	}


	//If algorithm finished without founding a way out
	if (!path_found) cout << endl << "Impossible to reach exit " << endl << endl;
	else {
		int i = 0;
		int current = PV[graph->outX*graph->columns_num + graph->outY];
		//Since we need to print a path to the exit in order from enterance to exit, we need to reverse the order of current one,
		//using the visited vector which we no longer need
		while (current > 0) {
			visited[i++] = current;
			current = PV[current]; //Only starting vertex will have negative value
		}


		for (i = i - 1; i >= 0; i--) {
			int k = visited[i] / graph->columns_num; int j = visited[i] % graph->columns_num; //Getting coordinates based of numeration of the vertex
			cout << "(" << k << "," << j << ") ---> ";
		}
		cout << "(" << graph->outX << "," << graph->outY << ")" << endl;
	}


	delete[] visited;
	delete[] SDFU;
	delete[] PV;
}


bool edgeExists(Graph* graph, int first, int second) {
	for (int i = graph->indices[first]; i < graph->indices[first + 1]; i++)
		if (graph->edges[i] == second)
			return true;
	return false;
}


void MazeOutput(Graph* graph) {
	int i, j;
	for (i = 0; i < graph->rows_num - 1; i++) {
		for (j = 0; j < graph->columns_num - 1; j++)
			if ((i == graph->enterX) && (j == graph->enterY)) cout << "O";
			else if ((i == graph->outX) && (j == graph->outY)) cout << "X";
			else {
				if (edgeExists(graph, i*graph->columns_num + j, i*graph->columns_num + j + 1) || edgeExists(graph, (i + 1)*graph->columns_num + j, i*graph->columns_num + j))
					cout << " ";
				else
					cout << "*";
			}
		//Last column (done seperately as its elements dont have a right neighboor)
		if ((i == graph->enterX) && (j == graph->enterY)) cout << "O" << endl;
		else if ((i == graph->outX) && (j == graph->outY)) cout << "X" << endl;
		else {
			if (edgeExists(graph, i*graph->columns_num + j, (i + 1)*graph->columns_num + j) || edgeExists(graph, i*graph->columns_num + j, i*graph->columns_num + j - 1))
				cout << " ";
			else cout << "*";
			cout << endl;
		}

	}
	//Last row (done seperately as its elements dont have a down neighboor)
	for (j = 0; j < graph->columns_num - 1; j++)
		if ((i == graph->enterX) && (j == graph->enterY)) cout << "O";
		else if ((i == graph->outX) && (j == graph->outY)) cout << "X";
		else {
			if (edgeExists(graph, i*graph->columns_num + j, i*graph->columns_num + j + 1) || edgeExists(graph, i*graph->columns_num + j, (i - 1)*graph->columns_num + j))
				cout << " ";
			else cout << "*";
		}
	//Last element (done seperately as it doesnt have down or right neighboor)
	if ((i == graph->enterX) && (j == graph->enterY)) cout << "O" << endl;
	else if ((i == graph->outX) && (j == graph->outY)) cout << "X" << endl;
	else {
		if (edgeExists(graph, i*graph->columns_num + j, i*graph->columns_num + j - 1) || edgeExists(graph, i*graph->columns_num + j, (i - 1)*graph->columns_num + j))
			cout << " ";
		else
			cout << "*";
		cout << endl;
	}


}
void MazeOutputDat(Graph* graph) {
	ofstream Data;
	Data.open("Lavirint.txt");
	int i, j;
	for (i = 0; i < graph->rows_num - 1; i++) {
		for (j = 0; j < graph->columns_num - 1; j++)
			if ((i == graph->enterX) && (j == graph->enterY)) Data << "O";
			else if ((i == graph->outX) && (j == graph->outY)) Data << "X";
			else {
				if (edgeExists(graph, i*graph->columns_num + j, i*graph->columns_num + j + 1) || edgeExists(graph, (i + 1)*graph->columns_num + j, i*graph->columns_num + j))
					Data << " ";
				else
					Data << "*";
			}
		//Last column (same reasoning as above)
		if ((i == graph->enterX) && (j == graph->enterY)) Data << "O" << endl;
		else if ((i == graph->outX) && (j == graph->outY)) Data << "X" << endl;
		else {
			if (edgeExists(graph, i*graph->columns_num + j, (i + 1)*graph->columns_num + j) || edgeExists(graph, i*graph->columns_num + j, i*graph->columns_num + j - 1))
				Data << " ";
			else Data << "*";
			Data << endl;
		}

	}
	//Last column (same reasoning as above)
	for (j = 0; j < graph->columns_num - 1; j++)
		if ((i == graph->enterX) && (j == graph->enterY)) Data << "O";
		else if ((i == graph->outX) && (j == graph->outY)) Data << "X";
		else {
			if (edgeExists(graph, i*graph->columns_num + j, i*graph->columns_num + j + 1) || edgeExists(graph, i*graph->columns_num + j, (i - 1)*graph->columns_num + j))
				Data << " ";
			else Data << "*";
		}
	//Last element (same reasoning as above)
	if ((i == graph->enterX) && (j == graph->enterY)) Data << "O" << endl;
	else if ((i == graph->outX) && (j == graph->outY)) Data << "X" << endl;
	else {
		if (edgeExists(graph, i*graph->columns_num + j, i*graph->columns_num + j - 1) || edgeExists(graph, i*graph->columns_num + j, (i - 1)*graph->columns_num + j))
			Data << " ";
		else
			Data << "*";
		Data << endl;
	}
	Data.close();
}











void deleteGraph(Graph* graph) {
	delete[] graph->indices;
	delete[] graph->edges;
	graph->indices = nullptr;
	graph->edges = nullptr;
	graph->vertices_num = 0;
	graph->edges_num = 0;
	graph->graph_exists = 0;
	graph->rows_num = 0;
	graph->columns_num = 0;
	graph->enterX = graph->enterY = graph->outX = graph->outY = 0;

}


int main() {
	while (true) {
		cout << "Enter a desired option" << endl;
		cout << "1. Create a maze " << endl;
		cout << "2. Add a path to the maze " << endl;
		cout << "3. Print the maze " << endl;
		cout << "4. Print the exit path of the maze " << endl;

		cout << "0. End " << endl;

		int k; cin >> k;

		switch (k) {
		case 1:
			if (graph->graph_exists) cout << endl << "Maze already exists" << endl << endl;
			else {
				while (true) {
					cout << endl << "Enter the number of rows and columns of the maze: ";
					int n, m;
					cin >> n >> m;
					if (n <= 0 || m <= 0)
						cout << endl << "Invalid input od dimensions " << endl;
					else {
						MakeGraph(graph, n, m);
						cout << endl << "Created maze of dimensions:  " << n << " i " << m << endl << endl;
						break;
					}
				}
				while (true) {
					cout << endl << "Enter coordinates of the enterance in the maze ";
					cin >> graph->enterX >> graph->enterY;
					cout << endl << "Enter coordinates of the EXIT in the maze ";
					cin >> graph->outX >> graph->outY;

					if (((graph->enterX != 0 && graph->enterX != graph->rows_num - 1) && (graph->enterY != 0 && graph->enterY != graph->columns_num - 1))
						|| ((graph->outX != 0 && graph->outX != graph->rows_num - 1) && (graph->outY != 0 && graph->outY != graph->columns_num - 1)))
						cout << endl << "Enterance and exit can only be found at the edges od the maze " << endl;
					else break;
				}
			}
			cout << endl;
			break;

		case 2:
			if (graph->graph_exists) {
				int i1, i2, j1, j2;
				while (true) {
					cout << endl << "Enter coordinates of the first vertex  ";

					cin >> i1 >> j1;
					if (i1 < 0 || j1 < 0)
						cout << endl << "Invalid input of coordinates " << endl << endl;
					else  break;
				}
				while (true) {
					cout << "Enter coordinates of second vertex  ";

					cin >> i2 >> j2;
					if (i2 < 0 || j2 < 0)
						cout << endl << "Invalid input of coordinates " << endl << endl;
					else  break;

				}
				addEdge(graph, i1*graph->columns_num + j1, i2*graph->columns_num + j2);
				cout << endl << "Added a path between vertices with coordinates: (" << i1 << ", " << j1 << ") i (" << i2 << ", " << j2 << ")" << endl << endl;
			}
			else cout << endl << "Graph doesn't exist yet " << endl << endl;
			break;

		case 3:
			if (graph->graph_exists) {
				cout << endl << "Whic way you want to print the maze " << endl << "1. Main output" << endl << "2. In a document " << endl;
				int option;
				cin >> option;
				if (option == 1)
					if (graph->columns_num > 80 or graph->rows_num > 50)
						cout << endl << "Size of the maze is too large to be printed on main output " << endl << endl;
					else MazeOutput(graph);

				else if (option == 2)
					MazeOutputDat(graph);

				else
					cout << "Invalid input of option " << endl << endl;
			}
			else cout << endl << "Graph doesn't exist yet" << endl << endl;
			break;

		case 4:
			findPath(graph);
			break;

		case 0:
			deleteGraph(graph);
			exit(1);

		default:
			deleteGraph(graph);
			exit(1);
		}


	}
}
