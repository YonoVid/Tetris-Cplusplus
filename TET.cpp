#include "TET.h"

static std::uniform_int_distribution<int> d7(0, 6);

tet::Tet::Tet(void(*if_fall)(board tablero), void (*if_levelup)()) {
	if (if_fall){
		this -> print = if_fall; 
	}
	if (if_levelup) {
		this->change_level = if_levelup;
	}
}

void tet::Tet::clean(int start) {
	// Funcio encargada de eliminar los valores en las posiciones antiguas
	for (int e = start; e <= 3; e++) {

		this->grid[this->position[e][0]][this->position[e][1]] = 0;
	}
}

void tet::Tet::restart_board() {
	int e, i;
	this->destroyed_lines = 0;
	this->level = 0;
	this->points = 0;
	this->make_pool();
	// Funcion encargada de asignar a cada variable del arreglo en valor 0
	for (e = 0; e <= 19; e++) {
		for (i = 0; i <= 9; i++) {
			this->grid[e][i] = 0;
		}
	}
}

bool tet::Tet::move_h(int way) {

	// Esta funcion se encarga de mover las piezas a izquierda y derecha una espacio.
	// Retorna un booleano indicando si se ha desplazado

	for (int e = 0; e <= 3; e++) {
		// Se verifica que las posibles posiciones en las que se desplazaran los valores no esten fuera de rango o ocupadas
		if ((this->position[e][1] + way < 0 || this ->position[e][1] + way > 9 || this->grid[position[e][0]][position[e][1] + way] == 1)) {
			return false;
		}
	}

	// Se eliminar los valores en las posiciones antiguas
	this->clean(0);

	for (int e = 0; e <= 3; e++) {

		// Se ubican los valores en sus nuevas posiciones, "Way" es la cantidad que se movera
		this->grid[this->position[e][0]][this->position[e][1] + way] = 2;
		// Se almecena las nuevas coordenadas
		this->position[e][1] = this->position[e][1] + way;
	}
	return true;

}

bool tet::Tet::rotate(int way) {

	int x, y;
	int temporalx, temporaly;
	// Esta funcion se encarga de rotar las piezas en 90° en sentido horario o antihorario.
	// Retorna un booleano indicando si se ha desplazado
	// Se almacenan las coordenadas del centro en x e y
	x = this->position[0][1];
	y = this->position[0][0];

	for (int e = 1; e <= 3; e++) {

		// Se almacena temporalmente la distancia al eje, almacenado en la posicion [0,0]
		temporalx = x - this-> position[e][1];
		temporaly = y - this-> position[e][0];

		if (x + (temporaly * way) < 0 || x + (temporaly * way) > 9 || y - (temporalx * way) > 19) {
			// Se verifica que las posibles posiciones no esten fuerda de rango
			return false;
		}
		if (this-> grid[y - (temporalx * way)][x + (temporaly * way)] == 1) {
			// Se verifica que las posibles posiciones no esten ocupadas con otro valor
			return false;
		}
	}

	// Se eliminar los valores en las posiciones antiguas
	this->clean(1);

	for (int e = 1; e <= 3; e++) {

		// Se almacenan temporalmente las distancias al eje, almacenado en [0,0]
		temporalx = x - position[e][1];
		temporaly = y - position[e][0];
		// Se ubican los valores en sus nuevas posiciones
		this->grid[y - (temporalx * way)][x + (temporaly * way)] = 2;
		// Se almacenan las nuevas coordenadas en el arreglo "Position"
		this-> position[e][0] = y - (temporalx * way);
		this-> position[e][1] = x + (temporaly * way);
	}

	return true;
}

int tet::Tet::move_v() {
	
	int result;
	bool mover;
	// Esta funcion se encarga de hacer que las piezas desciendan un espacio hacia abajo.
	// Retorna un booleano indicando si se debe crear una nueva pieza
	mover = true;
	for (int e = 0; e <= 3; e++) {
		// Se verifica que la posible posicion este dentro del rango y no este utilizada por otros datos
		if (this-> position[e][0] + 1 == 20 || this-> grid[this-> position[e][0] + 1][this-> position[e][1]] == 1) {
			mover = 0;
		}
	}

	if (mover) {
		// Se eliminar los valores en las posiciones antiguas
		this->clean(0);
		for (int e = 0; e <= 3; e++) {

			// Se ubican los valores en sus nuevas posiciones
			this->grid[this->position[e][0] + 1][this->position[e][1]] = 2;
			// Se almacenan las nuevas coordenadas en el arreglo "Position"
			this->position[e][0] = this->position[e][0] + 1;

		}

		return 0;
	}
	else {

		for (int e = 0; e <= 3; e++) {
			// Se cambian los valores que indican la pieza actual con
			// el de piezas ya colocadas
			this-> grid[this-> position[e][0]][this-> position[e][1]] = 1;
		}

		// Se revisa si alguna linea se ha completado
		result = this->check_line();
		// Se crea una nueva pieza
		if (this->new_piece()) {
			result = 3;
		}
		return result;
	}
}

