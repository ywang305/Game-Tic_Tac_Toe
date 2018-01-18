#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <utility>
using namespace std;

class TicTacToe {
public:
	using Location = std::pair<int, int>;
	enum Player { NoPlayer = 0, Player1, Player2 };
	enum Result { NoWin = 0, Win, Tie, InvalidMove };
	enum GameMode { Auto = 0, Semi_Auto, Manu };
	struct BestMove {
		Location location;
		int score;
		BestMove(Location loc, int sco) : location(loc), score(sco) {}
	};

	TicTacToe();
	~TicTacToe();

	///
	/// \brief MakeMove Interface for the game playing system to add a new move to
	/// the game.
	/// \param player   Player making this move.
	/// \param location The selected location. X=first, Y=second
	/// \return Result of the move. Win if the move wins the game, Invalid move if
	/// input has errors, NoWin otherwise.
	///
	Result MakeMove(Player player, Location location);
	void ResetGame();
	void Play(GameMode mode);
private:
	bool isOverflow(const Location &location);
	bool IsMovable();
	bool IsMovable(Location location);
	Result Evaluate(Player);
	BestMove GetBestMove(Player player);
	BestMove GetBestMove(Player player, int alpha, int beta);

	void Render();

	vector<vector<int>> board;
};


TicTacToe::TicTacToe() {
	// Add any initializations for the data structures.
	board.resize(3, vector<int>(3));
	ResetGame();
}

TicTacToe::~TicTacToe() {
	// Add any clean-up or deallocations needed.
}

TicTacToe::Result TicTacToe::MakeMove(Player player, Location location) {
	// Validate parameters, update game state and check for win condition.
	//return NoWin;
	
	if (!IsMovable(location)) {
		return Result::InvalidMove;
	}
	int x = location.first, y = location.second;
	board[x][y] = player;
	return Evaluate(player);
}

void TicTacToe::ResetGame() {
	// Clear the game state for a new game.
	for (auto &r : board) {
		for (auto &cell : r) {
			cell = Player::NoPlayer;
		}
	}
}


bool TicTacToe::isOverflow(const Location &location) {
	if (location.first < 0 || location.first >= 3 
		|| location.second < 0 || location.second >= 3) {
		return true;
	}
	return false;
}

bool TicTacToe::IsMovable() {
	for (auto &row : board) {
		for (auto cell : row) {
			if (cell == Player::NoPlayer) {
				return true;
			}
		}
	}
	return false;
}

bool TicTacToe::IsMovable(Location location) {
	return (!isOverflow(location)) && board[location.first][location.second] == Player::NoPlayer;
}

TicTacToe::Result TicTacToe::Evaluate(TicTacToe::Player player) {
	vector<int> goals(8, 0);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (board[i][j] == player) {
				goals[i]++;
				goals[3 + j]++;
				if (i == j) goals[6]++;
				if (i + j == 2) goals[7]++;
			}
		}
	}
	if (any_of(goals.begin(), goals.end(), [](int g) {
		return g >= 3;
	})) {
		return Result::Win;
	}
	return IsMovable()? Result::NoWin : Result::Tie;
}

TicTacToe::BestMove TicTacToe::GetBestMove(Player player) {
	if (Evaluate(Player::Player1) == Result::Win) return BestMove(Location(), 100);
	if (Evaluate(Player::Player2) == Result::Win) return BestMove(Location(), -100);
	if (!IsMovable()) return BestMove(Location(-1, -1), 0);

	BestMove bestMove = player == Player::Player1 ?
		BestMove(Location(), -10000) : BestMove(Location(), 10000);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (IsMovable(Location(i, j))) {
				board[i][j] = player;

				Player opponent = player == Player::Player1 ? Player::Player2 : Player::Player1;
				int curScore = GetBestMove(opponent).score;
				if ((player == Player::Player1 && bestMove.score < curScore) ||
					(player == Player::Player2 && bestMove.score > curScore)) {
					bestMove.location = Location(i,j);
					bestMove.score = curScore;
				}

				board[i][j] = Player::NoPlayer;
			}
		}
	}
	return bestMove;
}


