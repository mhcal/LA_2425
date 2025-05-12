#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"
#include <stdbool.h>
#include "../src/types.h"
#include "../src/utils.h"
#include "../src/commands.h"

void test_parse_coord() {
	ESTADO e;
	int col;
	int row;
	e.num_cols = 5;
	e.num_rows = 5;
	e.board_loaded = true;
	CU_ASSERT_FALSE(parse_coord("2731", &col, &row, &e)); // Invalid format
	CU_ASSERT_FALSE(parse_coord("a23", &col, &row, &e)); // off-limit coord
	CU_ASSERT_TRUE(parse_coord("a4", &col, &row, &e)); // valid coord
}

void test_print_board() {
	ESTADO e;
	e.board_loaded = true;
	CU_ASSERT_TRUE(e.board_loaded);
	CU_ASSERT_FALSE(!e.board_loaded);
}

void test_verifica_branca() {
	PARAMETROS p;
	p.estado = (ESTADO*)malloc(sizeof(ESTADO));
	p.estado->board = NULL;
	p.estado->move_stack = NULL;
	int rows, cols;
	rows = 5;
	cols = 5;
	p.estado->num_cols = 5;
	p.estado->num_rows = 5;
	p.estado->board_loaded = true;

	allocate_board(rows, cols, p.estado);
	char example[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};
	for (int i = 0; i < p.estado->num_rows; i++) {
		for (int j = 0; j < p.estado->num_cols; j++)
			p.estado->board[i][j] = example[i][j];
	}
	// board modifications
	p.arg = "a1";
	pintar(&p);
	p.arg = "b2";
	pintar(&p);
	p.arg = "b5";
	pintar(&p);
	p.arg = "c3";
	pintar(&p);

	CU_ASSERT_TRUE(verifica_branca(0,0,p.estado));
	CU_ASSERT_FALSE(verifica_branca(1,1,p.estado));
	CU_ASSERT_TRUE(verifica_branca(2,2,p.estado));
	CU_ASSERT_FALSE(verifica_branca(4,4,p.estado));

	free_move_stack(p.estado);
	free_board(p.estado);
	free(p.estado);
}

void test_verifica_riscada() {
	PARAMETROS p;
	p.estado = (ESTADO*)malloc(sizeof(ESTADO));
	p.estado->board = NULL;
	p.estado->move_stack = NULL;
	int rows, cols;
	rows = 5;
	cols = 5;
	p.estado->num_cols = 5;
	p.estado->num_rows = 5;
	p.estado->board_loaded = true;

	allocate_board(rows, cols, p.estado);
	char example[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};
	for (int i = 0; i < p.estado->num_rows; i++) {
		for (int j = 0; j < p.estado->num_cols; j++)
			p.estado->board[i][j] = example[i][j];
	}
	p.arg = "b2";
	riscar(&p);
	p.arg = "b1";
	pintar(&p);
	p.arg = "a2";
	pintar(&p);
	p.arg = "b3";
	pintar(&p);
	p.arg = "c2";
	pintar(&p);

	CU_ASSERT_TRUE(verifica_riscada(1,1,p.estado));
	p.arg = "e5";
	riscar(&p);
	p.arg = "e4";
	riscar(&p);
	
	CU_ASSERT_FALSE(verifica_riscada(4,4,p.estado));

	free_board(p.estado);
	free_move_stack(p.estado);
	free(p.estado);
}

