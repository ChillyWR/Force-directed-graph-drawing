#include <fstream>
#include <algorithm>
#include <vector>
#include <array>
#include <random>

inline unsigned get_random(unsigned min, unsigned max) {
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_int_distribution<unsigned> distribution(min, max);
	return distribution(gen);
}

void generate_adjacency_matrix(std::ostream& file, const unsigned& amount) {
	std::vector<std::vector<unsigned>> matrix(amount, std::vector<unsigned>(amount));
	for (unsigned i = 0; i < amount; ++i) {
		for (unsigned j = 0; j < i; ++j) {
			matrix[i][j] = matrix[j][i];
			file << matrix[i][j] << ' ';
		}
		matrix[i][i] = 0;
		file << 0 << ' ';
		for (unsigned j = i + 1; j < amount; ++j) {
			matrix[i][j] = get_random(0, 1);
			file << matrix[i][j] << ' ';
		}
		file << '\n';
	}
}

std::vector<std::array<unsigned, 2>> edges_list(unsigned amount) {
	const auto max_pairs = amount * (amount - 1) / 2;
	std::vector<std::array<unsigned, 2>> all_possible_edges(max_pairs);
	std::vector<unsigned> indices(max_pairs);
	for (size_t i = 0, k = 0; i < amount; ++i) {
		for (size_t j = i + 1; j < amount; ++j, ++k) {
			all_possible_edges[k][0] = i;
			all_possible_edges[k][1] = j;
			indices[k] = k;
		}
	}
	std::shuffle(indices.begin(), indices.end(), std::default_random_engine(std::random_device()()));
	std::vector<std::array<unsigned, 2>> edges(amount);
	for (size_t i = 0; i < amount; ++i)
		edges[i] = all_possible_edges[indices[i]];
	return edges;
}

void edges_list_output(std::ostream& file, const std::vector<std::array<unsigned, 2>>& edges) {
	for (const auto& edge : edges) {
		file << edge[0] << ' ' << edge[1] << ' ' << '1' << '\n';
	}
}

int main(int argc, char* argv[]) {
	// g++ input_generator.cpp -o input_generator
	// .\input_generator.exe 1 100
	constexpr char input_filename[] = "generated_input.txt";
	constexpr unsigned default_amount = 25;
	std::ofstream file(input_filename);
	switch (argc) {
	case 1: {
		file << 1 << '\n';
		edges_list_output(file, edges_list(default_amount)); break;
	}
	case 2: {
		const char mode = argv[1][0];
		file << mode << '\n';
		switch (mode) {
		case '1':
			edges_list_output(file, edges_list(default_amount)); break;
		case '2':
			generate_adjacency_matrix(file, default_amount); 
			break;
		default: exit(-1);
		}
		break;
	}
	case 3: {
		const char mode = argv[1][0];
		const unsigned amount = std::strtol(argv[2], nullptr, 10);
		file << mode << '\n';
		switch (mode) {
		case '1':
			edges_list_output(file, edges_list(amount)); break;
		case '2':
			generate_adjacency_matrix(file, amount); 
			break;
		default: exit(-1);
		}
		break;
	}
	default: exit(-1);
	}
	return 0;
}
