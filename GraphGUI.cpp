#include "GraphGUI.h"

#include <iostream>

GraphGUI::GraphGUI(const sf::RenderTarget& target, const std::vector<std::pair<std::array<unsigned, 2>, int>>& edges_list, float radius, bool weighted, bool oriented, bool planar)
	: m_Target_Size(target.getSize()), m_Vertices(new std::vector<VertexStruct>), m_Edges(new std::vector<EdgeStruct>),
	m_Vertex_Radius(radius), m_Weighted(weighted), m_Oriented(oriented), m_Planar(planar),
	m_Position_Generator{
			{ static_cast<float>(target.getSize().x) * inactive_zone_coefficient,
				static_cast<float>(target.getSize().x) * (1 - inactive_zone_coefficient) - radius },
			{ static_cast<float>(target.getSize().y) * inactive_zone_coefficient,
				static_cast<float>(target.getSize().y) * (1 - inactive_zone_coefficient) - radius }
}
{
	m_Vertices->reserve(edges_list.size() * 2);
	m_Edges->reserve(edges_list.size()* (2 * edges_list.size() - 1));
	list_parser(edges_list);
	set_config();
}

GraphGUI::GraphGUI(const sf::RenderTarget& target, const std::vector<std::vector<unsigned>>& adjacency_matrix, float radius, bool weighted, bool oriented, bool planar)
	: m_Target_Size(target.getSize()), m_Vertices(new std::vector<VertexStruct>()), m_Edges(new std::vector<EdgeStruct>),
	m_Vertex_Radius(radius), m_Weighted(weighted), m_Oriented(oriented), m_Planar(planar),
	m_Position_Generator{
			{ static_cast<float>(target.getSize().x) * inactive_zone_coefficient,
				static_cast<float>(target.getSize().x) * (1 - inactive_zone_coefficient) - radius },
			{ static_cast<float>(target.getSize().y) * inactive_zone_coefficient,
				static_cast<float>(target.getSize().y) * (1 - inactive_zone_coefficient) - radius }
}
{
	m_Vertices->reserve(adjacency_matrix.size());
	m_Edges->reserve(adjacency_matrix.size()* (adjacency_matrix.size() - 1) / 2);
	matrix_parser(adjacency_matrix);
	set_config();
}

void GraphGUI::set_config() {
	add_invisible_edges();
	planar_check();
	set_vertices_labels();
	set_edges_length();
}

std::pair<std::vector<unsigned>, std::vector<std::array<unsigned, 2>>> GraphGUI::get_config() const {
	std::vector<unsigned> vertices(m_Vertices->size());
	std::vector<std::array<unsigned, 2>> edges;
	for (unsigned i = 0; i < (*m_Vertices).size(); ++i) {
		vertices[i] = (*m_Vertices)[i].name;
	}
	for (const auto& edge_iter : (*m_Edges)) {
		if (edge_iter.visible) {
			edges.push_back({ edge_iter.Left->name, edge_iter.Right->name });
		}
	}
	return { vertices, edges };
}

GraphGUI::GeneratorStruct::GeneratorStruct(float min, float max) {
	distribution = std::uniform_real_distribution<float>(min, max);
}
float GraphGUI::GeneratorStruct::get_random() {
	return distribution(gen);
}

void GraphGUI::set_vertices_labels() {
	if (m_Font.loadFromFile(R"(arial.ttf)")) {
		for (auto& vertex : *m_Vertices) {
			vertex.set_label(m_Font);
		}
	}
	else throw std::invalid_argument("Can't open font file");
}
void GraphGUI::set_edges_length() const {
	for (auto& edge : (*m_Edges)) {
		edge.set_resting_length(m_Target_Size.x, m_Target_Size.y, (*m_Vertices).size());
	}
}

