#include<iostream>
#include<vector>
#include<string>
#include<cstdlib>
#include<ctime>
//esse código só funciona no Windows, eu precisei usar da API dele na função print para deixar o texto colorido
//(senão fica extremamente confuso de se jogar)
#include<Windows.h>
using std::cout;
using std::cin;
using std::vector;
using std::string;
using std::endl;

unsigned board[42];//42=6 rows*7 columns

inline void reset() {
	for (auto i = 0; i < 42; ++i) board[i] = 0;
}

inline bool win(short player) {
	//check rows for horizontal win
	for (size_t i = 0; i < 42; i += 7) {
		for (size_t j = 0; j <= 3; ++j) if (board[i + j] == player && board[i + j + 1] == player && board[i + j + 2] == player && board[i + j + 3] == player) {
			return true;
		}
	}
	//check columns for vertical win
	for (size_t i = 0; i < 7; ++i) {
		for (size_t j = 0; j <= 14; j += 7) if (board[i + j] == player && board[i + j + 7] == player && board[i + j + 14] == player && board[i + j + 21] == player) {
			return true;
		}
	}
	//check for diagonal victory
	for (size_t i = 3; i <= 17; i += 7) {
		for (size_t j = 0; j <= 3; ++j) {
			if (board[i + j] == player && board[i + j + 6] == player && board[i + j + 12] == player && board[i + j + 18] == player) return true;
			if (board[i - j] == player && board[i - j + 8] == player && board[i - j + 16] == player && board[i - j + 24] == player) return true;
		}

	}
	return false;
}

//função heuristica para algoritmo minimax: esta perto de ganhar se tiver houver 3 peças consecutivas com espaço para uma quarta
//ou algo do tipo XPXPX onde P é uma peça e X é um espaço vazio (colocar uma peça no meio faz com que o oponente seja incapaz de defender os dois lados)
inline bool closetowin(short player) {
	for (size_t i = 0; i < 42; i += 7) {
		for (size_t j = 0; j <= 3; ++j) if (board[i + j] == 0 && board[i + j + 1] == player && board[i + j + 2] == player && board[i + j + 3] == 0) {
			return true;
		}
		for (size_t j = 0; j <= 2; ++j) if (board[i + j] == 0 && board[i + j + 1] == player && board[i + j + 2] == 0 && board[i + j + 3] == player && board[i + j + 4] == 0) {
			return true;
		}
	}
	for (size_t i = 0; i < 7; ++i) {
		for (size_t j = 0; j <= 14; j += 7) if (board[i + j] == 0 && board[i + j + 7] == player && board[i + j + 14] == player && board[i + j + 21] == player) {
			return true;
		}
	}
	//diagonals
	for (size_t i = 10; i <= 17; i += 7) {
		for (size_t j = 0; j <= 3; ++j) {
			if (board[i + j] == player && board[i + j + 6] == player && board[i + j + 12] == player && board[i + j + 18] == 0) return true;
			if (board[i + j] == player && board[i + j + 6] == player && board[i + j + 12] == 0 && board[i + j + 18] == player) return true;
			if (board[i + j] == player && board[i + j + 6] == 0 && board[i + j + 12] == player && board[i + j + 18] == player) return true;
			if (board[i + j] == 0 && board[i + j + 6] == player && board[i + j + 12] == player && board[i + j + 18] == player) return true;

			if (board[i - j] == player && board[i - j + 8] == player && board[i - j + 16] == player && board[i - j + 24] == 0) return true;
			if (board[i - j] == player && board[i - j + 8] == player && board[i - j + 16] == 0 && board[i - j + 24] == player) return true;
			if (board[i - j] == player && board[i - j + 8] == 0 && board[i - j + 16] == player && board[i - j + 24] == player) return true;
			if (board[i - j] == 0 && board[i - j + 8] == player && board[i - j + 16] == player && board[i - j + 24] == player) return true;
		}

	}
	return false;
}


inline bool full() {
	for (int i = 0; i < 7; ++i) if (board[i] == 0) return false;
	return true;
}
inline bool fullcolumn(size_t col) {
	if (col < 1 or col>7) {
		return true;
	}
	else {
		--col; //o indice col vai de 1-7, e temos que converter isso pra 0-6
		if (board[col] == 0) return false;
		return true;
	}
}

inline bool placi(size_t place, short player) {
	if (place < 1 or place>7) {
		cout << "Erro: selecione uma coluna adequada" << endl;
		return false;
	}
	else {
		--place;
		size_t i = 0;
		while (i < 42 && board[place + i] == 0) i += 7;
		if (i) {
			board[place + i - 7] = player;
			return true;
		}
		else {
			cout << "Erro: a coluna ja esta cheia!" << endl;
			return false;
		}
	}
}

//remover a peça mais elevada da coluna place, é importante para garantir que estamos testando estados válidos no algoritmo
inline bool remove(size_t place) {
	if (place < 1 or place>7) {
		cout << "Erro: selecione uma coluna adequada" << endl;
		return false;
	}
	else {
		--place;
		size_t i = 0;
		while (board[place + i] == 0 && i < 42) i += 7;
		if (i < 42) {
			board[place + i] = 0;
			return true;
		}
		else {
			cout << "Erro: Nao ha nada na coluna a ser retirado!" << endl;
			return false;
		}
	}
}

