#include "GraphGUI.h"
#include "Algorithms.h"
#include <iostream>
#include <fstream>

std::vector<std::pair<std::array<unsigned, 2>, int>> input_form_list(std::istream& stream) {
	// Format: <v> <v> <w>\n
	std::vector<std::pair<std::array<unsigned, 2>, int>> edges_list;
	std::string row;
	while (std::getline(stream, row)) {
		std::cout << row + '\n';
		std::pair<std::array<unsigned, 2>, int> connection{};
		for (size_t i = 0, k = 0; i < row.length(); k++) {
			const size_t j = i;
			while (std::isdigit(row[i]) && i < row.length()) i++;
			switch (k) {
			case 0:
				connection.first[0] = std::stoi(row.substr(j, i - j)); break;
			case 1:
				connection.first[1] = std::stoi(row.substr(j, i - j)); break;
			case 2:
				connection.second = std::stoi(row.substr(j, i - j)); break;
			default:throw std::invalid_argument("Invalid input");
			}
			while (!std::isdigit(row[i]) && i < row.length()) i++;
		}
		edges_list.push_back(connection);
	}

	return edges_list;
}

std::vector<std::vector<unsigned int>> input_form_matrix(std::istream& stream) {
	std::vector<std::vector<unsigned int>> matrix;
	std::string row;
	while (std::getline(stream, row)) {
		std::cout << row + '\n';
		std::vector<unsigned int> weights;
		for (size_t i = 0; i < row.length(); i++) {
			const size_t j = i;
			while (std::isdigit(row[i]) && i < row.length()) i++;
			weights.push_back(std::stoi(row.substr(j, i - j)));
		}
		matrix.push_back(weights);
	}
	if (matrix[0].size() != matrix.size())
		throw std::invalid_argument("Invalid input");
	return matrix;
}

int main() {
	std::string input = "input.txt";
	std::string gen_input = "generated_input.txt";
	std::ifstream file(gen_input);
	GraphGUI* graph;
	sf::RenderWindow window(sf::VideoMode(1024, 720), "Graph");
	try {
		std::string line;
		std::getline(file, line);
		int input_format = std::stoi(line);
		switch (input_format) {
		case 1: {
			graph = new GraphGUI(window, input_form_list(file));
			break;
		}
		case 2: {
			graph = new GraphGUI(window, input_form_matrix(file));
			break;
		}
		default: throw std::invalid_argument("Invalid input");
		}
	}
	catch (std::invalid_argument& err) {
		std::cerr << err.what() << std::endl;
		return -1;
	}
	graph->build_graph(window);
	std::cout << "Graph built\n";
	auto config = graph->get_config();
	//std::copy(config.first.begin(), config.first.end(), std::ostream_iterator<unsigned>(std::cout, " "));
	//std::cout << "\n";
	while (window.isOpen()) {
		unsigned s = 0, f = 0;
		Algorithms algo(config);
		std::cout << "Enter start and finish of a path to find\n";
		std::cin >> s >> f;
		auto path = algo.BFS(s, f);
		//std::copy(path.begin(), path.end(), std::ostream_iterator<unsigned>(std::cout, " "));
		//std::cout << "\n";
		graph->enlighten_path(window, path);
		graph->reset_color();
	}
}
