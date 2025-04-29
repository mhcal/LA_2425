#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "../src/types.h"
#include "../src/commands.h"
#include "../src/utils.h"

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
	ESTADO e;
	e.num_rows = 5;
	e.num_cols = 5;
	e.board_loaded = true;
	e.move_stack = NULL;
	int rows = 5;
	int cols = 5;
	// example board initialization
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
	CU_ASSERT_EQUAL(system("mkdir -p boards"), 0);
	CU_ASSERT_FALSE(gravar(NULL, &e));
	char* test_filename = "test_board";
	CU_ASSERT_TRUE(gravar(test_filename, &e));
	compare_boards(test_filename, &e); // Checks if the file that gravar creates is equal to the current board
	free_board(&e);
}

void test_sair() {
	ESTADO e;
	e.looping = true;
	CU_ASSERT_TRUE(sair(NULL, &e));
	CU_ASSERT_FALSE(sair("NULL", &e));
	CU_ASSERT_FALSE(sair("jogo", &e));
	CU_ASSERT_FALSE(sair("a1", &e));
}

/*
	In this test function, the system function was used to create a boards folder
	in order to match the filepath in the definition of such function.
*/ 

void test_ler() {
	ESTADO e;
	e.num_rows = 0;
	e.num_cols = 0;
	e.board_loaded = false;
	e.move_stack = NULL;	
	
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
	
	CU_ASSERT_FALSE(ler(NULL, &e)); // null file
	CU_ASSERT_FALSE(ler("idontexist", &e)); // non existent board
	CU_ASSERT_TRUE(ler(test_filename, &e)); // valid board
	
	free_board(&e);
}

void test_pintar(){
	ESTADO e;
 	e.board_loaded = true;
	e.num_rows = 5;
	e.num_cols = 5;
	int rows = 5;
	int cols = 5;

	// example board initialization
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

	CU_ASSERT_TRUE(pintar("a1", &e));
	CU_ASSERT_TRUE(pintar("e5", &e));
	CU_ASSERT_FALSE(pintar(NULL, &e));
	CU_ASSERT_FALSE(pintar("f8", &e));
	CU_ASSERT_FALSE(pintar("a0", &e));
	free_board(&e);
}

void test_riscar(){
	ESTADO e;
 	e.board_loaded = true;
	e.num_rows = 5;
	e.num_cols = 5;
	int rows = 5;
	int cols= 5;

	// example board initialization
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

	CU_ASSERT_TRUE(riscar("a1", &e)); // limit 1 (first element)
	CU_ASSERT_TRUE(riscar("e5", &e)); // limit 2 (second element)
	CU_ASSERT_FALSE(riscar(NULL, &e)); // null argument testing
	CU_ASSERT_FALSE(riscar("f8", &e)); // off-limit bounds
	CU_ASSERT_FALSE(riscar("a0", &e)); // off-limit bounds

	free_board(&e);
}

void test_undo() {
	ESTADO e;
	e.board_loaded = true;
	int rows = 5;
	int cols= 5;
	e.num_rows = rows;
	e.num_cols = cols;

	// example board initialization
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

	// move stack initialization with 'pintar' function
	pintar("a1", &e);
	pintar("b2", &e);
	pintar("c3", &e);
	pintar("d4", &e);

	CU_ASSERT_TRUE(undo(NULL, &e)); // undo one move
	CU_ASSERT_TRUE(undo("3", &e)); // undo 3 moves
	CU_ASSERT_FALSE(undo("10", &e));

	ESTADO e2;
	e2.board_loaded = false;
	CU_ASSERT_FALSE(undo("a1", &e2)); // test with unloaded baord

	free_board(&e);
}

void test_verificar() {
	ESTADO e;
	e.num_rows = 5;
	e.num_cols = 5;
	int rows = 5;
	int cols = 5;
	e.board_loaded = false;
	CU_ASSERT_FALSE(verificar("a2",&e));
	CU_ASSERT_FALSE(verificar(NULL,&e));

	e.board_loaded = true;
	allocate_board(rows,cols,&e);

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
	// Valid board expected
	CU_ASSERT_TRUE(verificar(NULL,&e));

	char wrong1[5][5] = {
		{'E', '#', 'A', 'D', 'C'},
		{'D', 'C', '#', 'E', '#'},
		{'B', 'C', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};

	for (int i = 0; i < e.num_rows; i++) {
		for (int j = 0; j < e.num_cols; j++)
			e.board[i][j] = wrong1[i][j];
	}

	// Same letter violation expected
	CU_ASSERT_FALSE(verificar(NULL,&e));

	char wrong2[5][5] = {
		{'E', '#', 'A', 'D', 'c'},
		{'#', 'C', '#', 'E', '#'},
		{'B', '#', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};

	for (int i = 0; i < e.num_rows; i++) {
		for (int j = 0; j < e.num_cols; j++)
			e.board[i][j] = wrong2[i][j];
	}

	CU_ASSERT_FALSE(verificar(NULL,&e));

	char wrong3[5][5] = {
		{'E', '#', 'A', 'D', 'C'},
		{'#', 'C', '#', 'E', '#'},
		{'B', '#', 'D', 'C', 'E'},
		{'C', 'D', 'E', '#', 'B'},
		{'A', '#', 'C', 'B', '#'}
	};

	for (int i = 0; i < e.num_rows; i++) {
		for (int j = 0; j < e.num_cols; j++)
			e.board[i][j] = wrong3[i][j];
	}
	// Upper cases positions without an orthogonal path violation expected
	CU_ASSERT_FALSE(verificar(NULL,&e));

	free_board(&e);
}
