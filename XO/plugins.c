#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string.h>
#include <math.h>

#define TIME 5
#define EXPLORATION sqrt(2)
#define DECAYING_REWARD 0.99
#define IN "Files/in.in"
#define OUT "Files/out.out"

#pragma warning(disable: 4996)

#define for9(i) for(int i=1; i<10; i++)
#define for4(i) for(int i=1; i<4; i++)
#define new(x) x = malloc(sizeof(*x))

//Framework
int Win(int mini[4][4]) {
	for (int i = 1; i<4; i++) {
		if (mini[i][1] == 10 && mini[i][2] == 10 && mini[i][3] == 10)return 10;
		if (mini[i][1] == 20 && mini[i][2] == 20 && mini[i][3] == 20)return 20;
		if (mini[1][i] == 10 && mini[2][i] == 10 && mini[3][i] == 10)return 10;
		if (mini[1][i] == 20 && mini[2][i] == 20 && mini[3][i] == 20)return 20;
	}
	if (mini[1][1] == 10 && mini[2][2] == 10 && mini[3][3] == 10)return 10;
	if (mini[3][1] == 10 && mini[2][2] == 10 && mini[1][3] == 10)return 10;
	if (mini[1][1] == 20 && mini[2][2] == 20 && mini[3][3] == 20)return 20;
	if (mini[3][1] == 20 && mini[2][2] == 20 && mini[1][3] == 20)return 20;

	return 0;
}

void BoardCpy(int mat[10][10], int mini[4][4], int play[4][4], int mat_plg[10][10], int mini_plg[4][4], int play_plg[4][4]) {
	for9(i)
	{
		for9(j)
		{
			mat_plg[i][j] = mat[i][j];
		}
	}
	for4(i)
	{
		for4(j)
		{
			mini_plg[i][j] = mini[i][j];
			play_plg[i][j] = play[i][j];
		}
	}
}

int NoMovesLeft(int mini[4][4]) {
	for (int i = 1; i<4; i++) {
		for (int j = 1; j<4; j++) {
			if (mini[i][j]<9) return 0;
		}
	}
	return 3;
}

int GameOver(int mini[4][4]) {
	int winner, draw;

	if ((winner = Win(mini)) != 0 || (draw = NoMovesLeft(mini)) == 3) {// 10-x 20-o 3-draw 0- game in progress
		if (winner != 0)return winner;
		return draw;

	}

	return 0;
}

int Row(int x) {

	return ((x - 1) / 3) + 1;
}

int Sim(int broj) {
	if (broj == 1)
		broj = 3;
	else if (broj == 3)
		broj = 1;

	return broj;
}

void NextValid(int x, int y, int play[4][4], int mini[4][4]) {

	for (int i = 1; i<4; i++) {
		for (int j = 1; j<4; j++)play[i][j] = 0;
	}
	int row = y % 3, column = x % 3;

	if (row == 0)row = 3;
	else if (row == 1)row = 1;
	else row = 2;

	if (column == 0)
		column = 3;
	else if (column == 1)
		column = 1;
	else
		column = 2;

	if (mini[row][column]<9) {
		play[row][column] = 1;
	}
	else if (mini[Sim(row)][Sim(column)]<9) {
		play[Sim(row)][Sim(column)] = 1;
	}
	else {
		for (int i = 1; i < 4; i++) {
			for (int j = 1; j < 4; j++) {
				if (mini[i][j] < 9)
					play[i][j] = 1;
			}
		}
	}
}

int IsValid(int x, int y, int play[4][4], int mat[10][10]) {

	int row = ((y - 1) / 3) + 1, column = ((x - 1) / 3) + 1;

	if (play[row][column] == 1 && mat[y][x] == 0)
		return 1;
	else
		return 0;
}

int PlayMove(int znak, int play[4][4], int mat[10][10], int mini[4][4], int com, SDL_Window  *window, SDL_Renderer * renderer, int mode,int cnt, struct node * root);

int Simulate(int igrac, int mat[10][10], int mini[4][4], int play[4][4]) {

	int game;

	while ((game = GameOver(mini)) == 0) {

		PlayMove(igrac, play, mat, mini, 1, NULL, NULL, 0, 0, NULL);
		if (igrac == 10)igrac = 20;
		else igrac = 10;
	}

	return game;
}

void Update(int x, int y, int znak, int play[4][4], int mat[10][10], int mini[4][4]) {
	mat[y][x] = znak;


	int row = Row(y);
	int column = Row(x);
	mini[row][column] += 1;

	int minicopy[4][4];
	for (int i = 1; i<4; i++) {
		for (int j = 1; j<4; j++) {
			minicopy[i][j] = mat[(row - 1) * 3 + i][(column - 1) * 3 + j];
		}
	}
	int lokalwin = Win(minicopy);
	if (lokalwin == 10)mini[row][column] = 10;
	if (lokalwin == 20)mini[row][column] = 20;

	NextValid(x, y, play, mini);
}

void Hint(int igrac, int play[4][4], int mat[10][10], int mini[4][4], SDL_Window  *window, SDL_Renderer * renderer);

void Save(int igrac, int play[4][4], int mat[10][10], int mini[4][4], int mode, int cnt) {
	FILE * save = fopen("Files/save.txt", "w");

	fprintf(save, "%d ", cnt);
	fprintf(save, "%d ", mode);
	fprintf(save, "%d ", igrac);

	int i, j;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			fprintf(save, "%d ", mat[i][j]);
		}
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			fprintf(save, "%d ", play[i][j]);
			fprintf(save, "%d ", mini[i][j]);
		}
	}

	fclose(save);
}

struct node {
	int x;
	int y;

	int player;

	int end;

	double t;
	int n;
	struct node *sons[81];
	struct node *father;
};

