/*
 * Graph.h
 *
 *  Created on: Oct 8, 2016
 *      Author: Shawn Flynn
 */

#ifndef GRAPH_H_
#define GRAPH_H_

using namespace std;

#include <iostream>
#include <iterator>
#include <fstream>

#include <vector>
#include <list>
#include <string>
#include <limits>

#include "Graph_class.h"


// Constructors

// Default size = 50
template <class T>
Graph<T>::Graph():vertices(50),
				  edges(0),
				  pNeighbors_list(nullptr),
				  reach(nullptr) {

	// Generate the graph columns
	graph.resize(vertices);

	// Insert the graph rows
	fill(graph.begin(), graph.end(), vector<T>(vertices));

}	// end - Graph()

// size = SIZE
template <class T>
Graph<T>::Graph(const int SIZE):vertices(SIZE),
								edges(0),
								pNeighbors_list(nullptr),
								reach(nullptr) {

	// Check for a valid size
	if (SIZE < 2)
		return;

	// Define infinity
	T infinity = numeric_limits<T>::max();

	// Generate the graph columns
	graph.resize(SIZE);

	// Insert the graph rows
	fill(graph.begin(), graph.end(), vector<T>(SIZE, infinity));

}	// end - Graph(size)

// size = i x j
template <class T>
Graph<T>::Graph(int i, int j, string algorithm):vertices(i),
												edges(0),
												pNeighbors_list(nullptr),
												reach(nullptr) {

	// Check for a valid size
	if (i < 2 || j < 2)
		return;

	// Define infinity
	T infinity = numeric_limits<T>::max();

	// Make 1 larger for "Johnson"
	if (!algorithm.compare(sJ))
		vertices++;

	// Generate the graph columns
		APSPgraph.resize(vertices);

	// Initialize to infinity
	for(int i=0; i<vertices; i++)
	{
		APSPgraph[i].resize(vertices);
		for(int j=0; j<vertices; j++)
		{
			// Only need three k planes
			// initial, previous and
			APSPgraph[i][j].resize(3, infinity);
		}
	}

}	// end - Graph(int, int, string)

// from file
template <class T>
Graph<T>::Graph(string file_name):vertices(0),
								  edges(0),
								  pNeighbors_list(nullptr),
								  reach(nullptr) {

	// graph size
	int size;

	// Setup a file stream
	ifstream data_file(file_name);
	if (!data_file) {
		cerr << "Couldn't open file: " << file_name << endl;
		return;
	}

	// Define input stream iterators
	istream_iterator<int> input(data_file);
	istream_iterator<int> eos;

	// Read in the number of vertices
	if (input != eos)
		size = *input++;

	// Check for a valid size
	if (size < 2)
		return;

	// Read in the (unused) number of edges
	if (input != eos)
		*input++;

	// Initialize vertices member
	vertices = size;

	// Generate the graph columns
	graph.resize(size);

	// Insert the graph rows
	fill(graph.begin(), graph.end(), vector<T>(size));

	// Read in and add the edges
	while (input != eos) {

		// Get the graph nodes
		T x = *input++ - 1;
		T y = *input++ - 1;

		// Check validity
		if (x < 0 || x >= size || y < 0 || y >= size) {
			cerr << "Invalid graph node: " << x << ":" << y << endl;
			return;
		}

		// Get the edge value
		T value = *input++;

		// If the edge doesn't exist increment the edge count
		if (graph[x][y] == 0)
			edges++;

		// Set the x->y value
		graph[x][y] = value;
	}

}	// end - Graph(string)

// Destructor

template <class T>
Graph<T>::~Graph()
{
	// Deallocate the neighboring node list
	if (pNeighbors_list != nullptr)
		delete pNeighbors_list;
/*
	// Deallocate the graph
	if (!graph.empty()) {
		// Iterate through the vertices
		for (auto vertex : graph) {
			if (!vertex.empty())
				vertex.clear();
		}
		// Clear the graph
		graph.clear();
	}
*/
}	// end - ~Graph()

// Methods

// Depth-first search utility method
template <class T>
void Graph<T>::dfsearch(int node, int &count) {

	// Set initial node as reachable
	reach[node] = 1;

	// Increment the count
	count++;

	// Look for reachable nodes from here
	for(int j = 1; j < vertices; j++) {
		// If nodes are connected
		if(adjacent(node, j))
		{
			// If not already reached
			if(!reach[j])
				// DFS this node
				dfsearch(j,count);
		}
	}
}

// Return the number of vertices
template <class T>
int Graph<T>::Vertices() {
	return vertices;
}

// Return the number of edges
template <class T>
int Graph<T>::Edges() {
	return edges;
}

// Return true if vertices are adjacent
template <class T>
bool Graph<T>::adjacent(int x, int y) {
	// Check for valid arguments
	if (x < 0 || x >= vertices || y < 0 || y >= vertices)
		return false;
	// Return true if edge exists
	return (graph[x][y] != 0);
}

// Return a list of connected vertices
template <class T>
list<int>* Graph<T>::neighbors(int x) {

	// Check for valid node index
	if (x < 0 || x >= vertices)
		return nullptr;

	// Generate the neighbors list if necessary
	if (pNeighbors_list == nullptr)
		pNeighbors_list = new list<int>;
	else
		// Clear the list
		pNeighbors_list->clear();

	// Add neighbors to the list
	for (auto &y : graph[x]) {
		if (y != 0) {
			pNeighbors_list->push_back(y);
		}
	}

	// return the list
	return pNeighbors_list;
}