//retorna a "pontuação" da melhor jogada encontrada, e "pos" retorna a posição a ser jogada pela CPU, depth impede que o algoritmo
//gaste 10000000 de anos pesquisando toda a arvore do jogo, onde depois de ultrapassar a profundeza ele faz um chute torto
// (também conhecido como a função heuristica close to win) para determinar 
double best_play(unsigned& pos, short player = 2, unsigned depth = 0) {
	short opponent = (player == 1) ? 2 : 1;
	double victory = (player == 1) ? 400 : -400;
	vector<unsigned> moves;
	if (win(opponent)) return -victory;
	if (win(player)) return victory;
	if (full()) return 0;
	if (depth > 8) {
		if (closetowin(opponent)) return -victory / 2;
		if (closetowin(player)) return victory / 2;
		return 0;
	}
	unsigned dummy; //dummy variable for recursive calls
	double best = (player == 1) ? DBL_MIN : DBL_MAX;; //the best score
	double current; //current score
	for (unsigned i = 1; i <= 7; ++i) {
		if (!fullcolumn(i)) {
			placi(i, opponent);
			if (win(opponent)) {
				pos = i;
				remove(i);
				placi(i, player);
				auto temp = best_play(dummy, opponent, depth + 1);
				remove(i);
				return temp;
			}
			remove(i);
			placi(i, player);
			if (win(player)) {
				pos = i;
				remove(i);
				return victory;
			}
			current = best_play(dummy, opponent, depth + 1);
			if ((player == 1 && current > best) || (player == 2 && current < best)) {
				moves.clear();
				best = current;
				moves.push_back(i);
			}
			else if (current == best) moves.push_back(i);
			remove(i);
		}
	}
	//eu decidi tornar a jogada meio que aleatoria para ficar mais divertido
	if (depth == 0) {
		srand(time(NULL));
		auto s = moves.size();
		if (s > 0) {
			auto m = rand() % s;
			pos = moves[m];
		}
	}
	return best;
}

inline void print(void* HANDLE) {
	for (auto i = 0; i < 42; ++i) {
		if (i != 0 && i % 7 == 0) cout << endl;
		switch (board[i]) {
		case 1:
			SetConsoleTextAttribute(HANDLE, 4);
			cout << "H ";
			break;
		case 2:
			SetConsoleTextAttribute(HANDLE, 1);
			cout << "P ";
			break;
		default:
		case 0:
			SetConsoleTextAttribute(HANDLE, 15);
			cout << "O ";
			break;
		}
		SetConsoleTextAttribute(HANDLE, 15);
	}
	cout << endl;
	for (auto i = 1; i < 8; ++i) cout << i << ' ';
	cout << endl;
	cout << endl;
}

int main() {
	auto h = GetStdHandle(STD_OUTPUT_HANDLE);
	unsigned menu_input, input;
	unsigned cpu;
	short player = 1;
	while (true) {
		cout << "Selecione uma opcao:" << endl;
		cout << "0-Ajuda" << endl;
		cout << "1-Modo PvP" << endl;
		cout << "2-Modo vs CPU" << endl;
		cout << "3-Olhe 2 CPU tentando se matar" << endl;
		cout << "4-Sair" << endl;
		cin >> menu_input;
		switch (menu_input) {
		case 0:
			cout << "Cada jogador tem seu turno para colocar uma de suas peças na fileira que desejar" << endl;
			cout << "Ganha quem conseguir criar um arranjo de 4 de suas pecas consecutivas, seja na diagonal, vertical ou horizontal" << endl;
			cout << "Digite um número entre 1 e 7 (as colunas sao enumeradas para facilitar)" << endl;
			cout << "Depois aperte Enter para confirmar sua jogada" << endl;
			cout << "Caso esteja jogando contra a CPU, pode parecer que o terminal travou, tenha um pouco de paciencia" << endl << "e espere alguns segundos" << endl;
			cout << "Se ainda assim continuar travado, apertar algum botao dentro do terminal ajuda algumas vezes" << endl;
			cout << endl << endl;
			break;
		case 1:
			reset();
			player = 1;
			while (!win((player % 2) + 1) and !full()) {
				print(h);
				cout << "Jogador " << player << ": ";
				cin >> input;
				if (!placi(input, player)) continue;
				player = (player == 1) ? 2 : 1;
			}
			print(h);
			if (win(player % 2 + 1)) cout << "Vitoria do Jogador " << player % 2 + 1 << "!" << endl;
			else cout << "Empatou :P" << endl;
			break;
		case 2:
			reset();
			player = 2;
			while (!win(player) and !full()) {
				player = (player % 2) + 1;
			error:
				print(h);
				if (player == 1) {
					cout << "Jogador: ";
					cin >> input;
					if (!placi(input, player)) goto error;
				}
				else {
					best_play(cpu);
					placi(cpu, 2);
					cout << "Computador: " << cpu << endl;
				}
			}
			print(h);
			if (win(player)) {
				std::string message = (player == 1) ? "o Jogador!" : "a CPU!";
				cout << "Vitoria d" + message;
				cout << endl;
			}
			else cout << "Empatou :P" << endl;
			break;
		case 3:
			reset();
			player = 1;
			while (!win(1) and !win(2) and !full()) {
				print(h);
				best_play(cpu, player);
				placi(cpu, player);
				cout << "CPU " << player << ": " << cpu << endl;
				player = (player % 2) + 1;
			}
			print(h);
			if (win((player % 2) + 1)) cout << "CPU " << (player % 2) + 1 << " Ganhou!" << endl;
			else cout << "Empatou :P" << endl;
			break;
		case 4:
			exit(0);
			break;
		}
	}
	system("pause");
	return 0;
}