int PlayMove(int znak, int play[4][4], int mat[10][10], int mini[4][4], int com, SDL_Window  *window, SDL_Renderer * renderer, int mode, int cnt, struct node * root) {

	int x, y;

	//Random
	if (com == 1)
	{
		int b = 1;
		while (b) {
			y = rand() % 9 + 1;
			x = rand() % 9 + 1;
			if (IsValid(x, y, play, mat)) {
				b = 0;
			}
		}
	}

	//Monte Carlo
	else if (com == 2 || com == 4)
	{
		int no_simulations = (int)pow(10, com);

		int mat_plg[10][10], mini_plg[4][4], play_plg[4][4];

		double success_rate, cur_sr, success_cnt;
		int best_x, best_y;
		cur_sr = -no_simulations - 3;
		best_x = -1;
		best_y = -1;
		for (int ii = 0; ii<3; ii++)
			for (int jj = 0; jj<3; jj++)
			{
				if (play[ii + 1][jj + 1] == 1)
				{
					for (int y_crd = ii * 3 + 1; y_crd <= (ii + 1) * 3; y_crd++)
						for (int x_crd = jj * 3 + 1; x_crd <= (jj + 1) * 3; x_crd++)
						{
							success_rate = 0;

							if (mat[y_crd][x_crd] == 0)
							{


								if (GameOver(mini_plg) == znak)
									success_rate = no_simulations + 1;
								else
									for (int sim = 0; sim<no_simulations; sim++)
									{
										BoardCpy(mat, mini, play, mat_plg, mini_plg, play_plg);
										Update(x_crd, y_crd, znak, play_plg, mat_plg, mini_plg);

										success_cnt = Simulate(znak == 10 ? 20 : 10, mat_plg, mini_plg, play_plg);
										if (success_cnt == znak)success_rate++;
										else if (success_cnt>9)success_rate--;
									}

								success_rate /= no_simulations;
								if (success_rate > cur_sr)
								{
									cur_sr = success_rate;
									best_x = x_crd;
									best_y = y_crd;
								}

							}
						}
				}
			}

		x = best_x;
		y = best_y;

	}
	//Player
	else
	{
		SDL_Event windowEvent;

		while (1) {

			if (SDL_PollEvent(&windowEvent)) {
				if (SDL_QUIT == windowEvent.type) {
					return 0;
				}
				else if (SDL_MOUSEBUTTONDOWN == windowEvent.type) {
					x = windowEvent.motion.x / 100 + 1;
					y = windowEvent.motion.y / 100 + 1;

					if (IsValid(x, y, play, mat)) {
						if (root) {
							root->x = x;
							root->y = y;
						}
						break;
					}
				}
				else if (SDL_KEYDOWN == windowEvent.type) {
					if (windowEvent.key.keysym.sym == SDLK_h) {
						Hint(znak, play, mat, mini, window, renderer);
					}
					else if (windowEvent.key.keysym.sym == SDLK_s) {
						Save(znak, play, mat, mini, mode, cnt);
						return 0;
					}
				}
			}

		}
	}


	Update(x, y, znak, play, mat, mini);

	return 1;
}


//Monte Carlo Tree Search UCT

struct node * CreateRoot(int player) {

	struct node * root;

	new(root);

	root->n = 0;

	root->player = player;

	root->father = NULL;

	for (int i = 0; i < 81; root->sons[i++] = NULL);

	return root;
}

void FreeNodes(struct node * curr, struct node *next) {

	if (curr == NULL || curr == next)
		return;

	for (int i = 0; i < 81; i++) FreeNodes(curr->sons[i], next);

	free(curr);
}

void Rollout(struct node *node, int player, int mat[10][10], int mini[4][4], int play[4][4]) {

	double success_rate;
	int success_cnt;

	success_rate = 0;
	success_cnt = 0;

	success_cnt = Simulate(node->player == 10 ? 20 : 10, mat, mini, play);
	if (success_cnt == player)success_rate++;
	else if (success_cnt == 3)success_rate += 0.5;


	node->t = success_rate;
	node->n = 1;
}

double UCB1(int sgn, struct node *node) {
	return node->t / node->n + sgn * EXPLORATION*sqrt(log(node->father->n) / node->n);
}

int IsLeafNode(struct node *node) {
	if (node->n == 0 || node->n == 1 || node->end) {
		return 1;
	}

	return 0;
}

struct node* MaxUCB1(struct node* sons[81]) {
	int i;
	double max = -2;
	struct node * son = NULL;

	for (i = 0; i < 81; i++) {
		if (sons[i] == NULL) continue;
		if (sons[i]->n == 0) return sons[i];

		if (UCB1(1, sons[i]) > max) {
			max = UCB1(1, sons[i]);
			son = sons[i];
		}
	}

	return son;
}

struct node* MinUCB1(struct node* sons[81]) {
	int i;
	double min = 2;

	struct node * son = NULL;

	for (i = 0; i < 81; i++) {
		if (sons[i] == NULL) continue;
		if (sons[i]->n == 0) return sons[i];

		if (UCB1(-1, sons[i])< min) {
			min = UCB1(-1, sons[i]);
			son = sons[i];
		}

	}

	if (son == NULL) {
		for (i = 0; i < 81; i++) {
			if (sons[i] != NULL) return sons[i];
		}
	}

	return son;
}

struct node * PickNode(struct node *sons[81]) {
	int i;
	double max = -2;
	struct node * son = NULL;

	for (i = 0; i < 81; i++) {
		if (sons[i] == NULL || sons[i]->n == 0) continue;

		if (sons[i]->t / sons[i]->n > max) {
			max = sons[i]->t / sons[i]->n;
			son = sons[i];
		}
		else if (sons[i]->t / sons[i]->n == max && sons[i]->n > son->n) {
			max = sons[i]->t / sons[i]->n;
			son = sons[i];
		}

	}
	if (son == NULL) {
		for (i = 0; i < 81; i++) {
			if (sons[i] != NULL) return sons[i];
		}
	}
	return son;
}

