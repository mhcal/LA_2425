#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "utils.h"

bool help(PARAMETROS *p) {
	(void)p;
	printf("-------------------------------- COMANDOS --------------------------------\n"
		" * g <filename>   - grava o estado atual do jogo num ficheiro\n"
		" * l <filename>   - lê o estado do jogo de um ficheiro\n"
		" * b <coordenada> - coloca a letra na coordenada em maiúscula\n"
		" * r <coordenada> - risca a letra na coordenada correspondente\n"
		" * v              - aponta as restrições violadas no estado atual\n"
		" * a              - muda as casas que podem ser inferidas no estado atual\n"
		" * A              - invoca o comando a enquanto o jogo sofrer alterações\n"
		" * R              - resolve o jogo\n"
		" * d <num>        - desfaz as últimas <num> jogadas (default = 1)\n"
		"   d ajuda        - desfaz todas jogadas desde a última invocação de a/A/R\n"
		" * h              - imprime comandos disponíveis\n"
		" * s              - sair do jogo\n"
		"--------------------------------------------------------------------------\n");
	return true;
}

bool gravar(PARAMETROS *p) {
	if (p->arg == NULL) {
		fprintf(stderr, "Erro: comando gravar precisa de um argumento.\n");
		return false;
	}
	char path[LINE_SIZE];
	snprintf(path, sizeof(path), "boards/%s", p->arg); FILE *f = fopen(path, "w");
	if (!f) {
		fprintf(stderr, "Erro: não foi possível abrir o arquivo para gravação.\n");
		return false;
	}
	fprintf(f, "%d %d\n", p->estado->num_rows, p->estado->num_cols);
	for (int i = 0; i < p->estado->num_rows; i++) {
		for (int j = 0; j < p->estado->num_cols; j++)
			fprintf(f, "%c", p->estado->board[i][j]);
		fprintf(f, "\n");
	}
	if (!p->suppress)
		printf("Gravando em %s\n", path);
	fclose(f);
	return true;
}

bool sair(PARAMETROS *p) {
	if (p->arg != NULL) {
		fprintf(stderr, "Erro: comando sair não recebe argumentos.\n");
		return false;
	}
	p->estado->looping = false;
	return true;
}

bool ler(PARAMETROS *p) {
	if (p->arg == NULL) {
		fprintf(stderr, "Erro: comando ler precisa de um argumento.\n");
		return false;
	}
	char path[LINE_SIZE];
	snprintf(path, sizeof(path), "boards/%s", p->arg);
	FILE *f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "Erro: não foi possível abrir o arquivo para leitura.\n");
		return false;
	}
	int rows, cols;
	if (fscanf(f, "%d %d\n", &rows, &cols) != 2) {
		fprintf(stderr, "Erro: formato do arquivo inválido.\n");
		fclose(f);
		return false;
	}
	if (rows >= ROW_SIZE || cols >= COL_SIZE) {
		fprintf(stderr, "Erro: tabuleiro maior do que dimensões máximas.\n");
		fclose(f);
		return false;
	}
	free_move_stack(p->estado);
	if (!allocate_board(rows, cols, p->estado)) {
		fclose(f);
		return false;
	}
	char line[cols + 2];
	for (int i = 0; i < p->estado->num_rows; i++) {
		if (!fgets(line, sizeof(line), f)) {
			fprintf(stderr, "Erro ao ler tabuleiro.\n");
			fclose(f);
			return false;
		}
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = 0;
		for (int j = 0; j < p->estado->num_cols; j++)
			p->estado->board[i][j] = line[j];
	}
	fclose(f);
	p->estado->board_loaded = true;
	if (!p->suppress)
		printf("Lendo de %s\n", path);
	return true;
}

bool pintar(PARAMETROS *p) {
	if (!p->estado->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro carregado.\n");
		return false;
	}
	if (p->arg == NULL) {
		fprintf(stderr, "Erro: comando pintar precisa de um argumento.\n");
		return false;
	}
	int col, row; 
	if (!parse_coord(p->arg, &col, &row, p->estado))
		return false;
	char current = p->estado->board[row][col];
	if (is_riscada(current)) {
		if (!p->suppress)
			printf("Letra na posição %s está riscada.\n", p->arg);
	}
	else if (is_branca(current)) {
		if (!p->suppress)
			printf("Letra na posição %s já está maiúscula.\n", p->arg);
	}
	else {
		push_move(row, col, current, p->estado);
		p->estado->board[row][col] = p->estado->board[row][col] - 32;
		if (!p->suppress)
			printf("Letra na posição %s foi colocada em maiúscula.\n", p->arg);
	}
	return true;
}

