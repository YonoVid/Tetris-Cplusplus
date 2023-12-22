#ifndef TET_CLASS
#define TET_CLASS

#include<random>

namespace tet {

	class Tet {
	private:

		typedef int(*board)[10];
		std::mt19937 custom_random;
		int position[4][2];
		int grid[20][10];
		int piece_pool[7];
		int pool_index;
		int level;
		int points;
		int destroyed_lines;
		void (*print)(board tablero);
		void (*change_level)();

	public:
		
		Tet(void (*if_fall)(board tablero) = nullptr, void(*if_levelup)() = nullptr);
		void restart_board();
		void clean(int start);
		void make_pool();
		bool move_h(int way);
		int move_v();
		bool rotate(int way);
		bool new_piece();
		int check_line();
		board get_grid() { return this->grid; };
		void set_seed(unsigned seed) { custom_random.seed(seed); };
		int get_level() { return this->level; };
		void set_level(int new_level) { this->level = new_level; };
		int get_lines() { return this->destroyed_lines; };
		int get_points() { return this->points; }
		int get_piece() { return this->piece_pool[this->pool_index]; };
		int index_piece() { return this->pool_index; };

	};



}

#endif
