/* include block for required libraries */
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include "../lib/TMatrix.hpp"
#include "../lib/instance_parser.h"

/* std namespace functions */
using std::cout;
using std::cerr;
using std::cin;
using std::getline;
using std::string;
using std::stringstream;
using std::exception;
using std::ifstream;
using std::ios;
using std::unordered_map;
using std::for_each;
using std::vector;

/* separate the input graph nodes */
vector<int> separateNodes (string nodes)
{
	stringstream stream(nodes);
	string buffer;
	vector <int> result;
	while(getline(stream, buffer, '\t'))
	{
		if (result.size() < 2)
		{
			result.push_back(stoi(buffer));
		}
	};
	if (result.size() == 1)
	{
		result.pop_back();
		stream.clear();
		stream.seekg(0);
		while(getline(stream, buffer, ' '))
		{
			if (result.size() < 2)
			{
				result.push_back(stoi(buffer));
			}
		};

	}
	return result;
}

TMatrix<bool> parseInstance()
{
	cout << "Opening file...\n";
	ifstream input_instance("../etc/instance.txt", ios::in);
	TMatrix<bool> matrix(30000);
	if (input_instance.is_open())
	{
		/* we have to create a k/v pair structure to */
		/* save the equivalences between graph IDs and */
		/* instance IDs as defined in the instance text file */
		/* LEMON provides a NodeMap to assign properties to a node, */
	        /* however checking whether the input file ID belongs in that */
		/* map implies iterating over the whole graph to get the node */
		/* object and search in the map itself, which ends up being */ 
		/* worse than just making this myself, I think */ 
		unordered_map<int, int> instance_graph_ids;
		int node_count = 0;
		string current_line = "";
		bool prev_comment = false;
		cout << "Filling matrix...\n";
		while (getline (input_instance, current_line))
		{
			if (current_line[0] == '#')
			{
				continue;
				//Skip if the line is a comment
			}
			if (current_line[0] == '%')
			{
				prev_comment = true;
				continue;
				/* in matrix market formats, the first line after the comments */
				/* is a count of the graph's number of edges and nodes. we need */
				/* to skip that first line, else we might get buggy behavior */
			}
			if (prev_comment)
			{
				/* matrix.reserve(separateNodes(current_line)[0]); */
				prev_comment = false;
				continue;
			}
			vector <int> separated_nodes = separateNodes(current_line);
			for_each(separated_nodes.begin(), separated_nodes.end(),[&](int& node)
			{
				if (instance_graph_ids.find(node) == instance_graph_ids.end())
				/* we don't have this id in the graph matrix yet */
				/* we need to do this because the numbers of the IDs in the file*/
				/* are kind of  random and might expand further than the length of the graph */
				{
			 		instance_graph_ids[node] = node_count;
					node_count++;
				}
			}
			);
			/* we place the connections in the matrix */
			auto first_node = instance_graph_ids[separated_nodes[0]];
			auto second_node = instance_graph_ids[separated_nodes[1]];
			matrix.set(first_node,second_node, true);
		} 
		instance_graph_ids.clear();
		input_instance.close();
		matrix.setSize(node_count);
		return matrix;
	} 
	else 
	{
		cerr << "Error opening file. Is the file \"instance.txt\" present?\n";
		input_instance.close();
		return matrix;
	}
}