bool riscar(PARAMETROS *p) {
	if (!p->estado->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro carregado.\n");
		return false;
	}
	if (p->arg == NULL) {
		fprintf(stderr, "Erro: comando riscar precisa de um argumento.\n");
		return false;
	}
	int col, row;
	if (!parse_coord(p->arg, &col, &row, p->estado))
		return false;
	char current = p->estado->board[row][col];
	if (is_riscada(current)) {
		if (!p->suppress)
			printf("A posição %s já foi riscada.\n", p->arg);
	}
	else {
		push_move(row, col, current, p->estado);
		p->estado->board[row][col] = '#';
		if (!p->suppress)
			printf("Letra na posição %s foi riscada.\n", p->arg);
	}
	return true;
}

bool undo(PARAMETROS *p) {
	if (!p->estado->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro carregado.\n");
		return false;
	}
	if (p->estado->move_stack == NULL || p->estado->num_moves == 0) {
		fprintf(stderr, "Erro: não há jogadas para desfazer.\n");
		return false;
	}
	int passos = 1;
	if (p->arg != NULL) {
		if (strcmp(p->arg, "ajuda") == 0) {
			if (!p->estado->ajuda_dada || p->estado->num_ajuda <= 0) {
				fprintf(stderr, "Erro: nenhuma ajuda para desfazer.\n");
				return false;
			}
			passos = p->estado->num_ajuda;
			p->estado->ajuda_dada = false;
		}
		else {
			passos = atoi(p->arg);
			if (passos <= 0) {
				fprintf(stderr, "Erro: número de jogadas deve ser um inteiro positivo.\n");
				return false;
			}
		}
	}
	if (passos > p->estado->num_moves) {
		fprintf(stderr, "Erro: só existe(m) %d jogada(s) no histórico.\n", p->estado->num_moves);
		return false;
	}
	for (int i = 1; i <= passos; i++) {
		if (!p->suppress)
			printf("%2d) ", i);
		pop_move(p->estado, p->suppress);
	}
	putchar('\n');
	if (!p->suppress)
		printf("%d jogada(s) desfeita(s).\n", passos);
	return true;
}

bool verificar(PARAMETROS *p) {
	if (!p->estado->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro carregado.\n");
		return false;
	}
	if (p->arg != NULL) {
		fprintf(stderr, "Erro: comando verificar não recebe argumentos.\n");
		return false;
	}
	bool violation_found = false;
	for (int i = 0; i < p->estado->num_rows; i++) {
		for (int j = 0; j < p->estado->num_cols; j++) {
			if (is_branca(p->estado->board[i][j])) {
				if (!(verifica_branca(i, j, p->estado))) {
					if (!p->suppress)
						printf("Violação encontrada: letra '%c' na posição %c%d tem réplica pintada a branco na mesma linha ou coluna.\n", p->estado->board[i][j], 'a' + j, i + 1);
					violation_found = true;
				}
			}
			if (is_riscada(p->estado->board[i][j])) {
				if (!(verifica_riscada(i, j, p->estado))) {
					if (!p->suppress)
						printf("Violação encontrada: posição %c%d está riscada e tem vizinhos ortogonais riscados.\n", 'a' + j, i + 1);
					violation_found = true;
				}
			}
		}
	}
	if (!(verifica_caminho(p->estado))) {
		if (!p->suppress)
			printf("Violação encontrada: não existe um caminho ortogonal entre todas as casas pintadas de branco.\n");
		violation_found = true;
	}
	if (!violation_found)
		if (!p->suppress)
			printf("Nenhuma violação de regras encontrada no estado atual.\n");
	return !violation_found;
}

