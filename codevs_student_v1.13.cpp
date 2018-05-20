#include <stdio.h>
#include <math.h>

#include <iostream>

#define W 10
#define H 16
#define T 3
#define S 10
#define N 500

typedef struct PACK {
	short int pack[N][T*T];
}PACK;

typedef struct STATE {
	int turn;
	short int obstacle;
	short int field[H + 3][W];
	short int move[2];
	int value;
}STATE;

void InputFirst(PACK *pack);
bool InputTurn(STATE *MyState, STATE *EnemyState);
void ExecuteTurn(const PACK pack, const STATE MyState, const STATE EnemyState);
int GetNextState(PACK pack, STATE NowState, STATE *NextState, int next);
int MoveState(short int *pack, STATE NowState, STATE *NextState, int next, int posi, int roll);
void Myqsort(STATE *NextState, int left, int right);
int Bonus(STATE NowState);
int MoveStateBonus(short int *pack, STATE NowState, int posi);

int main() {
	PACK pack;
	STATE MyState, EnemyState;

	std::cout << "burokoron_v1.13" << std::endl;
	std::cout.flush();

	InputFirst(&pack);

	for (int i = 0; i < N; i++) {
		bool inputSuccess = InputTurn(&MyState, &EnemyState);

		if (!inputSuccess) {
			std::cerr << "input Failed" << std::endl;
			std::cerr.flush();
			break;
		}

		ExecuteTurn(pack, MyState, EnemyState);
	}
}

void InputFirst(PACK *pack) {
	int skip;
	char end[4];

	for (int i = 0;i < 5;i++) std::cin >> skip;

	for (int i = 0;i < N;i++) {
		for (int j = 0;j < T*T;j++) std::cin >> pack->pack[i][j];
		std::cin >> end;
	}
}

bool InputTurn(STATE *MyState, STATE *EnemyState) {
	int skip;
	char end[4];

	if (!(std::cin >> MyState->turn >> skip)) return false;
	EnemyState->turn = MyState->turn;
	
	std::cin >> MyState->obstacle;

	for (int i = H - 1;i >= 0;i--) {
		for (int j = 0;j < W;j++) {
			std::cin >> MyState->field[i][j];
		}
	}

	std::cin >> end;

	std::cin >> EnemyState->obstacle;

	for (int i = H - 1;i >= 0;i--) {
		for (int j = 0;j < W;j++) {
			std::cin >> EnemyState->field[i][j];
		}
	}

	std::cin >> end;

	return true;
}

void ExecuteTurn(const PACK pack, const STATE MyState, const STATE EnemyState) {
	STATE *NowMyState, *NextMyState;
	int k = 1, now = 0, next = 0, MaxTurn = 5, Width = 100;
	int t;

	if (MyState.turn > 50) Width /= 2;
	if (MyState.turn > 100) Width /= 4;

	NowMyState = (STATE *)calloc(Width, sizeof(STATE));
	NextMyState = (STATE *)calloc(Width * 36, sizeof(STATE));

	NowMyState[0] = MyState;
	NowMyState[0].move[0] = -3;
	NowMyState[0].move[1] = -3;
	NowMyState[0].value = 0;


	for (t = 0;t < MaxTurn;t++) {
		if (NowMyState[0].obstacle > 0) MaxTurn++;
		if (NowMyState[0].turn == 500) break;
		next = 0;
		for (int i = 0; i < k;i++) {
			if (NowMyState[i].value > -10000) {
				next = GetNextState(pack, NowMyState[i], &NextMyState[0], next);
			}
		}
		Myqsort(&NextMyState[0], 0, next - 1);

		if (next > Width) {
			next = Width;
			k = Width;
		}
		else k = next;
		for (int j = 0; j < next;j++) {
			NowMyState[j] = NextMyState[j];
		}
		if (NowMyState[0].value >= 1000000) break;
	}

	std::cerr << NowMyState[0].value << " " << t << std::endl;
	std::cerr.flush();

	std::cout << NowMyState[0].move[0] << " " << NowMyState[0].move[1] << std::endl;
	std::cout.flush();

	free(NowMyState);
	free(NextMyState);
}