unsigned GraphGUI::VertexStruct::last_set_name = 1;
GraphGUI::VertexStruct::VertexStruct(GraphGUI* _this, unsigned name, int degree,
	const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Vector2f& acceleration)
	: name(name), degree(degree), velocity(velocity), acceleration(acceleration), representation(_this->m_Vertex_Radius) {
	if (!(name == last_set_name) && name > last_set_name)
		last_set_name = name + 1;
	const auto checked_position = _this->boundaries_check(position);
	if (position == checked_position)
		representation.setPosition(position);
	else representation.setPosition(_this->m_Position_Generator[0].get_random(), _this->m_Position_Generator[1].get_random());
}

void GraphGUI::VertexStruct::add_acceleration(sf::Vector2f vector) {
	acceleration += vector;
}
void GraphGUI::VertexStruct::set_label(const sf::Font& font, const unsigned size) {
	label.setFont(font);
	label.setString(std::to_string(name));
	label.setCharacterSize(size);
	label.setFillColor(sf::Color::Yellow);
	label.setPosition(
		representation.getPosition().x - static_cast<float>(size),
		representation.getPosition().x - static_cast<float>(size));
}

void GraphGUI::VertexStruct::update_vertex_config() {
	velocity += acceleration;
	velocity *= deceleration;
	if (get_absolute_length(velocity) > max_speed)
		set_absolute_length(velocity, max_speed);
	const auto new_position = representation.getPosition() + velocity;
	representation.setPosition(new_position);
	const auto offset = static_cast<float>(label.getCharacterSize());
	label.setPosition(new_position - sf::Vector2f(offset, offset));
	acceleration = { 0, 0 };
}

GraphGUI::EdgeStruct::EdgeStruct(VertexStruct* Left, VertexStruct* Right, bool visible, int weight)
	: Left(Left), Right(Right), color(sf::Color::White), visible(visible), weight(weight) {}
void GraphGUI::EdgeStruct::set_resting_length(float x_max, float y_max, size_t vertices_amount) {
	// TODO: rewrite formulas due to https://cs.brown.edu/people/rtamassi/gdhandbook/chapters/force-directed.pdf
	const auto min = std::min(x_max, y_max);
	const auto degree_difference = static_cast<float>(std::abs(Left->degree - Right->degree) + 1);
	const auto max_degree = static_cast<float>(std::max(Left->degree, Right->degree));
	const auto amount_coefficient = min / std::pow(static_cast<float>(vertices_amount), 1.f / 4.f);
	if (visible)
		this->length = amount_coefficient / degree_difference * max_degree / 15.f;
	else
		this->length = amount_coefficient * 1.5f;
}
void GraphGUI::EdgeStruct::apply_forces() const {
	sf::Vector2f Force = Right->representation.getPosition() - Left->representation.getPosition();
	const float current_length = k * (get_absolute_length(Force) - length);
	set_absolute_length(Force, current_length);
	// a = F / m
	if (visible) {
	}
	else {
		Force /= 100.f;
	}
	Left->add_acceleration(Force / static_cast<float>(Left->degree) / booster);
	Right->add_acceleration(Force * -1.f / static_cast<float>(Right->degree) / booster);
}

std::vector<GraphGUI::VertexStruct>::iterator GraphGUI::find_any_vertex(const std::vector<unsigned>& vertex_names) const {
	return std::find_if(m_Vertices->begin(), m_Vertices->end(),
		[&vertex_names](const VertexStruct& vertex) {
			bool found_any = false;
			for (const auto& vertex_name : vertex_names) {
				found_any = found_any || vertex.name == vertex_name;
			}
			return found_any; });
}

std::vector<GraphGUI::EdgeStruct>::iterator GraphGUI::find_any_edge(const std::vector<std::array<unsigned, 2>>& pairs) const {
	return std::find_if(m_Edges->begin(), m_Edges->end(),
		[&pairs](const EdgeStruct& edge) {
			bool found_any = false;
			for (const auto& pair : pairs) {
				found_any = found_any || (edge.Left->name == pair[0] && edge.Right->name == pair[1]);
			}
			return found_any;
		});
}

