#include<random>
#include<curses.h>
#include"signal.h"
#include<ctime>
#include<chrono>
#include<thread>
#include "TET.h"

class Timer;

char mayusculas(char caracter);
int char_to_int(char value[2]);
void mostrar(int (*grid)[10]);
void game_print(std::string = "");
void clean_lines(int(*grid)[10]);
void update_speed();
int fall_piece(Timer &timer);

static char g_comando;
static char input{ '\0' };
static int g_contador, g_tiempo_espera;
static bool g_perder, g_destruir;
static int g_stored_index, g_stored_level;
typedef std::chrono::high_resolution_clock user_clock;
tet::Tet game(clean_lines, update_speed);

class Timer {

	private:
		user_clock::time_point time{ user_clock::now() };
		double timepassed{ 0 };
		int recordedtime[2]{0, 0 };

	public:

		int time_since_start() {
			user_clock::duration d{ user_clock::now() - time };
			return d.count();
		};
		
		void add_time() {

			recordedtime[1] = time_since_start();
			timepassed += static_cast<double>(recordedtime[1] - recordedtime[0]) / 10e9;
			recordedtime[0] = recordedtime[1];

		};

		double time_added() { return timepassed; };
		void frame_done(const double update_speed) { timepassed -= update_speed; };
		void restart() { recordedtime[0] = time_since_start(); };
};

int char_to_int(char value[2]) {
	if (value[1] == 0) {
		return int(value[0] - 48);
	}
	else {
		return int((value[0] - 48) * 10) + int(value[1] - 48);
	}

}

char mayusculas(char caracter) {
	if (caracter > 96) {
		return caracter - 32;
	}
	else {
		return caracter;
	}
}

void clean_lines(int(*grid)[10]) {

	mostrar(grid);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

}

void game_print(std::string error_msj) {

	mostrar(game.get_grid());
	printw(" PUNTAJE: %d\n", game.get_points());
	printw(" LINEAS: %d\n", game.get_lines());
	printw(" NIVEL: %d\n", game.get_level());

	printw(error_msj.c_str());
	move(32, 1);
	refresh();

}

void update_speed() {

	g_stored_level = game.get_level();

	if (g_stored_level < 9) {
		g_tiempo_espera = 30 - (g_stored_level * 3);
	}
	else if(g_stored_level == 9){
		g_tiempo_espera = 6;
	}
	else if (g_stored_level > 9 && g_stored_level < 13) {
		g_tiempo_espera = 5;
	}
	else if (g_stored_level > 12 && g_stored_level < 16) {
		g_tiempo_espera = 4;
	}
	else if (g_stored_level > 15 && g_stored_level < 19) {
		g_tiempo_espera = 3;
	}
	else if (g_stored_level > 18 && g_stored_level < 29) {
		g_tiempo_espera = 2;
	}
	else if (g_stored_level >= 29) {
		g_tiempo_espera = 1;
	}
}

void next_piece() {

	g_stored_index = game.index_piece();
	g_contador = 0;
	
	switch (game.get_piece()) {
	case 0:
		move(5, 9);
		printw("--------");
		move(6, 9);
		printw("--[][]--");
		move(7, 9);
		printw("--[][]--");
		move(8, 9);
		printw("--------");
		break;
	case 1:
		move(5, 9);
		printw("--------");
		move(6, 9);
		printw("--[]----");
		move(7, 9);
		printw("--[]----");
		move(8, 9);
		printw("--[][]--");
		break;
	case 2:
		move(5, 9);
		printw("--------");
		move(6, 9);
		printw("----[]--");
		move(7, 9);
		printw("----[]--");
		move(8, 9);
		printw("--[][]--");
		break;
	case 3:
		move(5, 9);
		printw("--[]----");
		move(6, 9);
		printw("--[]----");
		move(7, 9);
		printw("--[]----");
		move(8, 9);
		printw("--[]----");
		break;
	case 4:
		move(5, 9);
		printw("--------");
		move(6, 9);
		printw("----[]--");
		move(7, 9);
		printw("--[][]--");
		move(8, 9);
		printw("--[]----");
		break;
	case 5:
		move(5, 9);
		printw("--------");
		move(6, 9);
		printw("--[]----");
		move(7, 9);
		printw("--[][]--");
		move(8, 9);
		printw("----[]--");
		break;
	case 6:
		move(5, 9);
		printw("--------");
		move(6, 9);
		printw("--[]----");
		move(7, 9);
		printw("--[][]--");
		move(8, 9);
		printw("--[]----");
		break;
	}
}

void mostrar(int (*grid)[10]){

	std::string iconos[10];
	// Esta funcion se encarga de crear una representacion del arreglo grid
	for (int e = 0; e <= 19; e++) {
		move(3 + e, 21);
		printw("|");
		for (int i = 0; i <= 9; i++) {
			if (grid[e][i] == 0) {
				// Si el valor es 0 se le representa como "--"
				printw("--");
			}
			else {
				// Si el valor es 1 0 2 se le representa como "[]"
				printw("[]");
			}
		}
		// Se imprime cada fila del arreglo con su representacion respectiva almacenada en iconos
		printw("|\n");
	}
	refresh();
}