void Expand(struct node * node, int mat[10][10], int mini[4][4], int play[4][4]) {

	struct node * curr;

	int nextPlayer = node->player == 10 ? 20 : 10;

	for (int i = 0; i<3; i++)
		for (int j = 0; j<3; j++)
		{
			if (play[i + 1][j + 1] == 1)
			{
				for (int y = i * 3 + 1; y <= (i + 1) * 3; y++)
					for (int x = j * 3 + 1; x <= (j + 1) * 3; x++)
					{
						if (mat[y][x] == 0) {
							new(node->sons[y * 9 + x - 10]);
							curr = node->sons[y * 9 + x - 10];
							curr->x = x;
							curr->y = y;
							curr->father = node;
							curr->n = 0;
							curr->player = nextPlayer;

							for (int i = 0; i < 81; curr->sons[i++] = NULL);
						}
					}
			}
		}
}

struct node * MCTS(struct node * node, int player, int mat_[10][10], int mini_[4][4], int play_[4][4], int x, int y) {

	int i;
	int curr_time = (int)time(0);

	int mat[10][10], mini[4][4], play[4][4];

	struct node * temp;

	if (node == NULL) {
		node = CreateRoot(player);
	}
	else {
		i = (y - 1) * 9 + x - 1;

		if (node->sons[i] == NULL) {
			FreeNodes(node, NULL);
			node = CreateRoot(player);
		}
		else {
			temp = node;
			node = node->sons[i];
			FreeNodes(node->father, node);
			node->father = NULL;
		}
	}

	double t;

	while (time(0) - curr_time < TIME) {

		BoardCpy(mat_, mini_, play_, mat, mini, play);

		//Selection
		while (!IsLeafNode(node)) {
			if (node->player == player)
				node = MaxUCB1(node->sons);

			else
				node = MinUCB1(node->sons);

			Update(node->x, node->y, node->father->player, play, mat, mini);

		}


		//Expansion & Simulation
		if (node->n == 0) {

			if (GameOver(mini)) node->end = 1;
			else node->end = 0;

			Rollout(node, player, mat, mini, play);
		}
		else if (!node->end) {

			Expand(node, mat, mini, play);

			for (i = 0; i < 81; i++) {
				if (node->sons[i] != NULL) {
					node = node->sons[i];
					break;
				}
			}

			Update(node->x, node->y, node->father->player, play, mat, mini);

			if (GameOver(mini)) node->end = 1;
			else node->end = 0;

			Rollout(node, player, mat, mini, play);

		}
		else {
			t = node->t / node->n;
			node->t += t;
			node->n++;

			while (node->father != NULL) {
				t *= DECAYING_REWARD;
				node->father->t += t;
				node->father->n++;
				node = node->father;
			}
			continue;
		}

		//Backpropagation
		t = node->t;
		while (node->father != NULL) {
			t *= DECAYING_REWARD;
			node->father->t += t;
			node->father->n++;
			node = node->father;
		}

	}

	node = PickNode(node->sons);
	FreeNodes(node->father, node);
	node->father = NULL;
	return node;
}

void Pondering(struct node * node, int player, int mat_[10][10], int mini_[4][4], int play_[4][4]) {

	int mat[10][10], mini[4][4], play[4][4];

	BoardCpy(mat_, mini_, play_, mat, mini, play);

	double t;
	int i;

	//Selection
	while (!IsLeafNode(node)) {
		if (node->player == player)
			node = MaxUCB1(node->sons);

		else
			node = MinUCB1(node->sons);

		Update(node->x, node->y, node->father->player, play, mat, mini);

	}


	//Expansion & Simulation
	if (node->n == 0) {

		if (GameOver(mini)) node->end = 1;
		else node->end = 0;

		Rollout(node, player, mat, mini, play);
	}
	else if (!node->end) {

		Expand(node, mat, mini, play);

		for (i = 0; i < 81; i++) {
			if (node->sons[i] != NULL) {
				node = node->sons[i];
				break;
			}
		}

		Update(node->x, node->y, node->father->player, play, mat, mini);

		if (GameOver(mini)) node->end = 1;
		else node->end = 0;

		Rollout(node, player, mat, mini, play);

	}
	else {
		t = node->t / node->n;
		node->t += t;
		node->n++;

		while (node->father != NULL) {
			t *= DECAYING_REWARD;
			node->father->t += t;
			node->father->n++;
			node = node->father;
		}
		return;
	}

	//Backpropagation
	t = node->t;
	while (node->father != NULL) {
		t *= DECAYING_REWARD;
		node->father->t += t;
		node->father->n++;
		node = node->father;
	}
}


//UI
void Hint(int igrac, int play[4][4], int mat[10][10], int mini[4][4], SDL_Window  *window, SDL_Renderer * renderer) {
	struct node * root = MCTS(NULL, igrac, mat, mini, play, 0, 0);

	int x = root->x;
	int y = root->y;


	FreeNodes(root, NULL);

	SDL_Surface *hint = IMG_Load("Sprites/hint.png");

	SDL_Rect rect;

	SDL_Surface * surface = SDL_GetWindowSurface(window);

	rect.x = 100 * x - 100;
	rect.y = 100 * y - 100;

	SDL_BlitSurface(hint, NULL, surface, &rect);

	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_DestroyTexture(texture);

	int i, j;

	SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
	for (i = 1; i < 9; i++) {

		if (i % 3);
		//SDL_RenderDrawLine(renderer, 0, i * 100, 900, i * 100);
		else {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawLine(renderer, 0, i * 100, 900, i * 100);
			SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
		}

		for (j = 1; j < 9; j++) {

			if (j % 3);
			//SDL_RenderDrawLine(renderer, j * 100, 0, j * 100, 900);
			else {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawLine(renderer, j * 100, 0, j * 100, 900);
				SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
			}
		}
	}

	SDL_RenderPresent(renderer);
	SDL_FreeSurface(surface);
}

