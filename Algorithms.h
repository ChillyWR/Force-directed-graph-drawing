#pragma once
#include <queue>
#include <vector>
#include <array>
#include <stdexcept>


struct Algorithms {
	const std::vector<unsigned> vertices;
	const std::vector<std::array<unsigned, 2>> edges;
	std::vector<std::array<unsigned, 2>> queue;
	std::vector<bool> tags;

	Algorithms(const std::pair<std::vector<unsigned>, const std::vector<std::array<unsigned, 2>>>&);
	ptrdiff_t get_vertex_index(const unsigned& vertex) const;

	std::vector<unsigned> BFS(const unsigned&, const unsigned&);
	//std::vector<unsigned> DFS(const unsigned&, const unsigned&);
	//unsigned next_vertex(unsigned);
	std::vector< std::array<unsigned, 2>>::const_iterator find_any_edge(const std::vector<std::array<unsigned, 2>>&, int = 0) const;
	std::vector<unsigned> get_vertex_path();

};