void GraphGUI::add_connection(std::array<unsigned, 2> pair, unsigned weight) {
	VertexStruct* Connection[2]{};
	for (unsigned i = 0; i < pair.size(); ++i) {
		auto current_name = pair[i];
		const auto it = find_any_vertex({ current_name });
		if (it == m_Vertices->end()) {
			m_Vertices->emplace_back(this, current_name, 1);
			Connection[i] = &m_Vertices->back();
		}
		else {
			(*it).degree++;
			Connection[i] = &*it;
		}
	}
	m_Edges->emplace_back(Connection[0], Connection[1], weight);
}
void GraphGUI::list_parser(const std::vector<std::pair<std::array<unsigned, 2>, int>>& edges_list) {
	std::vector<bool> checked(edges_list.size(), false);                    //// check
	int count = 0;
	for (const auto& connection : edges_list) {
		if (!checked[count]) {
			add_connection(connection.first, connection.second);

			if (!m_Weighted && connection.second > 1)
				m_Weighted = true;
			auto it = std::find_if(edges_list.begin(), edges_list.end(),
				[&connection](const std::pair<std::array<unsigned, 2>, int>& element) {
					return (element.first[0] == connection.first[1]) && (element.first[1] == connection.first[0]); });

			if (it != edges_list.end()) {
				if ((*it).second == 0) {
					m_Oriented = true;
					checked[std::distance(edges_list.begin(), it)] = true;
				}
				else throw std::invalid_argument(
					"Invalid input\nIf exists {<v1> <v2> <w>} then inverse must be {<v2> <v1> 0} to specify orientation");
			}
		}
		count++;
	}
}
void GraphGUI::matrix_parser(const std::vector<std::vector<unsigned>>& adjacency_matrix) {
	// beta
	for (unsigned i = 0; i < adjacency_matrix.size(); i++) {
		for (unsigned j = i + 1; j < adjacency_matrix[0].size(); j++) {
			if (adjacency_matrix[i][j] != 0) {
				add_connection({ i + 1, j + 1 }, adjacency_matrix[i][j]);
				if (!m_Weighted && adjacency_matrix[i][j] > 1) {
					m_Weighted = true;
				}
				if (!m_Oriented && adjacency_matrix[i][j] != adjacency_matrix[j][i]) {
					m_Oriented = true;
					if (adjacency_matrix[i][j] != 0 && adjacency_matrix[i][j] != adjacency_matrix[j][i]) {
						throw std::invalid_argument("Weight for an edge must be constant regardless of direction");
					}
				}
			}
		}
	}
}
void GraphGUI::add_invisible_edges() const {
	for (unsigned i = 0; i < m_Vertices->size(); ++i) {
		for (unsigned j = i + 1; j < m_Vertices->size(); ++j) {
			if (find_any_edge({
				{ (*m_Vertices)[i].name, (*m_Vertices)[j].name },
				{ (*m_Vertices)[j].name, (*m_Vertices)[i].name }
				}) == m_Edges->end())
			{
				m_Edges->emplace_back(&(*m_Vertices)[i], &(*m_Vertices)[j], false);
			}
		}
	}
}

void GraphGUI::planar_check() {
	if (m_Edges->size() <= 3 * m_Vertices->size() - 6)
		m_Planar = true;
}
void GraphGUI::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	//if (m_Planar)
		//planar_graph_draw(target);
	vertices_draw(target);
	edges_draw(target);
}
void GraphGUI::vertices_draw(sf::RenderTarget& target) const {
	for (const auto& vertex : (*m_Vertices)) {
		target.draw(vertex.representation);
		target.draw(vertex.label);
	}
}
void GraphGUI::edges_draw(sf::RenderTarget& target) const {
	for (const auto& edge : (*m_Edges)) {
		if (edge.visible) {
			const sf::Vertex line[] = {
				sf::Vertex(edge.Left->representation.getPosition() + sf::Vector2f(m_Vertex_Radius, m_Vertex_Radius), edge.color),
				sf::Vertex(edge.Right->representation.getPosition() + sf::Vector2f(m_Vertex_Radius, m_Vertex_Radius), edge.color)
			};
			target.draw(line, 2, sf::Lines);
		}
	}
}