void DisplayBoard(SDL_Surface *background, SDL_Surface *x, SDL_Surface *o, SDL_Surface *playable,
	SDL_Surface *big_x, SDL_Surface *big_o, SDL_Window *window, SDL_Renderer *renderer, int board[10][10], int mini[4][4], int play[4][4]) {

	SDL_Rect rect;

	int i;
	int j;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	SDL_Surface * surface = SDL_GetWindowSurface(window);
	SDL_FillRect(surface, NULL, 0x000000);

	SDL_BlitSurface(background, NULL, surface, NULL);

	for (i = 1; i < 10; i++) {
		for (j = 1; j < 10; j++) {

			rect.x = 100 * j - 100;
			rect.y = 100 * i - 100;

			if (board[i][j] == 10) {
				SDL_BlitSurface(x, NULL, surface, &rect);
			}
			else if (board[i][j] == 20) {
				SDL_BlitSurface(o, NULL, surface, &rect);
			}

		}
	}

	for (i = 1; i < 4; i++) {
		for (j = 1; j < 4; j++) {

			rect.x = 300 * j - 300;
			rect.y = 300 * i - 300;

			if (play[i][j] == 1) {
				SDL_BlitSurface(playable, NULL, surface, &rect);
			}
			else if (mini[i][j] == 10) {
				SDL_BlitSurface(big_x, NULL, surface, &rect);
			}
			else if (mini[i][j] == 20) {
				SDL_BlitSurface(big_o, NULL, surface, &rect);
			}

		}
	}

	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_DestroyTexture(texture);

	SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
	for (i = 1; i < 9; i++) {

		if (i % 3);
		//SDL_RenderDrawLine(renderer, 0, i * 100, 900, i * 100);
		else {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawLine(renderer, 0, i * 100, 900, i * 100);
			SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
		}

		for (j = 1; j < 9; j++) {

			if (j % 3);
			//SDL_RenderDrawLine(renderer, j * 100, 0, j * 100, 900);
			else {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawLine(renderer, j * 100, 0, j * 100, 900);
				SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
			}
		}
	}

	SDL_RenderPresent(renderer);
	SDL_FreeSurface(surface);
}

void Crypt(int a, char * loc) {
	FILE * temp = fopen("Files/temp.txt", "w");

	FILE * highscores = fopen(loc, "r");

	if (!highscores) {
		highscores = fopen(loc, "w");
		fclose(highscores);
		fclose(temp);
		return;
	}

	char c;

	while ((c = fgetc(highscores)) != EOF) {
		fprintf(temp, "%c", c);
	}

	fclose(temp);
	fclose(highscores);

	temp = fopen("Files/temp.txt", "r");

	highscores = fopen(loc, "w");

	a *= 50;

	while ((c = fgetc(temp)) != EOF) {
		c += a;
		fprintf(highscores, "%c", c);
	}

	fclose(temp);
	fclose(highscores);

	temp = fopen("Files/temp.txt", "w");
	fclose(temp);
}

void SortScores(char * loc) {

	FILE *scores;
	scores = fopen(loc, "r");

	char table[255][255];
	int i = 0;
	while (1)
	{
		if (fgets(table[i], 255, scores) == NULL)break;
		i++;
	}
	fclose(scores);
	qsort(table, i, sizeof(table[0]), strcmp);
	scores = fopen(loc, "w");
	for (int k = 0; k<(i<10 ? i : 10); k++)
	{
		fprintf(scores, table[k]);
	}
	fclose(scores);

}

int Highscores(SDL_Window *window, SDL_Renderer *renderer, int mode) {

	SDL_Surface *background;
	SDL_Rect rect;

	rect.x = 50;
	rect.y = 0;
	rect.h = 50;
	rect.w = 100;

	FILE *table;



	char * loc;

	switch (mode) {
	case 9:
		loc = "Files/highscores1.txt";
		break;
	case 13:
		loc = "Files/highscores2.txt";
		break;
	default:
		loc = "Files/highscores3.txt";
		break;
	}

	Crypt(-1, loc);
	SortScores(loc);

	table = fopen(loc, "r");
	char line[255];

	background = IMG_Load("Sprites/Background.png");
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_Surface * surface = SDL_GetWindowSurface(window);
	SDL_FillRect(surface, NULL, 0x000000);
	SDL_BlitSurface(background, NULL, surface, NULL);

	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_DestroyTexture(texture);

	SDL_Event windowEvent;

	SDL_Surface *message = NULL;
	TTF_Font *font = NULL;
	SDL_Color textColor = { 255, 255, 255 };


	if (TTF_Init() == -1)
	{
		SDL_FreeSurface(background);
		return 0;
	}


	font = TTF_OpenFont("Fonts/segoepr.ttf", 70);
	message = TTF_RenderText_Solid(font, "Highscores", textColor);

	TTF_CloseFont(font);


	SDL_Texture * txt = SDL_CreateTextureFromSurface(renderer, message);
	SDL_BlitSurface(message, NULL, surface, &rect);
	SDL_FreeSurface(message);
	SDL_RenderCopy(renderer, txt, NULL, &rect);

	SDL_DestroyTexture(txt);


	font = TTF_OpenFont("Fonts/segoepr.ttf", 40);
	rect.y += 85;
	while (1)
	{
		rect.y += 50;
		if (fgets(line, sizeof(line), table) == NULL)break;
		line[strlen(line) - 1] = '\0';
		message = TTF_RenderText_Solid(font, line, textColor);

		SDL_Texture * txt = SDL_CreateTextureFromSurface(renderer, message);
		SDL_BlitSurface(message, NULL, surface, &rect);


		SDL_RenderCopy(renderer, txt, NULL, &rect);

		SDL_FreeSurface(message);
		SDL_DestroyTexture(txt);
	}
	fclose(table);


	SDL_RenderPresent(renderer);

	Crypt(1, loc);

	while (1) {

		if (SDL_PollEvent(&windowEvent)) {
			if (SDL_QUIT == windowEvent.type) {
				TTF_CloseFont(font);
				SDL_FreeSurface(surface);
				SDL_FreeSurface(background);
				return 1;
			}
			else if (SDL_MOUSEBUTTONDOWN == windowEvent.type) {

				break;
			}
		}
	}

	TTF_CloseFont(font);
	SDL_FreeSurface(surface);
	SDL_FreeSurface(background);
	return 0;
}