int GetNextState(PACK pack, STATE NowState, STATE *NextState, int next) {
	bool PackLeft, PackCenter, PackRight;
	short int PackRoll[9], temp;

	for (int i = 0;i < 9;i++) PackRoll[i] = pack.pack[NowState.turn][i];
	for (int i = 0;i < 9;i++) {
		if (PackRoll[i] == 0 && NowState.obstacle > 0) {
			PackRoll[i] = S + 1;
			NowState.obstacle--;
		}
	}

	for (int i = 0;i < 4;i++) {
		if (PackRoll[0] == 0 && PackRoll[3] == 0 && PackRoll[6] == 0) PackLeft = true;
		else PackLeft = false;
		if (PackRoll[1] == 0 && PackRoll[4] == 0 && PackRoll[7] == 0) PackCenter = true;
		else PackCenter = false;
		if (PackRoll[2] == 0 && PackRoll[5] == 0 && PackRoll[8] == 0) PackRight = true;
		else PackRight = false;

		if (PackLeft == true && PackCenter == true) {
			MoveState(PackRoll, NowState, NextState, next, -2, i);
			next++;
		}
		if (PackLeft == true) {
			MoveState(PackRoll, NowState, NextState, next, -1, i);
			next++;
		}
		for (int j = 0;j < 8;j++) {
			MoveState(PackRoll, NowState, NextState, next, j, i);
			next++;
		}
		if (PackCenter == true && PackRight == true) {
			MoveState(PackRoll, NowState, NextState, next, 9, i);
			next++;
		}
		if (PackRight == true) {
			MoveState(PackRoll, NowState, NextState, next, 8, i);
			next++;
		}

		temp = PackRoll[0];
		PackRoll[0] = PackRoll[6];
		PackRoll[6] = PackRoll[8];
		PackRoll[8] = PackRoll[2];
		PackRoll[2] = temp;

		temp = PackRoll[1];
		PackRoll[1] = PackRoll[3];
		PackRoll[3] = PackRoll[7];
		PackRoll[7] = PackRoll[5];
		PackRoll[5] = temp;
	}
	
	return next;
}