int tet::Tet::check_line() {

	bool completada;
	bool destruir;
	int distancia;
	int result{ 1 };
	int fila;
	int lineas[4];
	int temporal;
	// Funcion encargada de revisar si una fila se a completado ,de ser asi cambiar sus valores por 0
	destruir = false;
	for (int e = 0; e <= 3; e++) {
		completada = true;
		for (int i = 0; i <= 9; i++) {
			// Se verifican si alguna de las filas en "Position" esta completa
			if (completada && this-> grid[this-> position[e][0]][i] == 0) {
				completada = false;
			}
		}
		// Si la linea se ha completado se almacena su posicion en "lineas", en
		// caso contrario, se almacena un -1
		if (completada) {
			lineas[e] = this-> position[e][0];
			if (!(destruir)) {
				// Si aun no se ha detectado ninguna fila completa se cambia el
				// valor de destruir para indicar que ha ocurrido
				destruir = true;
			}
		}
		else {
			lineas[e] = -1;
		}
	}
	if (destruir) {
		destruir = true;
		for (int e = 0; e <= 3; e++) {
			// Se busca cualquier fila que pueda estar repetida y se cambia su valor con un -1
			if (!(lineas[e] == -1)) {
				for (int i = 0; i <= 3; i++) {
					if (!(e == i) && lineas[e] == lineas[i]) {
						lineas[i] = -1;
					}
				}
			}
		}
		for (int e = 1; e <= 3; e++) {
			// Se ordenan las filas de mayor a menor
			completada = true;
			for (int i = e - 1; i >= 0; i--) {
				if (completada && (lineas[i + 1] > lineas[i])) {
					temporal = lineas[i + 1];
					lineas[i + 1] = lineas[i];
					lineas[i] = temporal;
				}
				else {
					completada = false;
				}
			}
		}
		// Se utiliza "temporal" para almacenar la cantidad de lineas destruidas
		temporal = 0;
		// Se utiliza "distancia" para la cantidad de filas que se debe avanzar
		distancia = 0;
		//Se utiliza completada para determinar si se llega a una fila vacia
		completada = true;
		// Se utiliza "fila" para indicar la siguiente fila a destruir
		fila = lineas[0];
		while (completada) {
			completada = false;
			if (fila == lineas[temporal]) {
				// Si se llega a una fila que se debe destruir se aumenta distancia
				// y se aumenta temporal si es menor a 3
				if (temporal < 3) {
					temporal = temporal + 1;
				}
				distancia = distancia + 1;
			}
			else {
				// Si no es una fila que se debe destruir se le mueve la distancia
				// indicada por la variable "distancia"
				for (int i = 0; i <= 9; i++) {
					this-> grid[fila + distancia][i] = this-> grid[fila][i];
				}
			}
			for (int i = 0; i <= 9; i++) {

				// Se cambia el valor de completada si hay un valor no nulo en la fila
				if (!completada && grid[fila][i] == 1) {
					completada = true;
				}
				// Se remplazan los valores de la fila con 0
				this->grid[fila][i] = 0;
			}

			if (this->print) {
				this->print(this->grid);
			}

			// Se retrocede a la fila anterior y se muestra el estado actual de "grid"
			// en pantalla
			fila = fila - 1;

		}
		// Se actualiza el nivel de ser necesario junto a la velocidad de caida de las
		// piezas
		this->destroyed_lines += distancia;
		if (this-> level != 30 && this-> level * 10 <= this-> destroyed_lines) {
			this-> level += 1;

			if (this->change_level) {
				this-> change_level();
			}
			
			result = 2;

		}
		// Finalmente se otorga un puntaje dependiendo de la cantidad de lineas destruidas
		// simultaneamente y se suman al total de lineas destruidas
		temporal = 100 + (200 * (distancia - 1));
		if (distancia == 4) {
			temporal += 500;
		}

		this-> points += temporal * this-> level;
	}
	return result;
}

void tet::Tet::make_pool() {
	
	int temporal1, temporal2;
	int number;
	this->pool_index = 0;
	
	for (int e = 0; e < 7; e++) {
		this->piece_pool[e] = e;
	}
	for (int e = 0; e < 4; e++) {
		temporal1 = d7(this->custom_random);
		temporal2 = d7(this->custom_random);
		number = this->piece_pool[temporal1];
		this->piece_pool[temporal1] = this->piece_pool[temporal2];
		this->piece_pool[temporal2] = number;
	}

}