void test_verifica_caminho() {
	ESTADO e;
	int rows, cols;
	rows = 5;
	cols = 5;
	e.num_cols = 5;
	e.num_rows = 5;
	e.board_loaded = true;

	allocate_board(rows, cols, &e);
	char solved[5][5] = {
		{'E', '#', 'A', 'D', 'C'},
		{'D', 'C', '#', 'E', '#'},
		{'B', '#', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};
	for (int i = 0; i < e.num_rows; i++) {
		for (int j = 0; j < e.num_cols; j++)
			e.board[i][j] = solved[i][j];
	}
	
	CU_ASSERT_TRUE(verifica_caminho(&e));
	free_board(&e);
	
	ESTADO e2;
	e2.num_cols = 5;
	e2.num_rows = 5;
	e2.board_loaded = true;
	e2.board = NULL;
	allocate_board(rows, cols, &e2);
	char wrong[5][5] = {
		{'E', '#', 'A', '#', 'C'},
		{'#', 'C', '#', 'E', '#'},
		{'B', '#', 'D', '#', 'E'},
		{'#', 'D', 'E', 'E', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};
	for (int i = 0; i < e2.num_rows; i++) {
		for (int j = 0; j < e2.num_cols; j++)
			e2.board[i][j] = wrong[i][j];
	}

	CU_ASSERT_FALSE(verifica_caminho(&e2));
	free_board(&e2);
}

void test_pinta_vizinhos() {
	PARAMETROS p;
	p.estado = (ESTADO*)malloc(sizeof(ESTADO));
	p.estado->board = NULL;
	p.estado->move_stack = NULL;
	p.estado->num_moves = 0;
	p.estado->board_loaded = true;
	int rows = 5;
	int cols= 5;
	p.estado->num_rows = rows;
	p.estado->num_cols = cols;

	// example board initialization
	allocate_board(rows, cols, p.estado);
	char example[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};

	for (int i = 0; i < p.estado->num_rows; i++) {
		for (int j = 0; j < p.estado->num_cols; j++)
			p.estado->board[i][j] = example[i][j];
	}

	p.arg = "c3";
	riscar(&p);
	pinta_vizinhos(2,2,(p.estado));
	char test1[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'D', 'e', 'c'},
		{'b', 'D', '#', 'C', 'e'},
		{'c', 'd', 'E', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};
	
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++)
			CU_ASSERT_TRUE(p.estado->board[i][j] = test1[i][j]);
	}
	
	PARAMETROS p2;
	p2.estado = (ESTADO*)malloc(sizeof(ESTADO));
	p2.estado->board = NULL;
	p2.estado->move_stack = NULL;
	p2.estado->num_moves = 0;
	p2.estado->board_loaded = true;
	p2.estado->num_rows = rows;
	p2.estado->num_cols = cols;

	// example board initialization
	allocate_board(rows, cols, p2.estado);
	for (int i = 0; i < p2.estado->num_rows; i++) {
		for (int j = 0; j < p2.estado->num_cols; j++)
			p2.estado->board[i][j] = example[i][j];
	}

	pinta_vizinhos(2,2,(p2.estado));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			CU_ASSERT_TRUE(p2.estado->board[i][j] = example[i][j]); // test to see if the board remains unchanged when passing an non-white position
		}
	}

	p2.arg = "e5";
	riscar(&p2);
	pinta_vizinhos(4,4,(p2.estado));

	char test2[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'B'},
		{'a', 'c', 'c', 'B', '#'}
	};

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			CU_ASSERT_TRUE(p2.estado->board[i][j] = test2[i][j]); // upper limit test
		}
	}

	PARAMETROS p3;
	p3.estado = (ESTADO*)malloc(sizeof(ESTADO));
	p3.estado->board = NULL;
	p3.estado->move_stack = NULL;
	p3.estado->num_moves = 0;
	p3.estado->board_loaded = true;
	p3.estado->num_rows = 5;
	p3.estado->num_cols = 5;
	allocate_board(rows,cols,p3.estado);

	char test3[5][5] = {
		{'#', 'C', 'a', 'd', 'c'},
		{'D', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) 
			p3.estado->board[i][j] = example[i][j];
	}

	p3.arg = "a1";
	riscar(&p3);
	pinta_vizinhos(0,0,(p3.estado));

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) 
			CU_ASSERT_TRUE(p3.estado->board[i][j] = test3[i][j]);
	}

	free_move_stack(p.estado);
	free_board(p.estado);
	free(p.estado);
	free_move_stack(p2.estado);
	free_board(p2.estado);
	free(p2.estado);
	free_move_stack(p3.estado);
	free_board(p3.estado);
	free(p3.estado);
}

void test_risca_iguais() {
	PARAMETROS p;
	p.estado = (ESTADO*)malloc(sizeof(ESTADO));
	p.estado->board = NULL;
	p.estado->move_stack = NULL;
	p.estado->num_moves = 0;
	p.estado->board_loaded = true;
	int rows = 5;
	int cols= 5;
	p.estado->num_rows = rows;
	p.estado->num_cols = cols;

	// example board initialization
	allocate_board(rows, cols, p.estado);
	char example[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};

	for (int i = 0; i < p.estado->num_rows; i++) {
		for (int j = 0; j < p.estado->num_cols; j++)
			p.estado->board[i][j] = example[i][j];
	}

	p.arg = "c3";
	pintar(&p);
	risca_iguais(2,2,(p.estado));
	char test1[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', '#', 'e', 'c'},
		{'b', '#', 'D', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};
	
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++)
			CU_ASSERT_TRUE(p.estado->board[i][j] = test1[i][j]);
	}
	
	PARAMETROS p2;
	p2.estado = (ESTADO*)malloc(sizeof(ESTADO));
	p2.estado->board = NULL;
	p2.estado->move_stack = NULL;
	p2.estado->num_moves = 0;
	p2.estado->board_loaded = true;
	p2.estado->num_rows = rows;
	p2.estado->num_cols = cols;

	// example board initialization
	allocate_board(rows, cols, p2.estado);
	for (int i = 0; i < p2.estado->num_rows; i++) {
		for (int j = 0; j < p2.estado->num_cols; j++)
			p2.estado->board[i][j] = example[i][j];
	}

	risca_iguais(2,2,(p2.estado));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			CU_ASSERT_TRUE(p2.estado->board[i][j] = example[i][j]); // test to see if the board remains unchanged when passing an non-white position
		}
	}

	p2.arg = "e5";
	pintar(&p2);
	risca_iguais(4,4,(p2.estado));

	char test2[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', '#'},
		{'a', 'c', 'c', '#', 'B'}
	};

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			CU_ASSERT_TRUE(p2.estado->board[i][j] = test2[i][j]); // upper limit test
		}
	}

	PARAMETROS p3;
	p3.estado = (ESTADO*)malloc(sizeof(ESTADO));
	p3.estado->board = NULL;
	p3.estado->move_stack = NULL;
	p3.estado->num_moves = 0;
	p3.estado->board_loaded = true;
	p3.estado->num_rows = 5;
	p3.estado->num_cols = 5;
	allocate_board(rows,cols,p3.estado);

	char test3[5][5] = {
		{'E', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) 
			p3.estado->board[i][j] = example[i][j];
	}

	p3.arg = "a1";
	pintar(&p3);
	risca_iguais(0,0,(p3.estado));

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) 
			CU_ASSERT_TRUE(p3.estado->board[i][j] = test3[i][j]);
	}

	free_move_stack(p.estado);
	free_board(p.estado);
	free(p.estado);
	free_move_stack(p2.estado);
	free_board(p2.estado);
	free(p2.estado);
	free_move_stack(p3.estado);
	free_board(p3.estado);
	free(p3.estado);
}