// Add the specified x->y edge
template <class T>
void Graph<T>::add(int x, int y) {

	// Check for valid arguments;
	if (x < 0 || x >= vertices || y < 0 || y >= vertices)
		return;

	// Set the x->y value to 1
	graph[x][y] =  1;

	// Increment the number of edges
	edges++;
}

// Remove the specified x->y edge
template <class T>
void Graph<T>::remove(int x, int y) {

	// Check for valid arguments;
	if (x < 0 || x >= vertices || y < 0 || y >= vertices)
		return;

	// If the edge exists decrement the edge count
	if (graph[x][y] != 0)
		edges--;

	// Set the x->y value to 0
	graph[x][y] = 0;
}

// Return the edge value
template <class T>
T Graph<T>::get_edge_value(int x, int y) {

	// Check for valid arguments;
	if (x < 0 || x >= vertices || y < 0 || y >= vertices)
		return 0;

	// Return the x->y edge value
	return graph[x][y];
}

// Set the edge value
template <class T>
void Graph<T>::set_edge_value(int x, int y, T a) {

	// Check for valid arguments;
	if (x < 0 || x >= vertices || y < 0 || y >= vertices)
		return;

	// If the edge doesn't exist increment the edge count
	if (graph[x][y] == 0)
		edges++;

	// Set the x->y value
	graph[x][y] = a;
}

// Return the APSP edge value
template <class T>
T Graph<T>::get_APSP_edge_value(int i, int j, int k) {

	// Check for valid arguments;
	if (i < 0 || i >= vertices ||
		j < 0 || j >= vertices ||
		k < 0 ||k >= 3)
		return 0;

	// Return the x->y edge value from the k plane
	return APSPgraph[i][j][k];
}

// Set the APSP edge value
template <class T>
void Graph<T>::set_APSP_edge_value(int i, int j, int k, T dist) {

	// Check for valid arguments;
	if (i < 0 || i >= vertices ||
		j < 0 || j >= vertices ||
		k < 0 || k >= 3)
		return;

	// Define infinity
	long infinity = numeric_limits<long>::max();

	// If the edge doesn't exist increment the edge count
	if (APSPgraph[i][j][k] == infinity)
		edges++;

	// Set the x->y value
	APSPgraph[i][j][k] = dist;
}

// Get graph density
template <class T>
double Graph<T>::get_density() {

	// Return the graph density
	return (edges * 1.0) / (vertices * vertices);

}

// is_connected() method
template <class T>
bool Graph<T>::is_connected(const int SIZE) {

	// Initialize the reach array
	reach = new int[SIZE]{};

	// Initialize the count
	int count = 0;

	// Depth-first search of the graph
	dfsearch(0, count);

	// Deallocate the reach array
	delete [] reach;

	// Check result
	if(count == vertices)
		return true;
	else
		return false;
}

template <class T>
void Graph<T>::print_matrix(style_t style) {

	const int cout_width = 7;

	// Define infinity
	T infinity = numeric_limits<T>::max();

	// Output the title
	cout << "Adjacency Matrix:" << endl << endl;

	// Output top header line
	for (int i=0; i<cout_width; i++)
		cout << " ";

	int i = 0;
	for (auto x : graph) {
		cout.width(cout_width);
		if (style == NUMERIC)
			cout << i++;
		if (style == LOWERCASE)
			cout << static_cast<char>(97 + i++);
		if (style == UPPERCASE)
			cout << static_cast<char>(65 + i++);
	}
	cout << endl;
	// Output the graph rows
	i = 0;
	for (auto &x : graph){
		cout.width(cout_width);
		// Output vertex index
		if (style == NUMERIC)
			cout << i++;
		if (style == LOWERCASE)
			cout << static_cast<char>(97 + i++);
		if (style == UPPERCASE)
			cout << static_cast<char>(65 + i++);
		// Output vertex values
		for (auto &y : x) {
			cout.width(cout_width);
			if (y == infinity)
				cout << " ";
			else
			    cout << y;
		}
		cout << endl;
	}
	cout << endl;

}	// end - print_matrix()

// Print the Edge List
template <class T>
void Graph<T>::print_edge_list(style_t style) {

	// Output the title
	cout << "Edge List:" << endl << endl;

	// Output the rows
	int i = 0;
	for (auto &x : graph) {
		if (style == NUMERIC)
			cout << " " << i++;
		if (style == LOWERCASE)
			cout << " " << static_cast<char>(97 + i++);
		if (style == UPPERCASE)
			cout << " " << static_cast<char>(65 + i++);
		// Output the columns
		int j = 0;
		for (auto &y : x) {
			if (y != 0) {
				if (style == NUMERIC)
					cout << " -> " << j << ":" << y;
				if (style == LOWERCASE)
					cout << " -> "  << static_cast<char>(97 + j) << ":" << y;
				if (style == UPPERCASE)
					cout << " -> "  << static_cast<char>(65 + j) << ":" << y;
			}
			j++;
		}
		cout << endl;
	}
	cout << endl;

}	// end - print_edge_list()

// Print the graph statistics
template <class T>
void Graph<T>::print_stats() {

	cout << endl;

	// Print the number of vertices
	cout << "vertices = " << vertices << endl;

	// Print the number of edges
	cout << "edges = " << edges << endl;

	// Print the graph density
	cout << "density = " << get_density() << endl;

	cout << endl;

}	// end - print_stats()

// Print all output
template <class T>
void Graph<T>::print_graph(style_t style) {

	// Print graph statistics
	print_stats();

	// Print the Adjacency matrix
	print_matrix(style);

	// Print the edge list
	print_edge_list(style);

}	// end - print_graph()

#endif /* GRAPH_H_ */