bool tet::Tet::new_piece() {

	bool result{ 0 };
	// Funcion encargada de elegir una nueva pieza aleatoriamente y colocarla en "grid",
	// almacenando sus coordenadas en "Position".
	// Retorna un booleano si la pieza no se puede colocar porque algun espacio ya se
	// esta utilizando
	
	switch (this->piece_pool[this->pool_index]) {
	case 0:
		
		if (this->grid[0][4] == 1 || this->grid[1][4] == 1 || this->grid[0][5] == 1 || this->grid[1][5] == 1) {
			result = true;
		}

		this->grid[0][4] = 2;
		this->grid[1][4] = 2;
		this->grid[0][5] = 2;
		this->grid[1][5] = 2;
		this->position[0][0] = 1;
		this->position[0][1] = 4;
		this->position[1][0] = 0;
		this->position[1][1] = 4;
		this->position[2][0] = 0;
		this->position[2][1] = 5;
		this->position[3][0] = 1;
		this->position[3][1] = 5;

		break;

	case 1:
		
		if (this->grid[0][4] == 1 || this->grid[1][4] == 1 || this->grid[2][4] == 1 || this->grid[2][5] == 1) {
			result = true;
		}
		this->grid[0][4] = 2;
		this->grid[1][4] = 2;
		this->grid[2][4] = 2;
		this->grid[2][5] = 2;
		this->position[0][0] = 1;
		this->position[0][1] = 4;
		this->position[1][0] = 0;
		this->position[1][1] = 4;
		this->position[2][0] = 2;
		this->position[2][1] = 4;
		this->position[3][0] = 2;
		this->position[3][1] = 5;
		break;

	case 2:
	
		if (this->grid[0][5] == 1 || this->grid[1][5] == 1 || this->grid[2][5] == 1 || this->grid[2][4] == 1) {
			result = true;
		}
		this->grid[0][5] = 2;
		this->grid[1][5] = 2;
		this->grid[2][5] = 2;
		this->grid[2][4] = 2;
		this->position[0][0] = 1;
		this->position[0][1] = 5;
		this->position[1][0] = 0;
		this->position[1][1] = 5;
		this->position[2][0] = 2;
		this->position[2][1] = 5;
		this->position[3][0] = 2;
		this->position[3][1] = 4;
		
		break;

	case 3:

		if (this->grid[0][4] == 1 || this->grid[1][4] == 1 || this->grid[2][4] == 1 || this->grid[3][4] == 1) {
			result = true;
		}

		this->grid[0][4] = 2;
		this->grid[1][4] = 2;
		this->grid[2][4] = 2;
		this->grid[3][4] = 2;
		this->position[0][0] = 2;
		this->position[0][1] = 4;
		this->position[1][0] = 0;
		this->position[1][1] = 4;
		this->position[2][0] = 1;
		this->position[2][1] = 4;
		this->position[3][0] = 3;
		this->position[3][1] = 4;

		break;

	case 4:

		if (this->grid[0][5] == 1 || this->grid[1][5] == 1 || this->grid[1][4] == 1 || this->grid[2][4] == 1) {
			result = true;
		}
		this->grid[0][5] = 2;
		this->grid[1][5] = 2;
		this->grid[1][4] = 2;
		this->grid[2][4] = 2;
		this->position[0][0] = 1;
		this->position[0][1] = 5;
		this->position[1][0] = 0;
		this->position[1][1] = 5;
		this->position[2][0] = 1;
		this->position[2][1] = 4;
		this->position[3][0] = 2;
		this->position[3][1] = 4;
		break;

	case 5:

	if (this->grid[0][4] == 1 || this->grid[1][4] == 1 || this->grid[1][5] == 1 || this->grid[2][5] == 1) {
		result = true;
	}
		this->grid[0][4] = 2;
		this->grid[1][4] = 2;
		this->grid[1][5] = 2;
		this->grid[2][5] = 2;
		this->position[0][0] = 1;
		this->position[0][1] = 4;
		this->position[1][0] = 0;
		this->position[1][1] = 4;
		this->position[2][0] = 1;
		this->position[2][1] = 5;
		this->position[3][0] = 2;
		this->position[3][1] = 5;

		break;

	case 6:
		if (this->grid[0][4] == 1 || this->grid[1][4] == 1 || this->grid[2][4] == 1 || this->grid[1][5] == 1) {
			result = true;
		}
		this->grid[0][4] = 2;
		this->grid[1][4] = 2;
		this->grid[2][4] = 2;
		this->grid[1][5] = 2;
		this->position[0][0] = 1;
		this->position[0][1] = 4;
		this->position[1][0] = 0;
		this->position[1][1] = 4;
		this->position[2][0] = 2;
		this->position[2][1] = 4;
		this->position[3][0] = 1;
		this->position[3][1] = 5;

		break;
	}
	this->pool_index += 1;
	if (this->pool_index == 6) {
		this->make_pool();
	}
	return result;
}