TicTacToe::BestMove TicTacToe::GetBestMove(Player player, int alpha, int beta) {
	if (Evaluate(Player::Player1) == Result::Win) return BestMove(Location(), 10);
	if (Evaluate(Player::Player2) == Result::Win) return BestMove(Location(), -10);
	if (!IsMovable()) return BestMove(Location(-1, -1), 0);

	BestMove bestMove = player == Player::Player1 ?
		BestMove(Location(), -100) : BestMove(Location(), 100);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (IsMovable(Location(i, j))) {
				board[i][j] = player;

				Player opponent = player == Player::Player1 ? Player::Player2 : Player::Player1;
				int curScore = GetBestMove(opponent, alpha, beta).score;
				if (player == Player::Player1 && bestMove.score < curScore) {
					bestMove.location = Location(i, j);
					bestMove.score = curScore;
					alpha = max(alpha, bestMove.score);
				}
				else if (player == Player::Player2 && bestMove.score > curScore) {
					bestMove.location = Location(i, j);
					bestMove.score = curScore;
					beta = min(beta, bestMove.score);
					
				}

				board[i][j] = Player::NoPlayer;
				
				if (alpha >= beta) return bestMove;
			}
		}
	}
	return bestMove;
}




void TicTacToe::Render() {
	stringstream ss("\n");
	for (auto &row : board) {
		for (auto cell : row) {
			if (cell == Player::Player1) {
				ss << "o ";
			}
			else if (cell == Player::Player2) {
				ss << "x ";
			}
			else ss << "_ ";
		}
		ss << "\n";
	}
	cout << ss.str() << "\n";
}

void TicTacToe::Play(TicTacToe::GameMode mode) {
	Render();
	if (GameMode::Auto == mode) {
		Player player = Player::Player1;
		Result res = Result::NoWin;
		while (res != Result::Tie && res != Result::Win && res != Result::InvalidMove) {
			//res = MakeMove(player, GetBestMove(player).location);
			res = MakeMove(player, GetBestMove(player, -10000, 10000).location);
			Render();
			player = player == Player::Player1 ? Player::Player2 : Player::Player1;
		}
	}

	else if (GameMode::Semi_Auto == mode) {
		int user = 0;
		while(user != 1 && user != 2) {
			cout << "choose player1 or player2? input 1 or 2 : ";
			cin >> user;
		}
		Player player = Player::Player1;
		Result res = Result::NoWin;
		while (res == Result::NoWin) {
			if (user == player) {
				int x, y;
				res = Result::InvalidMove;
				while (res == Result::InvalidMove) {
					cout << "input x y : ";
					cin >> x >> y;
					res = MakeMove(player, Location(x, y));
				}
			}
			else { // AI
				res = MakeMove(player, GetBestMove(player).location);
			}
			
			Render();
			player = player == Player::Player1 ? Player::Player2 : Player::Player1;
		}
	}

	else if(mode == GameMode::Manu) {
		Player player = Player::Player1;
		Result res = Result::NoWin;
		while (res == Result::NoWin) {
			int x, y;
			res = Result::InvalidMove;
			while (res == Result::InvalidMove) {
				cout << "input x y : ";
				cin >> x >> y;
				res = MakeMove(player, Location(x, y));
			}
			Render();
			player = player == Player::Player1 ? Player::Player2 : Player::Player1;
		}
	}
}



TicTacToe game;

int main(int argc, char** argv) {
	std::cout << "Starting test!" << std::endl;
	const TicTacToe::Player player1 = TicTacToe::Player1;
	const TicTacToe::Player player2 = TicTacToe::Player2;

	game.Play(TicTacToe::GameMode::Auto);

	cout << "\n";
	system("pause");
	return 0;
}