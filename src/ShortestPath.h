/*
 * ShortestPath.h
 *
 *  Created on: Oct 8, 2016
 *      Author: Shawn Flynn
 */

#ifndef SHORTESTPATH_H_
#define SHORTESTPATH_H_

#include "ShortestPath_class.h"


// Constructors

// From specified filename
template <class T>
ShortestPath<T>::ShortestPath(string filename): shortest_path_distance(0),
												graph_size(0),
												pPath_list(nullptr),
												P(nullptr) {

	// Setup a file stream
	ifstream data_file(filename);
	if (!data_file) {
		cerr << "Couldn't open file: " << filename << endl;
		return;
	}

	// Define input stream iterators
	istream_iterator<int> input(data_file);
	istream_iterator<int> eos;

	// Read in the number of nodes
	if (input != eos)
		graph_size = *input++;

	// Read in the number of edges
	if (input != eos)
		*input++;

	// Check for a valid size
	if (graph_size < 2)
		return;

	// Generate and initialize a 2-D graph class
	SSSPgraph = Graph<int>(graph_size);

	// Set all existing edges from the file
	while ( input != eos ) {

		// Read the edge values
		int tail = *input++;
		int head = *input++;
		long dist = *input++;

		// Convert to zero-based graph
		SSSPgraph.set_edge_value(tail-1, head-1, dist);
	}

}	// end - ShortestPath(string)

// Constructor for All-Pairs Shortest Path class
template <class T>
ShortestPath<T>::ShortestPath(string filename, string algorithm):
													shortest_path_distance(0),
													graph_size(0),
													pPath_list(nullptr),
													P(nullptr) {

	// Setup a file stream
	ifstream data_file(filename);
	if (!data_file) {
		cerr << "Couldn't open file: " << filename << endl;
		return;
	}

	// Define input stream iterators
	istream_iterator<int> input(data_file);
	istream_iterator<int> eos;

	// Read in the number of nodes
	if (input != eos)
		graph_size = *input++;

	// Read in the number of edges
	if (input != eos)
		*input++;

	// Check for a valid size
	if (graph_size < 2)
		return;

	// Generate and initialize a 3-D graph class
	APSPgraph = Graph<T>(graph_size, graph_size, algorithm);

	// Update the graph size
	graph_size = APSPgraph.Vertices();

	// Set all i to i node distances to 0
	for (int k=0; k<3; k++) {
		for (int i=0; i<graph_size; i++) {
			APSPgraph.set_APSP_edge_value(i, i, k, (T) 0);
		}
	}

	// Set index adjustment
	int adj = 0;		// Johnson
	if (!algorithm.compare(sFW))
		adj++;			// Floyd-Warshall

	// Set all existing edges from the file
	while ( input != eos ) {

		// Read the edge values
		int tail = *input++;
		int head = *input++;
		T dist = *input++;

		// Put into each of the k graphs
		for (int k=0; k<3; k++)
			APSPgraph.set_APSP_edge_value(tail-adj, head-adj, k, dist);
	}

}	// end - ShortestPath(filename, algorithm)


// ShortestPath class destructor
template <class T>
ShortestPath<T>::~ShortestPath(void) {
	cout << "Deleting ShortestPath class" << endl;
}

// Get the graph size
template <class T>
int ShortestPath<T>::getGraphSize() {
	return graph_size;
}

// Get a graph entry
template <class T>
T ShortestPath<T>::GetGraphEntry(int i, int j, int k) {

	// Define infinity
	long infinity = numeric_limits<long>::max();

	// Check for valid i
	if (i<0 || i>=graph_size)
		return infinity;

	// Check for valid j
	if (j<0 || j>=graph_size)
		return infinity;

	// Check for valid k
	if (k<0 || k>=graph_size)
		return infinity;

	// Return the graph entry
	return APSPgraph.get_APSP_edge_value(i, j, k);

}	// end - GetGraphEntry()

// Get the shortest path distance
template <class T>
T ShortestPath<T>::getShortestPathDistance() {
	return shortest_path_distance;
}

