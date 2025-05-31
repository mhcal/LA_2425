#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "../src/types.h"
#include "../src/commands.h"
#include "../src/utils.h"
#include "helpers.h"

/* 
	Function that compares a stored board, obtained from the output of a 
	certain function with the current loaded board.
	This is used to check if the functions output matches the loaded example
	in its current state.
*/

void compare_boards(char* filename, ESTADO *e) {
	char filepath[100];
	snprintf(filepath, sizeof(filepath), "boards/%s", filename);
	FILE *f = fopen(filepath, "r");
	CU_ASSERT_PTR_NOT_NULL(f); 
	
	if (f) {
		int s_rows, s_cols;
		CU_ASSERT_EQUAL(fscanf(f,"%d %d\n",&s_rows,&s_cols), 2);
		char line[COL_SIZE + 2]; 
		for (int i = 0; i < s_rows; i++) {
			CU_ASSERT_PTR_NOT_NULL(fgets(line, sizeof(line), f));
			
			size_t len = strlen(line);
			if (len > 0 && line[len-1] == '\n')
				line[len-1] = '\0';
			
			
			for (int j = 0; j < s_cols; j++) {
				CU_ASSERT_EQUAL(line[j], e->board[i][j]);
			}
		}
		fclose(f);
	}
}

/*
	In this test function, the system function was used to create a boards folder
	in order to match the filepath in the definition of such function.
*/ 

void test_gravar() {
	PARAMETROS p;
	p_init(&p);
	int rows = 5;
	int cols = 5;
	// example board initialization
	allocate_board(rows, cols, p.estado);
	example_init(&p);

	CU_ASSERT_EQUAL(system("mkdir -p boards"), 0);
	CU_ASSERT_FALSE(gravar(&p));
	char* test_filename = "test_board";
	p.arg = test_filename;
	CU_ASSERT_TRUE(gravar(&p));
	compare_boards(test_filename, p.estado); // Checks if the file that gravar creates is equal to the current board
	free_board(p.estado);
	free(p.estado);
}

void test_sair() {
	PARAMETROS p;
	p_init(&p);
	p.estado->looping = true;
	p.arg = NULL;
	CU_ASSERT_TRUE(sair(&p));
	p.arg = "NULL";
	CU_ASSERT_FALSE(sair(&p));
	p.arg = "jogo";
	CU_ASSERT_FALSE(sair(&p));
	p.arg = "a1";
	CU_ASSERT_FALSE(sair(&p));
	free(p.estado);
}

/*
	In this test function, the system function was used to create a boards folder
	in order to match the filepath in the definition of such function.
*/ 

void test_ler() {
	PARAMETROS p;
	p_init(&p);	
	
	CU_ASSERT_EQUAL(system("mkdir -p boards"), 0);
	char* test_filename = "test_read_board";
	char filepath[100];
	snprintf(filepath, sizeof(filepath), "boards/%s", test_filename);
	
	FILE *f = fopen(filepath, "w");
	CU_ASSERT_PTR_NOT_NULL(f); 
	
	if (f) {
		fprintf(f, "5 5\n");
		fprintf(f, "ecadc\n");
		fprintf(f, "dcded\n");
		fprintf(f, "bddce\n");
		fprintf(f, "cdeeb\n");
		fprintf(f, "accbb\n");
		fclose(f);
	}
	p.arg = NULL;
	CU_ASSERT_FALSE(ler(&p)); // null file
	p.arg = "idontexist";
	CU_ASSERT_FALSE(ler(&p)); // non existent board
	p.arg = test_filename;
	CU_ASSERT_TRUE(ler(&p)); // valid board
	
	free_board(p.estado);
	free(p.estado);
}

void test_pintar(){
	PARAMETROS p;
	p_init(&p);
	p.estado->num_rows = 5;
	p.estado->num_cols = 5;
	int rows = 5;
	int cols = 5;

	// example board initialization
	p.estado->board_loaded  = true;
	allocate_board(rows, cols, p.estado);
	example_init(&p);

	p.arg = "a1";
	CU_ASSERT_TRUE(pintar(&p));
	p.arg = "e5";
	CU_ASSERT_TRUE(pintar(&p));
	p.arg = NULL;
	CU_ASSERT_FALSE(pintar(&p));
	p.arg = "f8";
	CU_ASSERT_FALSE(pintar(&p));
	p.arg = "a0";
	CU_ASSERT_FALSE(pintar(&p));
	
	free_move_stack(p.estado);
	free_board(p.estado);
	free(p.estado);
}

void test_riscar(){
	PARAMETROS p;
	p_init(&p);
	p.estado->num_rows = 5;
	p.estado->num_cols = 5;
	int rows = 5;
	int cols= 5;

	// example board initialization
	p.estado->board_loaded  = true;
	allocate_board(rows, cols, p.estado);
	example_init(&p);

	p.arg = "a1";
	CU_ASSERT_TRUE(riscar(&p)); // limit 1 (first element)
	p.arg = "e5";
	CU_ASSERT_TRUE(riscar(&p)); // limit 2 (second element)
	p.arg = NULL;
	CU_ASSERT_FALSE(riscar(&p)); // null argument testing
	p.arg = "f8";
	CU_ASSERT_FALSE(riscar(&p)); // off-limit bounds
	p.arg = "a0";
	CU_ASSERT_FALSE(riscar(&p)); // off-limit bounds

	free_board(p.estado);
	free_move_stack(p.estado);
	free(p.estado);
}

