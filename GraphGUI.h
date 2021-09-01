#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include <array>

// split into 2 files
// use Algo methods in GraphGUI but with VertexStruct and EdgeStruct
// GraphGUI::start_BFS uses Algo::BFS
// subclasses?

class GraphGUI : public sf::Drawable {
	static constexpr float max_speed = 2.5f;
	static constexpr float deceleration = 0.995f;
	static constexpr float inactive_zone_coefficient = 0.05f;
protected:
	struct VertexStruct {
		static unsigned last_set_name;
		unsigned name;
		int degree;
		sf::Vector2f velocity;
		sf::Vector2f acceleration;
		sf::CircleShape representation;
		sf::Text label;
		sf::Vector2f target_size;
		VertexStruct(GraphGUI*, unsigned, int = 0, const sf::Vector2f & = { -1, -1 },
			const sf::Vector2f & = { 0, 0 }, const sf::Vector2f & = { 0, 0 });
		inline void add_acceleration(sf::Vector2f);
		inline void set_label(const sf::Font&, unsigned = 16);
		void update_vertex_config();
	};
	struct EdgeStruct {
		VertexStruct* Left;
		VertexStruct* Right;
		sf::Color color;
		bool visible;
		int weight;
		float length = 0.f;
		static constexpr float k = 0.1f;
		static constexpr float booster = 100.f;
		EdgeStruct(VertexStruct*, VertexStruct*, bool = true, int = 1);
		void set_resting_length(float, float, size_t);
		void apply_forces() const;
	};
	sf::Font m_Font;
	sf::Vector2f m_Target_Size;
	std::vector<VertexStruct>* m_Vertices;
	std::vector<EdgeStruct>* m_Edges;
	float m_Vertex_Radius;
	bool m_Weighted, m_Oriented, m_Planar;
	struct GeneratorStruct {
		std::random_device rd;
		std::default_random_engine gen{ rd() };
		std::uniform_real_distribution<float> distribution;
		inline GeneratorStruct(float, float);
		inline float get_random();
	} m_Position_Generator[2];
	void set_config();
	void set_vertices_labels();
	void set_edges_length() const;
	sf::Vector2f boundaries_check(const sf::Vector2f&) const;
	inline std::vector<VertexStruct>::iterator find_any_vertex(const std::vector<unsigned>&) const;
	inline std::vector<EdgeStruct>::iterator find_any_edge(const std::vector<std::array<unsigned, 2>>&) const;
	void add_connection(std::array<unsigned, 2>, unsigned);
	void list_parser(const std::vector<std::pair<std::array<unsigned, 2>, int>>&);
	void matrix_parser(const std::vector<std::vector<unsigned>>&);
	void add_invisible_edges() const;
	inline void planar_check();
	static inline float get_absolute_length(const sf::Vector2f&);
	static inline void set_absolute_length(sf::Vector2f&, float);
	void vertices_draw(sf::RenderTarget&) const;
	void edges_draw(sf::RenderTarget&) const;
public:
	GraphGUI(const sf::RenderTarget&, const std::vector<std::pair<std::array<unsigned, 2>, int>>&, float = 5.f, bool = false, bool = false, bool = false);
	GraphGUI(const sf::RenderTarget&, const std::vector<std::vector<unsigned>>&, float = 5.f, bool = false, bool = false, bool = false);

	std::pair<std::vector<unsigned>, std::vector<std::array<unsigned, 2>>> get_config() const;
	void build_graph(sf::RenderWindow&) const;
	void enlighten_path(sf::RenderWindow&, const std::vector<unsigned>& path) const;
	void reset_color() const;
	void draw(sf::RenderTarget&, sf::RenderStates) const override;

	~GraphGUI() override;
};