int fall_piece(Timer &timer) {

	int result{ game.move_v() };
	if (g_stored_index != game.index_piece()) {
		next_piece();
		timer.restart();
	}
	if (result == 3) {
		g_perder = true;

	}
	return result;
}

int main() {
	
	Timer timer{};
	uint_fast16_t seed;
	std::string error;
	char enter[2];
	char used_key{NULL};
	WINDOW *screen = initscr();
	resize_term(32,64);
	(void)noecho();
	
	while (true) {

		nodelay(screen, false);
		curs_set(1);
		clear();
		move(2, 17);
		printw(" ___________\n");
		move(3, 17);
		printw("|           |          ___ \n");
		move(4, 17);
		printw("|___     ___|______ __|   |__\n");
		move(5, 21);
		printw("|   |   |   ___|__     __|\n");
		move(6, 21);
		printw("|   |   |  |___   |   |\n");
		move(7, 21);
		printw("|   |   |   ___|  |   |\n");
		move(8, 21);
		printw("|   |   |  |___   |   |\n");
		move(9, 21);
		printw("|___|   |______|  |___|\n");
		move(12, 16);
		printw("A tetris clone made with love <3  \n");
		move(13, 16);
		printw("Original game by Alexey Pajitnov \n");
		move(14, 20);
		printw("Developed in C++ by Vid    \n");
		move(18, 17);
		printw("Presiona Enter para continuar\n");
		refresh();
		getnstr(enter, 1);
		// Se crean dos arreglos uno para los datos y otro para
		// la posicion de la pieza actual
		// Se indica puntaje inicial 0 y que no hay errores

		error = " ";
		g_perder = false;
		seed = static_cast<uint_fast16_t>(timer.time_since_start() / 6e4);
		game.set_seed(seed);
		game.restart_board();
		// Se muestran las instrucciones y se le pide al jugador
		// elegir el nivel con el que partir
		clear();
		move(4,24);
		printw("INSTRUCCIONES:\n");
		move(6, 8);
		printw(" [A] Para mover la pieza a la izquierda\n");
		move(7, 8);
		printw(" [D] Para mover la pieza a la derecha\n");
		move(8, 8);
		printw(" [Q o J] Para girar la pieza en sentido antihorario\n");
		move(9, 8);
		printw(" [E o K] Para girar la pieza en sentido horario\n");
		move(10, 8);
		printw(" [S] Para mover la pieza hacia abajo\n");
		move(11, 8);
		printw(" [W] Para dejar la pieza caer\n");
		move(14, 2);
		printw(" Elije un nivel entre 1 y 30, presiona Enter para continuar");
		refresh();
		enter[0] = '0';
		(void)echo();
		while((char_to_int(enter) > 30) | (char_to_int(enter) < 1)){

			move(16, 30);
			printw("  ");
			move(16, 30);
			getnstr(enter, 2);

		}
		(void)noecho();
		game.set_level(char_to_int(enter));
		g_stored_level = game.get_level();
		update_speed();
		game.new_piece();
		// Se crea la primera pieza

		clear();
		move(3, 8);
		printw("NEXT PIECE:");
		move(4, 8);
		printw(" ________ ");
		move(5, 8);
		printw("|        |");
		move(6, 8);
		printw("|        |");
		move(7, 8);
		printw("|        |");
		move(8, 8);
		printw("|        |");
		move(9, 8);
		printw("|________|");
		next_piece();
		nodelay(screen, true);
		curs_set(0);
		timer.restart();

		while (!g_perder) {
			// Se muestran una representacion de los datos al usuario
			// junto a las intrucciones y datos de la partida
			//texto_puntaje = std::string(puntaje);

			game_print(error);
			move(-1, -1);
			error = "";

			timer.add_time();

			g_comando = mayusculas(getch());
			if (g_comando != ERR) { input = g_comando; }

			while (timer.time_added() >= 1.0/80.0) {	
				
				if (g_contador >= g_tiempo_espera) {

					fall_piece(timer);
					g_contador = 0;

				}

				if (!g_perder) {
					if (input == 'A') {
						game.move_h(-1);
						flushinp();
					}
					else if (input == 'D') {
						game.move_h(1);
						flushinp();
					}
					else if (input == 'Q' || input == 'J') {
						game.rotate(-1);
						flushinp();
					}
					else if (input == 'E' || input == 'K') {
						game.rotate(1);
						flushinp();
					}
					else if (input == 'S') {
						fall_piece(timer);
						flushinp();
					}
					else if (input == 'W') {
						int fall = fall_piece(timer);
						while (!(fall)) {
							// El ciclo se encarga de utilizar la funcion "Avanzar" hasta que la
							// pieza se deba detener
							mostrar(game.get_grid());
							fall = game.move_v();
							std::this_thread::sleep_for(std::chrono::milliseconds(20));
						}
						next_piece();
						flushinp();
						timer.restart();
					}
				}
				g_contador += 1;
				timer.frame_done(1.0 / 80.0);

				if (timer.time_added() < 1.0/80.0){ 
					used_key = g_comando;
					g_comando = '\0';
					input = '\0';
				}
			}
		}
		clear();
		game_print(error);
		move(27, 2);
		printw("Fin de la partida\n");
		printw("  Presiona Enter para volver al menu principal");
		getnstr(enter, 1);
	}
	return 0;
}