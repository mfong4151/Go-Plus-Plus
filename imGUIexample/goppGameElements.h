#pragma once
#include <string>


class stone {
public:

	//Coordinates of where we're storing the stones in the board matrix
	int stoneX;
	int stoneY;

	//Pixel coordinats of where each stone is relative to 
	float displayX;
	float displayY;

	//stone up;
	//stone down;
	//stone left;
	//stone right;
	int spaceOwnership;

};

//player p1 = 0, p2 = 1, empty stones = 2
// 2 is the default number assigned 


//Global player variables
class player {
public:
	std::string playerName;
	int playerNumber;
	char playerIcon;
	int playerScore = 0;
	int playerPass = 0;

};

//Global board, players, and 

stone boardMatrix[19][19];
player p1, p2, unowned;
int playerTurn = 0;


//Init players and board
void createGameContexts() {
	p1.playerNumber = 0;
	p2.playerNumber = 1;
	//Need to add player icons (black and white stones) here respectively
	unowned.playerNumber = 2;

	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			boardMatrix[i][j].spaceOwnership = 2;
			boardMatrix[i][j].stoneX = i;
			boardMatrix[i][j].stoneY = j;
		}
	}
}