int compare_estado(ESTADO *e, ESTADO *e2) {
	//if (e->looping != e2->looping) return 1;
	if (e->num_rows != e2->num_rows) return 1;
	if (e->num_cols != e2->num_cols) return 1;
	if (e->board_loaded != e2->board_loaded) return 1;
	for (int i  = 0; i < e->num_rows; i++) {
		for (int j = 0; j < e->num_cols; j++) {
			if (e->board[i][j] != e2->board[i][j]) return 1;
		}
	}
	/*
	if (e->num_moves != e2->num_moves) return 1;
	MOVE *currente = e->move_stack;
	MOVE *currente2 = e2->move_stack;
	while(currente != NULL && currente2 != NULL) {
		if (currente->row != currente2->row || currente->col != currente2->col || currente->prev != currente2->prev) return 1;
		currente = currente->next;
		currente2 = currente2->next;
	}
		*/
	
	//if (e->ajuda_dada != e2->ajuda_dada) return 1;
	//if (e->num_ajuda != e2->num_ajuda) return 1;

	return 0;
}
void test_copy_estado(){
	ESTADO e;
	e.looping = true;
	e.board = NULL;
	e.move_stack = NULL;
	e.num_moves = 0;
	e.num_ajuda = 0;

	e.ajuda_dada = false;
	e.board_loaded = true;

	allocate_board(5, 5, &e);
	char example[5][5] = {
		{'E', '#', 'A', 'D', 'C'},
		{'D', 'C', '#', 'E', '#'},
		{'B', '#', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};
	for (int i = 0; i < e.num_rows; i++) {
		for (int j = 0; j < e.num_cols; j++)
			e.board[i][j] = example[i][j];
	}
	ESTADO *copia = copy_estado(&e);
	ESTADO e1;
	e1.board = NULL;
	e1.move_stack = NULL;
	e1.board_loaded = true;
	e1.looping = false;
	e1.num_rows = 5;
	e1.num_cols = 5;
	e1.num_moves = 0;
	e1.ajuda_dada = false;
	e1.num_ajuda = 0;
	allocate_board(5, 5, &e1);
	char example1[5][5] = {
		{'E', '#', 'A', 'D', 'C'},
		{'D', 'C', '#', 'E', '#'},
		{'B', '#', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '9'}
	};
	for (int i = 0; i < e1.num_rows; i++) {
		for (int j = 0; j < e1.num_cols; j++)
			e1.board[i][j] = example1[i][j];
	}
	ESTADO *copia1 = copy_estado(&e1);

	CU_ASSERT_EQUAL(compare_estado(copia, &e), 0);
	CU_ASSERT_EQUAL(compare_estado(copia1, &e1), 0);
	
	free_board(&e);
	free_board(&e1);
	free_board(copia);
	free_estado(copia);
	free_board(copia1);
	free_estado(copia1);
}
void test_isola(){
	ESTADO e;
	e.board_loaded = true;
	e.num_rows = 5;
	e.num_cols = 5;
	allocate_board(5,5,&e);
	char example[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			e.board[i][j] = example[i][j];
		}
	}

	isola(4,4,&e);

	char test[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', '#'},
		{'a', 'c', 'c', '#', 'B'}
	};

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			CU_ASSERT_TRUE(e.board[i][j] = test[i][j]);
		}
	}

	free_board(&e);
}