void test_undo() {
	PARAMETROS p;
	p_init(&p);
	int rows = 5;
	int cols= 5;
	p.estado->num_rows = rows;
	p.estado->num_cols = cols;

	// example board initialization
	p.estado->board_loaded  = true;
	allocate_board(rows, cols, p.estado);
	example_init(&p);

	// move stack initialization with 'pintar' function
	p.arg = "a1";
	pintar(&p);

	p.arg = "b2";
	pintar(&p);

	p.arg = "c3";
	pintar(&p);

	p.arg = "d4";
	pintar(&p);
	p.estado->num_moves = 4;
	p.arg = NULL;
	CU_ASSERT_TRUE(undo(&p)); // undo one move

	p.arg = "3";
	CU_ASSERT_TRUE(undo(&p)); // undo 3 moves

	p.arg = "10";
	CU_ASSERT_FALSE(undo(&p));

	PARAMETROS p2;
	p_init(&p2);
	CU_ASSERT_FALSE(undo(&p2)); // test with unloaded baord
	free_move_stack(p.estado);
	free_board(p.estado);
	free(p.estado);
	free(p2.estado);
}

void test_verificar() {
	PARAMETROS p;
	p_init(&p);
	p.estado->num_rows = 5;
	p.estado->num_cols = 5;
	int rows = 5;
	int cols = 5;
	p.estado->board_loaded = false;
	p.arg = "a2";
	CU_ASSERT_FALSE(verificar(&p));
	p.arg = NULL;
	CU_ASSERT_FALSE(verificar(&p));

	p.estado->board_loaded = true;
	allocate_board(rows,cols,p.estado);

	char solved[5][5] = {
		{'E', '#', 'A', 'D', 'C'},
		{'D', 'C', '#', 'E', '#'},
		{'B', '#', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};

	for (int i = 0; i < p.estado->num_rows; i++) {
		for (int j = 0; j < p.estado->num_cols; j++)
			p.estado->board[i][j] = solved[i][j];
	}
	// Valid board expected
	
	CU_ASSERT_TRUE(verificar(&p));

	char wrong1[5][5] = {
		{'E', '#', 'A', 'D', 'C'},
		{'D', 'C', '#', 'E', '#'},
		{'B', 'C', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};

	for (int i = 0; i < p.estado->num_rows; i++) {
		for (int j = 0; j < p.estado->num_cols; j++)
			p.estado->board[i][j] = wrong1[i][j];
	}

	// Same letter violation expected
	CU_ASSERT_FALSE(verificar(&p));

	char wrong2[5][5] = {
		{'E', '#', 'A', 'D', 'c'},
		{'#', 'C', '#', 'E', '#'},
		{'B', '#', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};

	for (int i = 0; i < p.estado->num_rows; i++) {
		for (int j = 0; j < p.estado->num_cols; j++)
			p.estado->board[i][j] = wrong2[i][j];
	}

	CU_ASSERT_FALSE(verificar(&p));

	char wrong3[5][5] = {
		{'E', '#', 'A', 'D', 'C'},
		{'#', 'C', '#', 'E', '#'},
		{'B', '#', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};

	for (int i = 0; i < p.estado->num_rows; i++) {
		for (int j = 0; j < p.estado->num_cols; j++)
			p.estado->board[i][j] = wrong3[i][j];
	}
	// Upper cases positions without an orthogonal path violation expected
	CU_ASSERT_FALSE(verificar(&p));

	free_board(p.estado);
	free(p.estado);
}

void test_ajuda() {
	PARAMETROS p;
	ESTADO e;
	e.board = NULL;
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
	p.estado = &e;
	p.suppress = false;
	ajuda(&p);

	CU_ASSERT_TRUE(verificar(&p));
	free_board(&e);
}

void test_ajuda_repete() {
	PARAMETROS p;
	ESTADO e;
	e.board = NULL;
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
	p.estado = &e;
	p.suppress = false;
	p.arg = "e5";
	pintar(&p);
	ajuda_repete(&p);
	char isolated[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'e'},
		{'c', 'd', 'e', 'e', '#'},
		{'a', 'c', 'c', '#', 'B'}
	};

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++)
			CU_ASSERT_TRUE(p.estado->board[i][j] = isolated[i][j]);
	}
	
	
	char deadend[5][5] = {
		{'e', 'c', 'a', 'd', 'c'},
		{'d', 'c', 'd', 'e', 'c'},
		{'b', 'd', 'd', 'c', 'E'},
		{'c', 'd', 'e', 'E', '#'},
		{'a', 'c', 'C', '#', 'B'}
	};


	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++)
			p.estado->board[i][j] = deadend[i][j];
	}

	CU_ASSERT_FALSE(ajuda_repete(&p));

	free_board(&e);
}

void test_resolver() {
	PARAMETROS p;
	ESTADO e;
	e.board = NULL;
	e.board_loaded = true;
	e.move_stack = NULL;
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
	p.estado = &e;
	p.suppress = false;
	resolver(&p);

	char solved[5][5] = {
		{'E', '#', 'A', 'D', 'C'},
		{'D', 'C', '#', 'E', '#'},
		{'B', '#', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};

	CU_ASSERT_TRUE(verificar(&p));

	for (int i = 0; i < 5; i++) {
		for(int j = 0; j < 5; j++) {
			CU_ASSERT_TRUE(p.estado->board[i][j] = solved[i][j]);
		}
	}
	free_board(&e);
}

