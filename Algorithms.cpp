#include "Algorithms.h"

#include <iostream>

Algorithms::Algorithms(const std::pair<std::vector<unsigned>, const std::vector<std::array<unsigned, 2>>>& graph_signature)
	: vertices(graph_signature.first), edges(graph_signature.second), tags(graph_signature.first.size(), false) {}

ptrdiff_t Algorithms::get_vertex_index(const unsigned& vertex) const {
	return std::distance(vertices.begin(), std::find(vertices.begin(), vertices.end(), vertex));
}

std::vector<unsigned> Algorithms::BFS(const unsigned& start, const unsigned& finish) {
	const unsigned start_index = get_vertex_index(start);
	const unsigned finish_index = get_vertex_index(finish);
	if (start_index == vertices.size()) throw std::invalid_argument("BFS: start vertex not fount in graph");
	if (finish_index == vertices.size()) throw std::invalid_argument("BFS: finish vertex not fount in graph");

	tags[start_index] = true;
	queue.push_back({ 0, start });
	for (int i = 0; i < static_cast<int>(vertices.size()); ++i) {
		const unsigned new_counter = queue[i][0] + 1;
		const unsigned current_vertex = queue[i][1];
		if (current_vertex == finish) {
			queue.erase(queue.begin() + i + 1, queue.end());
			for (int j = i - 1; queue[j][0] == new_counter - 1; --j) {
				queue.erase(queue.begin() + j);
			}
			return get_vertex_path();
		}
		for (const auto& edge : edges) {
			unsigned connected_vertex = 0;
			if (edge[0] == current_vertex)
				connected_vertex = edge[1];
			else if (edge[1] == current_vertex)
				connected_vertex = edge[0];
			if (connected_vertex != 0) {
				const ptrdiff_t index = get_vertex_index(connected_vertex);
				if (!tags[index]) {
					tags[index] = true;
					queue.push_back({ new_counter, connected_vertex });
				}
			}
		}
	}
	throw std::invalid_argument("BFS error");
}

//std::vector<unsigned> Algorithms::DFS(const unsigned& start, const unsigned& finish) {
//	const unsigned start_index = get_vertex_index(start);
//	const unsigned finish_index = get_vertex_index(finish);
//	if (start_index == vertices.size()) throw std::invalid_argument("BFS: start vertex not fount in graph");
//	if (finish_index == vertices.size()) throw std::invalid_argument("BFS: finish vertex not fount in graph");
//	
//	tags[start_index] = true;
//	queue.push_back({ 0, start });
//	next_vertex(start);
//	
//
//	return get_vertex_path();
//}
//
//unsigned Algorithms::next_vertex(unsigned current) {
//	auto it = find_any_edge({ {current, 0}, {0, current} }, 0);
//	while (it != edges.end()) {
//		unsigned next;
//		if (current == (*it)[0])
//			next = (*it)[1];
//		else next = (*it)[0];
//		const ptrdiff_t index = get_vertex_index(next);
//		if (!tags[index]) {
//			std::cout << next << "   ";
//			tags[index] = true;
//			queue.push_back({ queue.back()[0] + 1, next });
//			next_vertex(next);
//		}
//		it = find_any_edge({ {current, 0}, {0, current} }, std::distance(edges.begin(), it) + 1);
//	}
//	return 0;
//}

std::vector<std::array<unsigned, 2>>::const_iterator Algorithms::find_any_edge(const std::vector<std::array<unsigned, 2>>& pairs, int start_index) const {
	if (start_index > static_cast<int>(edges.size()) - 1)
		return edges.end();
	return std::find_if(edges.begin() + start_index, edges.end(),
		[&pairs](const std::array<unsigned, 2>& edge) {
			bool found_any = false;
			for (const auto& pair : pairs) {
				if (pair[0] == 0)
					found_any = found_any || edge[1] == pair[1];
				else if (pair[1] == 0)
					found_any = found_any || edge[0] == pair[0];
				else
					found_any = found_any || (edge[0] == pair[0] && edge[1] == pair[1]);
			}
			return found_any;
		});
}

std::vector<unsigned> Algorithms::get_vertex_path() {
	const auto last_number = queue.back()[0];
	std::vector<unsigned> path(last_number + 1);
	path[last_number] = queue.back()[1];
	for (size_t i = queue.size() - 2, j = last_number - 1; j != 0; --i) {
		if (queue[i][0] == j &&
			find_any_edge({
				{queue[i][1], path[j + 1]},
				{path[j + 1], queue[i][1]} }) != edges.end()) {
			path[j] = queue[i][1];
			j--;
		}
	}
	path[0] = queue.front()[1];
	queue.clear();
	std::fill(tags.begin(), tags.end(), false);

	return path;
}