// Dijkstra shortest-path algorithm
template <class T>
void ShortestPath<T>::getShortestPath(Graph<T> graph,
									 const int start,
									 const int end) {

	const bool DEBUG = false;

	// Define infinity
	int MAX_DIST = numeric_limits<int>::max();

	// Check for same node
	if (start == end) {
		shortest_path_distance = 0;
		return;
	}

	// Calculate the graph size
	int size = graph.Vertices();

	// Define and initialize the local lists
	vector<bool> visited(size, false);
	vector<int> node_dist(size, MAX_DIST);

	// Generate a local path node vector
	vector<int>* pNode_list = new vector<int>(size, -1);

	// Set the start node distance to 0
	node_dist[start] = 0;

	// Loop counter
	int count = 0;

	// Loop through the vertices
	while(count < size) {

		int minDistance = MAX_DIST;
		int closestNode = 0;

		// Find the closest adjacent node
		for(int i = 0; i < size; i++) {
			if((!visited[i]) && (minDistance >= node_dist[i])) {
				minDistance = node_dist[i];
				closestNode = i;
			}
		}

		// Indicate this node has been visited
		visited[closestNode] = true;

		if (DEBUG) {
			cout << "count = " << count << endl;
			cout << "closestNode = " << closestNode << endl;
			cout << "minDistance = " << minDistance << endl;
		}

		// Loop through the nodes
		for(int i = 0; i < size; i++) {

			// Get the distance to this node from the start node
			int cur_distance;

			// Check for MAX_DIST
			if (node_dist[closestNode] == MAX_DIST)
				cur_distance = MAX_DIST;
			else
				cur_distance = node_dist[closestNode] +
										graph.get_edge_value(closestNode, i);

			// Adjust current distance
			if (P != nullptr)
				if (cur_distance != MAX_DIST)
					cur_distance += P[closestNode] - P[i];

			// If the node has not been visited and the distance is not 0
			if (!visited[i] && (graph.get_edge_value(closestNode, i) != 0) ) {

				// If this node is closer
				if (node_dist[i] > cur_distance) {

					// Put the current distance into the list
					node_dist[i] = cur_distance;

					// Put the current node into the path list
					pNode_list->at(i) = closestNode;
				}
			}
		}

		// Move to the next node
		count++;
	}

	// Set the shortest path distance
	shortest_path_distance = node_dist[end];
	if (shortest_path_distance == MAX_DIST)
		shortest_path_distance = 0;
	else {
		shortest_path_distance -= node_dist[start];
		// Adjust for "Johnson" algorithm
		shortest_path_distance += -P[start] + P[end];
	}

 	// Don't create a shortest path for "Johnson"
 	if (P == nullptr) {
 		// Create the shortest path list
 		createPath_list();

 		// Generate the path node list - for Dijkstra
 		genPathNodeList(*pNode_list, end);
 	}

	// Test output
	if (DEBUG) {
		cout << "The distances to the other nodes are:" << endl;
		for(int i = start; i <= end; i++) {
			cout << i << " : " << node_dist[i] << endl;
		}
		cout << endl;
		cout << "The path node list is:" << endl;
		for (auto i : *pNode_list) {
			if (i == 0)
				cout << i;
			else
				cout << " -> " << i;
		}
		cout << endl;
	}

}	// end - getShortestPath()

// Floyd-Warshall shortest path algorithm
template <class T>
string ShortestPath<T>::getAPSPShortestPath(string algorithm) {

	// Define infinity
	long infinity = numeric_limits<long>::max();

	// Define the current distance
	long distance;

	// Upper limit for i
	int i_size = graph_size;

	// Set upper limit on i
	// Only the first node for "Johnson" algorithm
	if (algorithm.compare(sJ)==0)
		i_size = 1;

	// Loop through all of the subnode sets
	for (int k=1; k<graph_size; k++) {
		// Set an early exit flag
		bool early_exit = true;

		// Loop all of the specified nodes
		for (int i=0; i<i_size; i++) {
			// Loop through all of the nodes
			for (int j=0; j<graph_size; j++) {

				// Local values
				long dist1 = APSPgraph.get_APSP_edge_value(i, k, prev);
				long dist2 = APSPgraph.get_APSP_edge_value(k, j, prev);

				// Get current value
				if ((dist1 == infinity) ||
					(dist2 == infinity))
					distance = infinity;
				else
					distance = dist1 + dist2;

				// Get the inherited value
				long dist3 = APSPgraph.get_APSP_edge_value(i, j, prev);

				// Compare the values
				if (dist3 < distance) {

					// Store the inherited distance
					APSPgraph.set_APSP_edge_value(i, j, curr, dist3);
				} else {

					// Store the new value
					APSPgraph.set_APSP_edge_value(i, j, curr, distance);

					// Check for negative-cost cycle
					if (i == j && distance < 0)
						return "NULL";

					// Clear the early exit flag
					early_exit = false;
				}
			}	// for i
		}	// for j

		// Copy current k to previous k
		// and initialize current k from initial k
		for(int i=0; i<graph_size; i++)
			for (int j=0; j<graph_size; j++) {
				// Current k to previous k
				long distance = APSPgraph.get_APSP_edge_value(i, j, curr);
				APSPgraph.set_APSP_edge_value(i, j, prev, distance);
				// Initial k to current k
				distance = APSPgraph.get_APSP_edge_value(i, j, init);
				APSPgraph.set_APSP_edge_value(i, j, curr, distance);
			}

		// Check for early exit
		if (early_exit)
			return "OK";

	}	// for k

	// Return result
	return "OK";

}	// end getAPSPShortestPath()