int Menu(SDL_Window *window, SDL_Renderer *renderer) {


	SDL_Surface * menu, *menu2, *menu3;

	menu = IMG_Load("Sprites/Menu.png");
	menu2 = IMG_Load("Sprites/Menu2.png");
	menu3 = IMG_Load("Sprites/Menu3.png");

	if (menu == NULL || menu2 == NULL || menu3 == NULL) {
		printf(SDL_GetError());
		return 0;
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_Surface * surface = SDL_GetWindowSurface(window);

	SDL_BlitSurface(menu, NULL, surface, NULL);

	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_DestroyTexture(texture);

	SDL_RenderPresent(renderer);

	int mode = 0;

	SDL_Event windowEvent;

	while (1) {

		if (SDL_PollEvent(&windowEvent)) {
			if (SDL_QUIT == windowEvent.type) {
				mode = 0;
				break;
			}
			else if (SDL_MOUSEBUTTONDOWN == windowEvent.type) {

				if (windowEvent.motion.x > 200 && windowEvent.motion.x < 700) {
					if (windowEvent.motion.y > 400 && windowEvent.motion.y < 500) {
						mode = 1;
						break;
					}
					else if (windowEvent.motion.y > 500 && windowEvent.motion.y < 600) {
						mode = 2;
						break;
					}
					else if (windowEvent.motion.y > 600 && windowEvent.motion.y < 700) {
						mode = 4;
						break;
					}
					else if (windowEvent.motion.y > 700 && windowEvent.motion.y < 800) {
						mode = 0;
						break;
					}
					else if (windowEvent.motion.y > 800 && windowEvent.motion.y < 900) {
						if (windowEvent.motion.x > 250 && windowEvent.motion.x < 400)
						{
							mode = 9;
							break;
						}
						else if (windowEvent.motion.x > 500 && windowEvent.motion.x < 650)
						{
							mode = 12;
							break;
						}
					}

				}
			}
			else if (SDL_KEYDOWN == windowEvent.type) {
				if (windowEvent.key.keysym.sym == SDLK_b) {
					mode = 10;
					break;
				}
			}
		}
	}

	if (mode == 2) {
		//menu2 who goes first
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_BlitSurface(menu2, NULL, surface, NULL);

		texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_RenderCopy(renderer, texture, NULL, NULL);

		SDL_DestroyTexture(texture);

		SDL_RenderPresent(renderer);
		while (1) {

			if (SDL_PollEvent(&windowEvent)) {
				if (SDL_QUIT == windowEvent.type) {
					mode = 0;
					break;
				}
				else if (SDL_MOUSEBUTTONDOWN == windowEvent.type) {

					if (windowEvent.motion.x > 200 && windowEvent.motion.x < 700) {
						if (windowEvent.motion.y > 400 && windowEvent.motion.y < 500) {
							mode = 2;
							break;
						}
						else if (windowEvent.motion.y > 600 && windowEvent.motion.y < 700) {
							mode = 3;
							break;
						}

					}
				}
			}
		}
		//menu3 difficulty
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_BlitSurface(menu3, NULL, surface, NULL);

		texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_RenderCopy(renderer, texture, NULL, NULL);

		SDL_DestroyTexture(texture);

		SDL_RenderPresent(renderer);
		if (mode) {
			while (1) {

				if (SDL_PollEvent(&windowEvent)) {
					if (SDL_QUIT == windowEvent.type) {
						mode = 0;
						break;
					}
					else if (SDL_MOUSEBUTTONDOWN == windowEvent.type) {

						if (windowEvent.motion.x > 200 && windowEvent.motion.x < 700) {
							if (windowEvent.motion.y > 400 && windowEvent.motion.y < 500) {
								mode += 3;
								break;
							}
							else if (windowEvent.motion.y > 500 && windowEvent.motion.y < 600) {
								mode += 5;
								break;
							}
							else if (windowEvent.motion.y > 600 && windowEvent.motion.y < 700) {
								break;
							}

						}
					}
				}
			}
		}
	}
	else if (mode == 10)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_BlitSurface(menu2, NULL, surface, NULL);

		texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_RenderCopy(renderer, texture, NULL, NULL);

		SDL_DestroyTexture(texture);

		SDL_RenderPresent(renderer);
		while (1) {

			if (SDL_PollEvent(&windowEvent)) {
				if (SDL_QUIT == windowEvent.type) {
					mode = 0;
					break;
				}
				else if (SDL_MOUSEBUTTONDOWN == windowEvent.type) {

					if (windowEvent.motion.x > 200 && windowEvent.motion.x < 700) {
						if (windowEvent.motion.y > 400 && windowEvent.motion.y < 500) {
							mode = 10;
							break;
						}
						else if (windowEvent.motion.y > 600 && windowEvent.motion.y < 700) {
							mode = 11;
							break;
						}

					}
				}


			}
		}
	}

	else if (mode == 9) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_BlitSurface(menu3, NULL, surface, NULL);

		texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_RenderCopy(renderer, texture, NULL, NULL);

		SDL_DestroyTexture(texture);

		SDL_RenderPresent(renderer);

		while (1) {

			if (SDL_PollEvent(&windowEvent)) {
				if (SDL_QUIT == windowEvent.type) {
					mode = 0;
					break;
				}
				else if (SDL_MOUSEBUTTONDOWN == windowEvent.type) {

					if (windowEvent.motion.x > 200 && windowEvent.motion.x < 700) {
						if (windowEvent.motion.y > 400 && windowEvent.motion.y < 500) {
							break;
						}
						else if (windowEvent.motion.y > 500 && windowEvent.motion.y < 600) {
							mode += 4;
							break;
						}
						else if (windowEvent.motion.y > 600 && windowEvent.motion.y < 700) {
							mode += 5;
							break;
						}

					}
				}
			}
		}

	}

	SDL_FreeSurface(surface);
	SDL_FreeSurface(menu);
	SDL_FreeSurface(menu2);
	SDL_FreeSurface(menu3);

	return mode;
}