bool ajuda(PARAMETROS *p) {
	bool original_suppress = p->suppress;
	p->suppress = true;
	if (!p->estado->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro carregado.\n");
		return false;
	}
	if (p->arg != NULL) {
		fprintf(stderr, "Erro: comando ajuda não recebe argumentos.\n");
		return false;
	}
	if (!verificar(p)) {
		fprintf(stderr, "Erro: o tabuleiro atual é inválido. Por favor corrija antes de invocar comando ajuda.\n");
		return false;
	}
	p->estado->ajuda_dada = true;
	p->estado->num_ajuda = 0;
	int original_num_moves = p->estado->num_moves;
	ESTADO *original_state = copy_estado(p->estado);
	for (int r = 0; r < p->estado->num_rows; r++) {
		for (int c = 0; c < p->estado->num_cols; c++) {
			if (is_riscada(original_state->board[r][c])) {
				pinta_vizinhos(r, c, p->estado);
			}
			else if (is_branca(original_state->board[r][c])) {
				risca_iguais(r, c, p->estado);
			}
			else if (isola(r, c, original_state) && is_minuscula(p->estado->board[r][c])) {
				push_move(r, c, p->estado->board[r][c], p->estado);
				p->estado->board[r][c] -= 32;
			}
		}
	}
	p->estado->num_ajuda = p->estado->num_moves - original_num_moves;
	bool valido = verificar(p);
	if (!original_suppress) {
		if (!valido) {
			printf("!! violações encontradas após implementação das regras\n"
					"!! dica: podes usar 'd ajuda' para desfazer todos os passos\n\n");
			p->suppress = false;
			verificar(p);
		}
		else
			printf("Nenhuma violação encontrada após implementação das regras.\n");
	}
	free_estado(original_state);
	return valido;
}

bool ajuda_repete(PARAMETROS *p) {
	bool original_suppress = p->suppress;
	p->suppress = true;
	if (!p->estado->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro carregado.\n");
		return false;
	}
	if (p->arg != NULL) {
		fprintf(stderr, "Erro: comando ajuda não recebe argumentos.\n");
		return false;
	}
	if (!verificar(p)) {
		fprintf(stderr, "Erro: o tabuleiro atual é inválido. Por favor corrija antes de invocar comando ajuda.\n");
		return false;
	}
	int total_changes = 0;
	bool changes_made = true;
	bool result = true;
	while (changes_made && result) {
		result = ajuda(p);
		changes_made = (p->estado->num_ajuda > 0);
		total_changes += p->estado->num_ajuda;
	}
	p->estado->ajuda_dada = true;
	p->estado->num_ajuda = total_changes;
	bool valido = verificar(p);
	p->suppress = original_suppress;
	if (!original_suppress) {
		if (!valido) {
			printf("!! violações encontradas após implementação das regras\n"
					"!! dica: podes usar 'd ajuda' para desfazer todos os passos\n\n");
			p->suppress = false;
			verificar(p);
		}
		else
			printf("Nenhuma violação encontrada após implementação das regras.\n");
	}
	return valido;
}

bool resolver(PARAMETROS *p) {
	if (!p->estado->board_loaded) {
		fprintf(stderr, "Erro: nenhum tabuleiro carregado.\n");
		return false;
	}
	if (p->arg != NULL) {
		fprintf(stderr, "Erro: comando resolver não recebe argumentos.\n");
		return false;
	}
	ESTADO *copia_estado = copy_estado(p->estado);
	ESTADO *original_estado = copy_estado(p->estado);
	bool result = dfs(copia_estado);
	if (result) {
		p->estado->ajuda_dada = true;
		p->estado->num_ajuda = 0;
		for (int i = 0; i < p->estado->num_rows; i++) {
			for (int j = 0; j < p->estado->num_cols; j++) {
				if (p->estado->board[i][j] != copia_estado->board[i][j]) {
					p->estado->num_ajuda++;
					push_move(i, j, p->estado->board[i][j], p->estado);
					p->estado->board[i][j] = copia_estado->board[i][j];
				}
			}
		}
		if (!p->suppress)
			printf("Tabuleiro resolvido com sucesso!\n");
	}
	else {
		if (!p->suppress)
			printf("Não foi possível encontrar uma solução para este tabuleiro.\n");
	}
	free_estado(copia_estado);
	free_estado(original_estado);
	return result;
}