int MoveState(short int *pack, STATE NowState, STATE *NextState, int next, int posi, int roll) {
	int chain = 0, count = 0, score = 0;
	bool flag;

	NextState[next] = NowState;
	NextState[next].turn++;

	if (NextState[next].move[0] == -3) {
		NextState[next].move[0] = posi;
		NextState[next].move[1] = roll;
	}

	if (posi == -2) {
		for (int j = 0;j < 3;j++) {
			int i = 0;
			if (pack[8 - j * 3] != 0) {
				while (NextState[next].field[i][posi + 2] != 0) i++;
				NextState[next].field[i][posi + 2] = pack[8 - j * 3];
			}
		}
	}

	if (posi == -1) {
		for (int j = 0;j < 3;j++) {
			int i = 0;
			if (pack[7 - j * 3] != 0) {
				while (NextState[next].field[i][posi + 1] != 0) i++;
				NextState[next].field[i][posi + 1] = pack[7 - j * 3];
			}
			i = 0;
			if (pack[8 - j * 3] != 0) {
				while (NextState[next].field[i][posi + 2] != 0) i++;
				NextState[next].field[i][posi + 2] = pack[8 - j * 3];
			}
		}
	}

	if (0 <= posi&& posi <= 7) {
		for (int j = 0;j < 3;j++) {
			int i = 0;
			if (pack[6 - j * 3] != 0) {
				while (NextState[next].field[i][posi] != 0) i++;
				NextState[next].field[i][posi] = pack[6 - j * 3];
			}
			i = 0;
			if (pack[7 - j * 3] != 0) {
				while (NextState[next].field[i][posi + 1] != 0) i++;
				NextState[next].field[i][posi + 1] = pack[7 - j * 3];
			}
			i = 0;
			if (pack[8 - j * 3] != 0) {
				while (NextState[next].field[i][posi + 2] != 0) i++;
				NextState[next].field[i][posi + 2] = pack[8 - j * 3];
			}
		}
	}

	if (posi == 8) {
		for (int j = 0;j < 3;j++) {
			int i = 0;
			if (pack[6 - j * 3] != 0) {
				while (NextState[next].field[i][posi] != 0) i++;
				NextState[next].field[i][posi] = pack[6 - j * 3];
			}
			i = 0;
			if (pack[7 - j * 3] != 0) {
				while (NextState[next].field[i][posi + 1] != 0) i++;
				NextState[next].field[i][posi + 1] = pack[7 - j * 3];
			}
		}
	}

	if (posi == 9) {
		for (int j = 0;j < 3;j++) {
			int i = 0;
			if (pack[6 - j * 3] != 0) {
				while (NextState[next].field[i][posi] != 0) i++;
				NextState[next].field[i][posi] = pack[6 - j * 3];
			}
		}
	}

	do {
		flag = false;
		count = 0;
		short int DelFlag[H][W] = { 0 };
		for (int i = 0;i < H;i++) {
			for (int j = 0;j < W;j++) {
				if (NextState[next].field[i][j] == S + 1) NextState[next].value -= i * 10;
				else if (NextState[next].field[i][j] != 0){
					int sum = NextState[next].field[i][j], k = 0;
					while (sum < 10) // 左方向
					{
						if (j - k == 0) break;
						k++;
						if (NextState[next].field[i][j - k] == 0) break;
						sum += NextState[next].field[i][j - k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NextState[next].value--;
					k = 0;
					sum = NextState[next].field[i][j];
					while (sum < 10) // 左上方向
					{
						if (j - k == 0) break;
						if (i + k == H - 1) break;
						k++;
						if (NextState[next].field[i + k][j - k] == 0) break;
						sum += NextState[next].field[i + k][j - k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NextState[next].value--;
					k = 0;
					sum = NextState[next].field[i][j];
					while (sum < 10) // 上方向
					{
						if (i + k == H - 1) break;
						k++;
						if (NextState[next].field[i + k][j] == 0) break;
						sum += NextState[next].field[i + k][j];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NextState[next].value--;
					k = 0;
					sum = NextState[next].field[i][j];
					while (sum < 10) // 右上方向
					{
						if (j + k == W - 1) break;
						if (i + k == H - 1) break;
						k++;
						if (NextState[next].field[i + k][j + k] == 0) break;
						sum += NextState[next].field[i + k][j + k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NextState[next].value--;
					k = 0;
					sum = NextState[next].field[i][j];
					while (sum < 10) // 右方向
					{
						if (j + k == W - 1) break;
						k++;
						if (NextState[next].field[i][j + k] == 0) break;
						sum += NextState[next].field[i][j + k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NextState[next].value--;
					k = 0;
					sum = NextState[next].field[i][j];
					while (sum < 10) // 右下方向
					{
						if (j + k == W - 1) break;
						if (i - k == 0) break;
						k++;
						if (NextState[next].field[i - k][j + k] == 0) break;
						sum += NextState[next].field[i - k][j + k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NextState[next].value--;
					k = 0;
					sum = NextState[next].field[i][j];
					while (sum < 10) // 下方向
					{
						if (i - k == 0) break;
						k++;
						if (NextState[next].field[i - k][j] == 0) break;
						sum += NextState[next].field[i - k][j];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NextState[next].value--;
					k = 0;
					sum = NextState[next].field[i][j];
					while (sum < 10) // 左下方向
					{
						if (j - k == 0) break;
						if (i - k == 0) break;
						k++;
						if (NextState[next].field[i - k][j - k] == 0) break;
						sum += NextState[next].field[i - k][j - k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NextState[next].value--;
				}
			}
		}

		if (count > 0) {
			flag = true;
			chain++;
			score += (int)floor(pow(1.3, (double)chain))*(count / 2);
		}

		if (flag == true) {
			for (int i = 0;i < W;i++) {
				for (int j = 0;j < H;j++) {
					if (DelFlag[j][i] == 1) NextState[next].field[j][i] = 0;
				}

				int j = 0;
				for (int k = 0;k < H;k++) {
					if (NextState[next].field[k][i] != 0) {
						if (j != k) {
							NextState[next].field[j][i] = NextState[next].field[k][i];
							NextState[next].field[k][i] = 0;
						}
						j++;
					}
				}
			}
		}
	} while (flag == true);

	for (int i = 0;i < W;i++) {
		if (NextState[next].field[H][i] != 0) {
			NextState[next].value = -10000;
			return next++;
		}
	}

	if (chain > 13) {
		NextState[next].value += 10000 * score;
		return next++;
	}

	if (score >= 5) NextState[next].value += score * 100;
	NextState[next].value -= score * 10;
	NextState[next].obstacle -= score / 5;
	if (NextState[next].obstacle == 0 && NextState[next].turn < 100) NextState[next].value += Bonus(NextState[next]);

	return next++;
}

void Myqsort(STATE *NextState, int left, int right) {
	if (left<right) {
		int i = left, j = right, pivod;
		STATE temp;

		pivod = NextState[i + (j - i) / 2].value;

		while (1) {
			while (pivod < NextState[i].value) i++;
			while (pivod > NextState[j].value) j--;
			if (i >= j) break;

			temp = NextState[i];
			NextState[i] = NextState[j];
			NextState[j] = temp;
			i++;
			j--;
		}

		Myqsort(NextState, left, i - 1);
		Myqsort(NextState, j + 1, right);
	}
}

int Bonus(STATE NowState) {
	short int PackRoll[9] = { 0 };
	int value, max = -10000;

	for (int i = 1;i <= 9;i++) {
		PackRoll[0] = i;
		for (int j = 0;j <= 9;j++) {
			STATE NextState = NowState;
			value = MoveStateBonus(PackRoll, NextState, j);

			if (max < value) max = value;
		}
	}

	return max;
}

int MoveStateBonus(short int *pack, STATE NowState, int posi) {
	int chain = 0, count = 0, score = 0;
	bool flag;

	int l = 0;
	while (NowState.field[l][posi + 2] != 0) l++;
	NowState.field[l][posi + 2] = pack[0];

	do {
		flag = false;
		count = 0;
		short int DelFlag[H][W] = { 0 };
		for (int i = 0;i < H;i++) {
			for (int j = 0;j < W;j++) {
				if (NowState.field[i][j] != 0) {
					int sum = NowState.field[i][j], k = 0;
					while (sum < 10) // 左方向
					{
						if (j - k == 0) break;
						k++;
						if (NowState.field[i][j - k] == 0) break;
						sum += NowState.field[i][j - k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NowState.value--;
					k = 0;
					sum = NowState.field[i][j];
					while (sum < 10) // 左上方向
					{
						if (j - k == 0) break;
						if (i + k == H - 1) break;
						k++;
						if (NowState.field[i + k][j - k] == 0) break;
						sum += NowState.field[i + k][j - k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NowState.value--;
					k = 0;
					sum = NowState.field[i][j];
					while (sum < 10) // 上方向
					{
						if (i + k == H - 1) break;
						k++;
						if (NowState.field[i + k][j] == 0) break;
						sum += NowState.field[i + k][j];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NowState.value--;
					k = 0;
					sum = NowState.field[i][j];
					while (sum < 10) // 右上方向
					{
						if (j + k == W - 1) break;
						if (i + k == H - 1) break;
						k++;
						if (NowState.field[i + k][j + k] == 0) break;
						sum += NowState.field[i + k][j + k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NowState.value--;
					k = 0;
					sum = NowState.field[i][j];
					while (sum < 10) // 右方向
					{
						if (j + k == W - 1) break;
						k++;
						if (NowState.field[i][j + k] == 0) break;
						sum += NowState.field[i][j + k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NowState.value--;
					k = 0;
					sum = NowState.field[i][j];
					while (sum < 10) // 右下方向
					{
						if (j + k == W - 1) break;
						if (i - k == 0) break;
						k++;
						if (NowState.field[i - k][j + k] == 0) break;
						sum += NowState.field[i - k][j + k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NowState.value--;
					k = 0;
					sum = NowState.field[i][j];
					while (sum < 10) // 下方向
					{
						if (i - k == 0) break;
						k++;
						if (NowState.field[i - k][j] == 0) break;
						sum += NowState.field[i - k][j];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NowState.value--;
					k = 0;
					sum = NowState.field[i][j];
					while (sum < 10) // 左下方向
					{
						if (j - k == 0) break;
						if (i - k == 0) break;
						k++;
						if (NowState.field[i - k][j - k] == 0) break;
						sum += NowState.field[i - k][j - k];
						if (sum == 10) {
							DelFlag[i][j] = 1;
							count++;
							break;
						}
					}
					if (sum > 10 && k < 2) NowState.value--;
				}
			}
		}

		if (count > 0) {
			flag = true;
			chain++;
			score += (int)floor(pow(1.3, (double)chain))*(count / 2);
		}

		if (flag == true) {
			for (int i = 0;i < W;i++) {
				for (int j = 0;j < H;j++) {
					if (DelFlag[j][i] == 1) NowState.field[j][i] = 0;
				}

				int j = 0;
				for (int k = 0;k < H;k++) {
					if (NowState.field[k][i] != 0) {
						if (j != k) {
							NowState.field[j][i] = NowState.field[k][i];
							NowState.field[k][i] = 0;
						}
						j++;
					}
				}
			}
		}
	} while (flag == true);

	for (int i = 0;i < W;i++) {
		if (NowState.field[H][i] != 0) return -1000;
	}

	NowState.value = score * 100;
	NowState.value -= score * 10;

	return NowState.value;
}