void GraphGUI::build_graph(sf::RenderWindow& window) const {
	float min_change = 1;
	while (window.isOpen() && min_change > 0.01f) {
		for (auto& edge : (*m_Edges)) {
			edge.apply_forces();
		}
		min_change = 0;
		for (auto& vertex : (*m_Vertices)) {
			vertex.update_vertex_config();
			const auto circle_position = boundaries_check(vertex.representation.getPosition());
			if (vertex.representation.getPosition() != circle_position)
				vertex.representation.setPosition(circle_position);
			const auto label_position = boundaries_check(vertex.label.getPosition());
			if (vertex.label.getPosition() != label_position)
				vertex.label.setPosition(label_position);
			min_change += get_absolute_length(vertex.velocity);
		}
		sf::Event event{};
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(*this);
		window.display();
	}
}

void GraphGUI::enlighten_path(sf::RenderWindow& window, const std::vector<unsigned>& path) const {
	bool wait = true;
	for (unsigned i = 0; i < path.size() - 1 && window.isOpen();) {
		sf::Event event{};
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
				wait = false;
		}
		if (!wait) {
			const auto it = find_any_edge({
				{path[i], path[i + 1]},
				{path[i + 1], path[i]} });
			sf::Color color = sf::Color::Green;
			if (i == 0) {
				color = sf::Color::Red;
			}
			else if (i == path.size() - 2) {
				if (path[i] == it->Left->name)
					it->Left->representation.setFillColor(color);
				else if (path[i] == it->Right->name)
					it->Right->representation.setFillColor(color);
				++i;
				color = sf::Color::Red;
			}
			if (path[i] == it->Left->name)
				it->Left->representation.setFillColor(color);
			else if (path[i] == it->Right->name)
				it->Right->representation.setFillColor(color);
			it->color = sf::Color::Green;
			++i;
		}
		if (!wait) {
			window.clear();
			window.draw(*this);
			window.display();
			wait = true;
		}
	}
}

void GraphGUI::reset_color() const {
	for (auto& vertex : (*m_Vertices)) {
		vertex.representation.setFillColor(sf::Color::White);
	}
	for (auto& edge : (*m_Edges)) {
		edge.color = sf::Color::White;
	}
}

float GraphGUI::get_absolute_length(const sf::Vector2f& vector) {
	return std::sqrt(static_cast<float>(std::pow(vector.x, 2) + std::pow(vector.y, 2)));
}
void GraphGUI::set_absolute_length(sf::Vector2f& vector, const float new_length) {
	const float old_length = get_absolute_length(vector);
	vector *= new_length / old_length;
}
sf::Vector2f GraphGUI::boundaries_check(const sf::Vector2f& position) const {
	auto x = position.x;
	auto y = position.y;
	const auto x_max = m_Target_Size.x - 2 * m_Vertex_Radius;
	const auto y_max = m_Target_Size.y - 2 * m_Vertex_Radius;
	if (x < 0.f)
		x = 0.f;
	else if (x > x_max)
		x = x_max;
	if (y < 0.f)
		y = 0.f;
	else if (y > y_max)
		y = y_max;
	return { x, y };
}

//template<class T>
//void GraphGUI::render_window(sf::RenderWindow& window, T method) {
//	while (window.isOpen()) {
//		sf::Event event{};
//		while (window.pollEvent(event)) {
//			if (event.type == sf::Event::Closed)
//				window.close();
//		}
//		if(func()) break;
//		window.clear();
//		window.draw(*this);
//		window.display();
//	}
//}

//void GraphGUI::planar_graph_draw(sf::RenderTarget& target) const {}

GraphGUI::~GraphGUI() {
	m_Edges->clear();
	m_Vertices->clear();
	delete m_Edges;
	delete m_Vertices;
}