// Johnson APSP algorithm
template <class T>
string ShortestPath<T>::getJShortestPath(string filename) {

	// Set the adjustment list size
	int P_size = graph_size - 1;

	// Create the node adjustment list
	P = new int[P_size]();

	// Create node 0 edges to all other vertices
	for (int i=0; i<graph_size; i++) {
		// Set on all k planes
		APSPgraph.set_APSP_edge_value(0, i, 0, 0);
		APSPgraph.set_APSP_edge_value(0, i, 1, 0);
		APSPgraph.set_APSP_edge_value(0, i, 2, 0);
	}

	// Run Floyd-Warshall to get reweighting values
	string result = getAPSPShortestPath(sJ);

	// Check the results for a negative cycle
	if (!result.compare("NULL"))
		return "NULL";

	// Populate the node adjustment list
	for (int i=1; i<P_size; i++)
		P[i-1] = APSPgraph.get_APSP_edge_value(0, i, 1);

	// Create a 2-D graph class
	SSSPgraph = Graph<int>(filename);

	// Update the graph size
	graph_size = SSSPgraph.Vertices();

	// Initialize shortest path
	int shortest_path = numeric_limits<int>::max();

	// Calculate all shortest paths
	for (int i=0; i<graph_size; i++) {
		for (int j=0; j<graph_size; j++) {

			// Call Dijkstra
			getShortestPath(SSSPgraph, i, j);

			// Check for minimum shortest path
			if (shortest_path_distance < shortest_path)
				shortest_path = shortest_path_distance;
		}
	}

	// Set the shortest path distance
	shortest_path_distance = shortest_path;

	// Return OK
	return "OK";

}	// end - getJShortestPath()

// Get the All-Pairs Shortest Path path
template <class T>
T ShortestPath<T>::getAPSPShortestShortestPath() {

	// Local shortest-shortest-path variable
	long ssp = numeric_limits<long>::max();

	// Find the shortest-shortest path
	for (int i=0; i<graph_size; i++)
		for (int j=0; j<graph_size; j++) {

			// Current shortest path value
			long dist = APSPgraph.get_APSP_edge_value(i, j, 1);

			// Check if smaller
			if (dist < ssp)
				ssp = dist;
		}

	// Return the shortest-shortest path distance
	return ssp;

}	// end - GetAPSPShortestShortestPath()

// Create a path node list
template <class T>
void ShortestPath<T>::createPath_list() {

	// Generate if necessary
	if (pPath_list == NULL)
		pPath_list = new list<int>;
	// Else clear it
	else
		pPath_list->clear();

}

// Generate the path node list
template <class T>
void ShortestPath<T>::genPathNodeList(vector<T>& path_list, int node) {

	// Check for a valid path node list
	if (pPath_list == NULL) {
		cout << "No path node list" << endl;
		return;
	}

	// At starting node
	if(node == 0) {
		pPath_list->push_back(0);
	}
	// No path to this node
	else if(path_list.at(node) == -1)
		return;
	// Recurse to next node
	else {
		genPathNodeList(path_list, path_list.at(node));
		// Put the node into path node list
		pPath_list->push_back(node);
	}

}	// end - genPathNodeList()

// Print the shortest path node list and distance
template <class T>
void ShortestPath<T>::print_ShortestPath(Graph<T> graph) {

	// Check for a valid path node list
	if (pPath_list == NULL) {
		cout << "No path node list" << endl;
		return;
	}

	// Print the header
	cout << "Shortest Path:" << endl;

	// Check for a valid list
	if (pPath_list == NULL) {
		cout << "No path node list" << endl;
		return;
	}

	// Output the nodes
	bool firstNode = true;
	for (auto node : *pPath_list) {
		if (firstNode) {
			cout << node;
			firstNode = false;
		} else {
			cout << " -> " << node;
		}
	}

	// Print the distance
	cout << " = " << shortest_path_distance << endl << endl;

}	// end - printPath()

// Print All-Pairs Shortest Path
template <class T>
void ShortestPath<T>::print_APSPgraph(int k) {

	// Define infinity
	long infinity = numeric_limits<long>::max();

	// Print the k plane
	cout << "k = " << k << endl;

	// Print the graph
	for (int i=graph_size-1; i>=0; i--) {
		for (int j=0; j<graph_size; j++) {

			// Print the graph
			long entry = GetGraphEntry(i, j, k);
			if (entry == infinity)
				cout << "  ";
			else
				cout << GetGraphEntry(i, j, k) << " ";
		}
		// Print the line ending
		cout << endl;
	}
	// Print a divider
	cout << endl;

}


#endif /* SHORTESTPATH_H_ */