int DisplayResult(SDL_Window *window, SDL_Renderer *renderer, int game) {
	SDL_Surface * x, *o, *draw;

	SDL_Rect rect;

	x = IMG_Load("Sprites/xwins.png");
	o = IMG_Load("Sprites/owins.png");
	draw = IMG_Load("Sprites/draw.png");

	if (x == NULL || o == NULL || draw == NULL) {
		printf(SDL_GetError());
		return EXIT_FAILURE;
	}

	rect.x = 200;
	rect.y = 300;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	SDL_Surface * surface = SDL_GetWindowSurface(window);

	switch (game) {
	case 10:
		SDL_BlitSurface(x, NULL, surface, &rect);
		break;
	case 20:
		SDL_BlitSurface(o, NULL, surface, &rect);
		break;
	case 3:
		SDL_BlitSurface(draw, NULL, surface, &rect);
		break;
	}

	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_DestroyTexture(texture);

	SDL_RenderPresent(renderer);
	SDL_FreeSurface(surface);
	SDL_FreeSurface(x);
	SDL_FreeSurface(o);
	SDL_FreeSurface(draw);

	return 0;
}

int Game(SDL_Window *window, SDL_Renderer* renderer, int mode) {

	if (mode == 9 || mode > 12)
	{
		if (Highscores(window, renderer, mode))
			return 1;
		else
			return 0;
	}

	int mat[10][10] = { 0 }, mini[4][4] = { 0 }, play[4][4] = { 0 };

	int i;
	int j;
	int xc = 0, yc = 0;
	FILE *in, *out;

	SDL_Surface *x = NULL;
	SDL_Surface *o = NULL;
	SDL_Surface *playable = NULL;
	SDL_Surface *big_x = NULL;
	SDL_Surface *big_o = NULL;
	SDL_Surface *background = NULL;

	background = IMG_Load("Sprites/Background.png");
	x = IMG_Load("Sprites/X.png");
	o = IMG_Load("Sprites/O.png");
	big_x = IMG_Load("Sprites/big_X.png");
	big_o = IMG_Load("Sprites/big_O.png");
	playable = IMG_Load("Sprites/playable.png");

	if (x == NULL || o == NULL || background == NULL || playable == NULL || big_x == NULL || big_o == NULL) {
		printf(SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Event windowEvent;

	for (i = 1; i<4; i++) {
		for (j = 1; j<4; j++)play[i][j] = 1;
	}

	int game;
	int igrac = 10;
	int cnt = 0;
	char str[255];
	FILE *highscores;
	time_t date;
	int bul = 0;

	if (mode == 12) {
		FILE * save = fopen("Files/save.txt", "r");

		if (!save) mode = 0;
		else {
			fscanf(save, "%d ", &cnt);
			fscanf(save, "%d ", &mode);
			fscanf(save, "%d ", &igrac);


			for (i = 0; i < 10; i++) {
				for (j = 0; j < 10; j++) {
					fscanf(save, "%d ", &mat[i][j]);
				}
			}
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					fscanf(save, "%d ", &play[i][j]);
					fscanf(save, "%d ", &mini[i][j]);
				}
			}

			fclose(save);


		}
	}

	struct node *root = NULL;

	switch (mode) {
	case 1:
		//PvP
		DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);
		while ((game = GameOver(mini)) == 0) {

			if (SDL_PollEvent(&windowEvent))
				if (SDL_QUIT == windowEvent.type) {
					break;
				}

			if (!PlayMove(igrac, play, mat, mini, 0, window, renderer, mode, cnt, NULL)) {
				break;
			}

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			if (igrac == 10)igrac = 20;
			else igrac = 10;
		}
		break;
	case 2:
		//PvAi HARD
		DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);


		while ((game = GameOver(mini)) == 0) {

			if (SDL_PollEvent(&windowEvent))
				if (SDL_QUIT == windowEvent.type) {
					break;
				}

			if (!PlayMove(igrac, play, mat, mini, 0, window, renderer, mode, cnt, root)) {
				break;
			}
			cnt++;

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			igrac = 20;

			if (game = GameOver(mini)) break;

			root = MCTS(root, igrac, mat, mini, play, root ? root->x : 0, root ? root->y : 0);
			Update(root->x, root->y, igrac, play, mat, mini);

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			igrac = 10;

		}
		if (GameOver(mini) == 10)
		{
			bul = 1;
		}

		FreeNodes(root, NULL);

		break;
	case 3:
		//AivP HARD
		DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

		if (igrac == 20) {
			if (!PlayMove(igrac, play, mat, mini, 0, window, renderer, mode, cnt, root)) {
				break;
			}

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);
			igrac = 10;
			cnt++;
		}

		while ((game = GameOver(mini)) == 0) {

			if (SDL_PollEvent(&windowEvent))
				if (SDL_QUIT == windowEvent.type) {
					break;
				}


			root = MCTS(root, igrac, mat, mini, play, root ? root->x : 0, root ? root->y : 0);
			Update(root->x, root->y, igrac, play, mat, mini);

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			if (game = GameOver(mini)) break;

			igrac = 20;

			if (!PlayMove(igrac, play, mat, mini, 0, window, renderer, mode, cnt, root)) {
				break;
			}

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);
			igrac = 10;
			cnt++;
		}
		if (GameOver(mini) == 20)
		{
			bul = 1;
		}

		FreeNodes(root, NULL);
		break;
	case 4:
		//AivAi
		DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);
		struct node *root_o = NULL;
		struct node *root_x = NULL;

		while ((game = GameOver(mini)) == 0) {

			if (SDL_PollEvent(&windowEvent))
				if (SDL_QUIT == windowEvent.type) {
					break;
				}

			root_x = MCTS(root_x, igrac, mat, mini, play, root_o ? root_o->x : 0, root_o ? root_o->y : 0);
			Update(root_x->x, root_x->y, igrac, play, mat, mini);


			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			igrac = 20;
			if (game = GameOver(mini)) break;

			root_o = MCTS(root_o, igrac, mat, mini, play, root_x->x, root_x->y);
			Update(root_o->x, root_o->y, igrac, play, mat, mini);


			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			igrac = 10;


		}

		FreeNodes(root_o, NULL);
		FreeNodes(root_x, NULL);
		break;
	case 10:
		//AivText HARD
		DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

		in = fopen(IN, "w");
		fclose(in);

		while ((game = GameOver(mini)) == 0) {

			if (SDL_PollEvent(&windowEvent))
				if (SDL_QUIT == windowEvent.type) {
					break;
				}


			root = MCTS(root, igrac, mat, mini, play, xc, yc);
			Update(root->x, root->y, igrac, play, mat, mini);
			out = fopen(OUT, "w");
			fprintf(out, "%d %d", root->x, root->y);
			fclose(out);

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			if (game = GameOver(mini)) break;

			igrac = 20;

			xc = 0;
			while (xc == 0)
			{

				Pondering(root, 10, mat, mini, play);

				if (SDL_PollEvent(&windowEvent))
					if (SDL_QUIT == windowEvent.type) {
						FreeNodes(root, NULL);

						SDL_FreeSurface(x);
						SDL_FreeSurface(o);
						SDL_FreeSurface(playable);
						SDL_FreeSurface(big_x);
						SDL_FreeSurface(big_o);
						SDL_FreeSurface(background);

						out = fopen(OUT, "w");
						fclose(out);

						in = fopen(IN, "w");
						fclose(in);

						return 1;
					}

				in = fopen(IN, "r");
				fscanf(in, "%d %d", &xc, &yc);
				fclose(in);
			}

			in = fopen(IN, "w");
			fclose(in);

			Update(xc, yc, igrac, play, mat, mini);

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);
			igrac = 10;
		}

		FreeNodes(root, NULL);
		break;
	case 11:
		//TextvAi HARD
		DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

		in = fopen(IN, "r");

		if (!in) in = fopen(IN, "w");

		fclose(in);

		while ((game = GameOver(mini)) == 0) {

			if (SDL_PollEvent(&windowEvent))
				if (SDL_QUIT == windowEvent.type) {
					break;
				}

			xc = 0;
			while (xc == 0)
			{

				if (root) Pondering(root, 20, mat, mini, play);

				if (SDL_PollEvent(&windowEvent))
					if (SDL_QUIT == windowEvent.type) {
						FreeNodes(root, NULL);

						SDL_FreeSurface(x);
						SDL_FreeSurface(o);
						SDL_FreeSurface(playable);
						SDL_FreeSurface(big_x);
						SDL_FreeSurface(big_o);
						SDL_FreeSurface(background);

						out = fopen(OUT, "w");
						fclose(out);

						in = fopen(IN, "w");
						fclose(in);

						return 1;
					}

				in = fopen(IN, "r");
				fscanf(in, "%d %d", &xc, &yc);
				fclose(in);
			}

			in = fopen(IN, "w");
			fclose(in);

			Update(xc, yc, igrac, play, mat, mini);

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			igrac = 20;

			if (game = GameOver(mini)) break;

			root = MCTS(root, igrac, mat, mini, play, xc, yc);
			Update(root->x, root->y, igrac, play, mat, mini);
			out = fopen(OUT, "w");
			fprintf(out, "%d %d", root->x, root->y);
			fclose(out);

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			igrac = 10;

		}

		FreeNodes(root, NULL);

		break;

	default:
		//PvAi MEDIUM & EASY
		if (!mode) break;


		int A = (mode % 2) ? 0 : 2;
		int B = (mode % 2) ? 2 : 0;

		if (mode > 6) {
			A *= 2;
			B *= 2;
		}

		DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);
		while ((game = GameOver(mini)) == 0) {

			if (SDL_PollEvent(&windowEvent))
				if (SDL_QUIT == windowEvent.type) {
					break;
				}

			if (!PlayMove(igrac, play, mat, mini, igrac == 10 ? A : B, window, renderer, mode, cnt, NULL)) {
				break;
			}

			DisplayBoard(background, x, o, playable, big_x, big_o, window, renderer, mat, mini, play);

			cnt++;
			if (igrac == 10)igrac = 20;
			else igrac = 10;
		}
		cnt /= 2;
		if (GameOver(mini) == 10 && A == 0)
		{
			cnt++;
			bul = 1;
		}
		else if (GameOver(mini) == 20 && B == 0)
		{
			bul = 1;
		}
		break;
	}

	int exit = 0;

	if (game) {
		if (DisplayResult(window, renderer, game)) {
			SDL_FreeSurface(x);
			SDL_FreeSurface(o);
			SDL_FreeSurface(playable);
			SDL_FreeSurface(big_x);
			SDL_FreeSurface(big_o);
			SDL_FreeSurface(background);

			return 1;
		}

		while (1) {
			if (SDL_PollEvent(&windowEvent))
			{
				if (SDL_QUIT == windowEvent.type) {
					exit = 1;
					break;
				}
				if (SDL_MOUSEBUTTONDOWN == windowEvent.type) {
					exit = 0;
					if (bul)
					{
						char * loc;

						switch (mode) {
						case 5: case 6:
							loc = "Files/highscores1.txt";
							break;
						case 7: case 8:
							loc = "Files/highscores2.txt";
							break;
						default:
							loc = "Files/highscores3.txt";
							break;
						}

						SDL_Rect rect;

						rect.x = 250;
						rect.y = 300;
						rect.h = 50;
						rect.w = 100;

						SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
						SDL_RenderClear(renderer);

						SDL_Surface * surface = SDL_GetWindowSurface(window);
						SDL_FillRect(surface, NULL, 0x000000);
						SDL_BlitSurface(background, NULL, surface, NULL);

						SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

						SDL_RenderCopy(renderer, texture, NULL, NULL);

						SDL_DestroyTexture(texture);

						SDL_Event windowEvent;

						SDL_Surface *message = NULL;
						TTF_Font *font = NULL;
						SDL_Color textColor = { 255, 255, 255 };


						if (TTF_Init() == -1)
						{
							SDL_FreeSurface(background);
							return 0;
						}


						font = TTF_OpenFont("Fonts/segoepr.ttf", 70);
						message = TTF_RenderText_Solid(font, "Input name:", textColor);

						SDL_Texture * txt = SDL_CreateTextureFromSurface(renderer, message);



						SDL_BlitSurface(message, NULL, surface, &rect);
						SDL_FreeSurface(message);
						SDL_FreeSurface(surface);


						SDL_RenderCopy(renderer, txt, NULL, &rect);

						SDL_DestroyTexture(txt);

						SDL_RenderPresent(renderer);

						TTF_CloseFont(font);

						rect.x = 250;
						rect.y = 400;
						rect.h = 50;
						rect.w = 100;

						int quit = 0;
						SDL_Event e;

						strcpy(str, "");

						SDL_StartTextInput();
						while (quit == 0)
						{
							int renderText = 0;

							while (SDL_PollEvent(&e) != 0)
							{
								if (e.type == SDL_KEYDOWN)
								{
									if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(str) > 0)
									{
										str[strlen(str) - 1] = '\0';
										renderText = 1;
									}
									else if (e.key.keysym.sym == SDLK_RETURN)
									{
										quit = 1;
									}
									else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
									{
										SDL_SetClipboardText(str);
									}
									else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
									{
										strcpy(str, SDL_GetClipboardText());
										renderText = 1;
									}
								}
								else if (e.type == SDL_TEXTINPUT)
								{
									if (!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
									{
										strcat(str, e.text.text);
										renderText = 1;
									}
								}
							}
							if (renderText == 1)
							{
								rect.x = 250;
								rect.y = 300;
								rect.h = 50;
								rect.w = 100;


								SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
								SDL_RenderClear(renderer);

								SDL_Surface * surface = SDL_GetWindowSurface(window);
								SDL_FillRect(surface, NULL, 0x000000);
								SDL_BlitSurface(background, NULL, surface, NULL);

								SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

								SDL_RenderCopy(renderer, texture, NULL, NULL);

								SDL_DestroyTexture(texture);

								SDL_Event windowEvent;

								SDL_Surface *message = NULL;
								TTF_Font *font = NULL;
								SDL_Color textColor = { 255, 255, 255 };


								if (TTF_Init() == -1)
								{
									SDL_FreeSurface(x);
									SDL_FreeSurface(o);
									SDL_FreeSurface(playable);
									SDL_FreeSurface(big_x);
									SDL_FreeSurface(big_o);
									SDL_FreeSurface(background);
									return 0;
								}


								font = TTF_OpenFont("Fonts/segoepr.ttf", 70);
								message = TTF_RenderText_Solid(font, "Input name:", textColor);

								SDL_Texture * txt = SDL_CreateTextureFromSurface(renderer, message);
								SDL_BlitSurface(message, NULL, surface, &rect);
								SDL_FreeSurface(message);
								SDL_RenderCopy(renderer, txt, NULL, &rect);

								SDL_DestroyTexture(txt);

								SDL_RenderPresent(renderer);


								rect.x = 250;
								rect.y = 400;
								rect.h = 50;
								rect.w = 100;
								message = TTF_RenderText_Solid(font, str, textColor);

								txt = SDL_CreateTextureFromSurface(renderer, message);
								SDL_BlitSurface(message, NULL, surface, &rect);
								SDL_FreeSurface(message);

								SDL_RenderCopy(renderer, txt, NULL, &rect);

								SDL_FreeSurface(surface);

								TTF_CloseFont(font);

								SDL_DestroyTexture(txt);

								SDL_RenderPresent(renderer);
							}
						}
						SDL_StopTextInput();

						Crypt(-1, loc);
						highscores = fopen(loc, "a");
						time(&date);
						strcat(str, " ");
						strcat(str, ctime(&date));
						str[strlen(str) - 2] = ' ';
						str[strlen(str) - 3] = ' ';
						str[strlen(str) - 4] = ' ';
						str[strlen(str) - 5] = ' ';
						fprintf(highscores, "%d ", cnt);
						fprintf(highscores, str);
						fclose(highscores);
						Crypt(1, loc);
					}
					break;
				}
			}
		}
	}
	else {
		exit = 1;
	}

	SDL_FreeSurface(x);
	SDL_FreeSurface(o);
	SDL_FreeSurface(playable);
	SDL_FreeSurface(big_x);
	SDL_FreeSurface(big_o);
	SDL_FreeSurface(background);



	return exit;
}
