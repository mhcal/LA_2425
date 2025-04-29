#include <stdio.h>
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
	ESTADO e;
	int rows, cols;
	rows = 5;
	cols = 5;
	e.num_cols = 5;
	e.num_rows = 5;
	e.board_loaded = true;

	allocate_board(rows, cols, &e);
	char example[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};
	for (int i = 0; i < e.num_rows; i++) {
		for (int j = 0; j < e.num_cols; j++)
			e.board[i][j] = example[i][j];
	}
	// board modifications
	pintar("a1",&e);
	pintar("b2",&e);
	pintar("b5",&e);
	pintar("c3",&e);

	CU_ASSERT_TRUE(verifica_branca(0,0,&e));
	CU_ASSERT_FALSE(verifica_branca(1,1,&e));
	CU_ASSERT_TRUE(verifica_branca(2,2,&e));
	CU_ASSERT_FALSE(verifica_branca(4,4,&e));

	free_board(&e);
}

void test_verifica_riscada() {
	ESTADO e;
	int rows, cols;
	rows = 5;
	cols = 5;
	e.num_cols = 5;
	e.num_rows = 5;
	e.board_loaded = true;

	allocate_board(rows, cols, &e);
	char example[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', 'b'},
		{'a', 'c', 'c', 'b', 'b'}
	};
	for (int i = 0; i < e.num_rows; i++) {
		for (int j = 0; j < e.num_cols; j++)
			e.board[i][j] = example[i][j];
	}
	riscar("b2",&e);
	pintar("b1",&e);
	pintar("a2",&e);
	pintar("b3",&e);
	pintar("c2",&e);

	CU_ASSERT_TRUE(verifica_riscada(1,1,&e));

	riscar("e5",&e);
	riscar("e4",&e);
	
	CU_ASSERT_FALSE(verifica_riscada(4,4,&e));

	free_board(&e);
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
