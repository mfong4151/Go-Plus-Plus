//https://github.com/ocornut/imgui/blob/master/imgui.h
//https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples

#include "UseImGui.h"
#include <cmath>
#include <set>
#include <string>
#include <utility>
#include <iostream>
#include <string>
#include <vector>

//All Classes might want to go in UseImGui.h or a separate header file altogethr

//Backend/game logic:

class stone {
public:

	//Coordinates of where we're storing the stones in the board matrix
	int stoneX;
	int stoneY;

	//Pixel coordinats of where each stone is relative to 
	float displayX;
	float displayY;
	//To be worked on later; by saving pointers to adjacent stones in the actual stone, we can make the DFS process much faster than constantly accessing from the global board
	//At this point this isn't a high priority

	//stone up;
	//stone down;
	//stone left;
	//stone right;
	int spaceOwnership;

};

class player {
public:
	std::string playerName;
	int playerNumber;
	char playerIcon;
	int playerScore = 0;

};

//Global player variables, board variable, player turn,
//p1 = 0, p2 = 1, empty space = 2, 
//endGame player variables are used for claiming and displaying provisional spaces at the end of game.
// gamePhase tracks what "phase" of the game we're in: 0 -> stone placement 1 -> Endgame
//visited: used to check territories for capture during dfs

player p1, p2, endGame1, endGame2, empty;
stone boardMatrix[19][19];
int playerTurn = 0;
int gamePhase = 0; 
int playerPass = 0;
std::set<std::pair<int, int>> visited;

//Check if both players have passed, if so, enter endgame phase
void checkTwoPass()
{
	if (playerPass == 2){
		gamePhase = 1;
	}

	std::cout << gamePhase;
}

//Init players and board
void gameContext::createGameContexts() {
	p1.playerNumber = 0;
	p2.playerNumber = 1;
	//Need to add player icons (black and white stones) here respectively
	//empty.playerNumber = 2;
	endGame1.playerNumber = 10;
	endGame2.playerNumber = 11;
	
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			boardMatrix[i][j].spaceOwnership = 2;
			boardMatrix[i][j].stoneX = i;
			boardMatrix[i][j].stoneY = j;
		}
	}
}
void printSet() {

	for (auto const& x : visited) {
		std::cout << '(';
		std::cout << x.first;
		std::cout << ',';
		std::cout << x.second;
		std::cout << ')';
		std::cout << ';';
	}
}


/////////////////////////////////////////////Stone placement via dfs; this is the core of the backend code ////////////////////////////////////////////////
//1. We place a stone in an empty spot (void placeStone)
//2. We need to check if we've put ourselves into an enclosed territory, if we have, then we remove the stone(s) almost immediately (bool enclosed)
//2a. If this returns false, then proceed to number 3a.
//2b. If this returns True, proceed 3b
//3a. We check if there are any neighbors of the opposite color. If there is, then we have to check for enclosed territories
//3b. remove the stones via itteration over visited, 


bool enclosed(int owner, int op, int x, int y) {
	//If we run into enemy stones, or if we trace over a space that we already came through return true
	
	std::pair<int, int> coords( x, y );

	// Two base cases need to stand alone, or else stack overflow occurs


	if ((x < 0 || y < 0 || x >= 19 || y >= 19) ||
		(boardMatrix[x][y].spaceOwnership == op)||
		(visited.find(coords) != visited.end()))
	{
		return true;
	}

	//If we run into empty spaces, return false
	if (boardMatrix[x][y].spaceOwnership == 2) 
	{
		return false;
	}

	visited.insert(coords);

	//Implict written: If we go over our own space, then we check it for enclosure.
	//This include going over other spaces that might already belong to the player.
	
	bool up = enclosed(owner, op, x, y - 1);
	bool down = enclosed(owner, op, x, y + 1);
	bool left =	enclosed(owner, op, x - 1, y);
	bool right = enclosed(owner, op, x + 1, y);

	return (up && down && left && right);
}

int removeStonesAddScore() {

	int score = 0;

	for (auto const& itr : visited) {
		boardMatrix[itr.first][itr.second].spaceOwnership = 2;

		score += 1;
	}
	return score;
}

void placeStone(int x, int y)
{
	stone emptySpace = boardMatrix[x][y];
	stone newStone;
	newStone.spaceOwnership = playerTurn;
	int op;
	int score = 0;
	bool selfEnclosure, upEnclosure = false, downEnclosure = false, leftEnclosure = false, rightEnclosure = false;

	if (playerTurn == 0) {
		op = 1;
	}
	else if (playerTurn == 1) {
		op = 0;
	}

	if (emptySpace.spaceOwnership == 2) {
		newStone.stoneX = emptySpace.stoneX;
		newStone.stoneY = emptySpace.stoneY;
		newStone.displayX = emptySpace.displayX;
		newStone.displayY = emptySpace.displayY;

		boardMatrix[x][y] = newStone;
		//Empty global visited reset empty every time we run check

		if (y - 1 >= 0 && boardMatrix[x][y - 1].spaceOwnership == op) {
			visited = {};
			upEnclosure = enclosed(op, playerTurn, x, y - 1);
			if (upEnclosure) {
				score += removeStonesAddScore(); 
			};
		}

		//Check down

		if (y + 1 < 19 && boardMatrix[x][y + 1].spaceOwnership == op) {  
			visited = {};
			downEnclosure = enclosed(op, playerTurn, x, y + 1);
			if (downEnclosure) {
				score += removeStonesAddScore();
			};
		}

		//Check left

		if (x - 1 >= 0 && boardMatrix[x - 1][y].spaceOwnership == op) {
			visited = {};
			leftEnclosure = enclosed(op, playerTurn, x - 1, y);
			if (leftEnclosure) {
				score += removeStonesAddScore();
			};
		}

		//Check right

		if (x + 1 < 19 && boardMatrix[x + 1][y].spaceOwnership == op) {
			
			visited = {};
			rightEnclosure = enclosed(op, playerTurn, x + 1, y);
			if (rightEnclosure) {
				score += removeStonesAddScore();
			};
		}
		
		if (playerTurn == 0) {
			p1.playerScore += score;
		}
		else {
			p2.playerScore += score;
		}
	
		visited = {};
		bool selfEnclosure = enclosed(playerTurn, op, x, y);
		//We need to run a check for if the player has actually put themselves in a bad position. If they have, and they haven't taken a cluster in atari, then points are granted to op.

		if (selfEnclosure && !(upEnclosure || downEnclosure || leftEnclosure || rightEnclosure))
		{
			score += removeStonesAddScore();
			if (playerTurn == 0) {
				p2.playerScore += score;
			}
			else {
				p1.playerScore += score;
			}
		}
	}
}


int addStonesAddScore(int spaceOwner) {

	int score = 0;

	for (auto const& itr : visited) {
		boardMatrix[itr.first][itr.second].spaceOwnership = spaceOwner;

		score += 1;
	}
	return score;
}

//Endgame DFS Methods

bool enclosedEndGame(int owner, int op, int x, int y) {
	//If we run into enemy stones, or if we trace over a space that we already came through return true

	std::pair<int, int> coords(x, y);

	if ((x < 0 || y < 0 || x >= 19 || y >= 19) ||
		boardMatrix[x][y].spaceOwnership == owner ||
		(visited.find(coords) != visited.end()))
	{
		return true;
	}

	if (boardMatrix[x][y].spaceOwnership == op)
	{
		return false;
	}

	
	visited.insert(coords);


	//Implict: If we go over our own space, then we check it for enclosure.
	//This include going over other spaces that might already belong to the player.

	bool up =    enclosed(owner, op, x, y - 1);
	bool down =  enclosed(owner, op, x, y + 1);
	bool left =  enclosed(owner, op, x - 1, y);
	bool right = enclosed(owner, op, x + 1, y);
	return up && down && left && right;
}


//	int bfsEdgeCases(int x, int y, int playerOwnership) {
//
//		int foundPlayer;
//		int queue[20] = [board[x][y]];
//
//
//		return foundPlayer
//	}


//9/6/22Endgame processes not finished and not tested
//At this point in the game's itteration, I'm fine with not having bfs procedure finished. 
//At bare minimum, I'd like to have manual input figured out


//Method for generating endgame procedures:
//We assume that some number of stones have been placed on the board already, 
//The first thing to do is go over every space, and see if there's any empty spaces adjacent to respective players stones
//This can be done easily with BFS
//Second, we need to look for edge cases
//Third, we get to a point where obvious, machine identifiable cases are impossible to determine, and let players decide.
//This is usually the case in actual Go games.
//If we were to just let the machine have the final say, users would be understandibly pissed
void endGamePlaceStones(int x, int y) {


	std::cout << boardMatrix[x][y].spaceOwnership;
	if (boardMatrix[x][y].spaceOwnership == 2) {
		boardMatrix[x][y].spaceOwnership = 10;
	}
	else if (boardMatrix[x][y].spaceOwnership == 10) {
		boardMatrix[x][y].spaceOwnership = 11;

	}
	else if (boardMatrix[x][y].spaceOwnership == 11) {
		boardMatrix[x][y].spaceOwnership = 2;
	}
}

void tallyFinalScores() {
	
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			if (boardMatrix[i][j].spaceOwnership == 10) {
				p1.playerScore += 1;
			}
			else {
				p2.playerScore += 1;
			}
		}
	}

}

void endGameProceedures() {
	int score;
	int owner, op;

	//1. Seek enclosed, empty spaces
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			
			if (boardMatrix[i][j].spaceOwnership != 2)
				owner = boardMatrix[i][j].spaceOwnership;
				if (owner == 0) {
					op == 1;
				}
				else {
					op == 2;
				}

				if (i + 1 < 19 && boardMatrix[i + 1][j].spaceOwnership == 2) {
					visited = {};
					if (enclosedEndGame(owner, op, i + 1, j)) {
						score = addStonesAddScore(owner);

						if (owner == 0) {
							p1.playerScore += score;
						}
						else {
							p2.playerScore += score;
						}
					}
					
				}

				if (i - 1 >= 0 && boardMatrix[i - 1][j].spaceOwnership == 2) {
					visited = {};
					if (enclosedEndGame(owner, op, i - 1, j)) {
						score = addStonesAddScore(owner);

						if (owner == 0) {
							p1.playerScore += score;
						}
						else {
							p2.playerScore += score;
						}
					}
				}


				if (j - 1 >= 0 && j < 19
					&& j < 19 && boardMatrix[i][j - 1].spaceOwnership == 2) {
					visited = {};
					if (enclosedEndGame(owner, op, i , j - 1)) {
						score = addStonesAddScore(owner);

						if (owner == 0) {
							p1.playerScore += score;
						}
						else {
							p2.playerScore += score;
						}
					}
				}

				if (j + 1 < 19 && boardMatrix[i][j + 1].spaceOwnership == 2) {
					visited = {};
					if (enclosedEndGame(owner, op, i, j + 1)) {
						score = addStonesAddScore(owner);

						if (owner == 0) {
							p1.playerScore += score;
						}
						else {
							p2.playerScore += score;
						}
					}
				}
		}

	}

	//2. Look for the obvious corner cases in a "bredth first" manner,
	  
	//3. Is handeled with Invisible button

	//4. tally final scores for each players

	tallyFinalScores();
}

void swapPlayerTurn()
{
	if (playerTurn == 0) {
		playerTurn = 1;
	}
	else {
		playerTurn = 0;
	}

}

//Creates drawable, non-interactable items
void updateBoard(int boardSize, float x, float y, int offset, float incriment)
{
	//Declaring draw variables
	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImU32 boardColor = ImGui::GetColorU32(ImVec4(242.f / 255.f, 176.f / 255.f, 109.f / 255.f, 1.f));
	ImU32 linesColor = ImGui::GetColorU32(ImVec4(13.f / 255.f, 2.f / 255.f, 1.f / 255.f, 1.f));


	//Draw board
	draw->AddRectFilled(ImVec2(x, y), ImVec2(x + boardSize, y + boardSize), boardColor, 0, 0);

	//Draw Lines on the board, simultaneously get the board coordinates
	//Verticle lines
	//Provisional variables to keep code DRY
	float xCord;
	float yCord;

	for (int i = 0; i < 19; i++) {

		xCord = x + incriment + i * incriment;
		draw->AddLine(ImVec2(xCord, y + incriment), ImVec2(xCord, y + boardSize - incriment), linesColor, 3.0f);
	}

	//Horizontal lines
	for (int i = 0; i < 19; i++) {

		yCord = y + i * incriment + incriment;
		draw->AddLine(ImVec2(x + incriment, yCord), ImVec2(x + boardSize - incriment, yCord), linesColor, 3.0f);
	}

	//Draw eyes on the board
	int points[3] = { 3, 9, 15 };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			draw->AddCircleFilled(ImVec2(x + incriment + points[i] * incriment, y + incriment + points[j] * incriment), 4.f, linesColor, 0);
		}
	}

}
//Creates drawable, interactable items
void updateBoardContents(int x, int y, float incriment)
{
	//For every intersection of values in boardX and boardY we need to create an invisible button, 
	//When clicked, the invisible button converts into a stone icon, and no longer is becomes clickable
	//The stone icon then displays on the item	

	stone activeSpace;
	int currentOwnership;
	float buttonStartX = 1.25 *incriment;
	float buttonStartY = 1.65 *incriment;
	int buttonID = 0;
	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImU32 blackStoneColor = ImGui::GetColorU32(ImVec4(13.f / 255.f, 2.f / 255.f, 1.f / 255.f, 1.f));
	ImU32 whiteStoneColor = ImGui::GetColorU32(ImVec4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));


	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			activeSpace = boardMatrix[i][j];
			currentOwnership = activeSpace.spaceOwnership;
			activeSpace.displayX = buttonStartX + i * incriment;
			activeSpace.displayY = buttonStartY + j * incriment;

			//Set button position,
			ImGui::SetCursorPos(ImVec2(activeSpace.displayX, activeSpace.displayY));

			if (currentOwnership == 2 || currentOwnership == 10 || currentOwnership == 11) {


				ImGui::PushID(buttonID);
				if (ImGui::InvisibleButton("SPC", ImVec2(incriment, incriment))) {

					if (gamePhase == 0) {
						placeStone(i, j);
						swapPlayerTurn();
						playerPass = 0;
					}
					else if (gamePhase == 1) {
						endGamePlaceStones(i, j);
						
					}
				}
				ImGui::PopID();
			}
			if (currentOwnership == 0 || currentOwnership == 10) {
				//Display a black stone
				draw->AddCircleFilled(ImVec2(x + incriment + i * incriment, y + incriment + j * incriment), 18.f, blackStoneColor, 0);

			}
			if (currentOwnership == 1 || currentOwnership == 11) {
				//Display a white stone
				draw->AddCircleFilled(ImVec2(x + incriment + i * incriment, y + incriment + j * incriment), 18.f, whiteStoneColor, 0);

			}

			buttonID++;
		}
	}
}


///Frontend 
// Methods strictly for rendering assets to IMGUI

void UseImGui::Init(GLFWwindow* window, const char* glsl_version) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//Setup platform/render bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();
}

void UseImGui::NewFrame() {
	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


void UseImGui::Update() {

	static float f = 0.0f;
	int boardSize = 900;
	float incriment = boardSize / 20; 
	//Arbitrary offset just for making it easier to look at everything
	int offset = 20;
	

	//ImGui::Begin("Example: Auto-resizing window", NULL, ImGuiWindowFlags_AlwaysAutoResize);              // Create a window called "Hello, world!" and append into it.
	ImGui::Begin("Go++");

	const ImVec2 p = ImGui::GetCursorScreenPos();
	float x = p.x + 4.0f + offset, y = p.y + 4.0f + offset;

	//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	

	static float clear_color[3]; // Edit 3 floats representing a color

	updateBoard(boardSize, x, y, offset, incriment);
	updateBoardContents(x, y, incriment);

	ImGui::SetNextWindowPos(ImVec2(x + boardSize + 100, y + 10));
	ImGui::BeginChild("Players",ImVec2(300, 900), true);
	ImGui::Text("Player 1");
	ImGui::SameLine();
	ImGui::Text("Player 2");
	ImGui::Text("Captured: %d", p1.playerScore);
	ImGui::SameLine();
	ImGui::Text("Captured: %d", p2.playerScore);
	ImGui::Text("Player Turn: %d", playerTurn + 1);
	
	
	
	//Doesnt work
	if (ImGui::Button("How To Play")) {
		ImGui::BeginPopupContextWindow();
		
		ImGui::Text("You play by eating the children!");
		ImGui::EndPopup();
	
	}
	
	


	//Does work
	if (ImGui::Button("Pass")) 
	{
		playerPass += 1;
		checkTwoPass();
		swapPlayerTurn();

	}
	

	if (ImGui::Button("Surrender")) {
		int winner;

		switch (playerTurn) {
		case 0:
			winner = 1;
			break;
		case 1:
			winner = 0;
			break;

		ImGui::BeginPopup("WINNER");
		ImGui::Text("%d WINS", winner);
		ImGui::EndPopup();
		}		
	}

	ImGui::EndChild();
	//ImGui::BeginChild("Player 2");
	//ImGui::EndChild();


	ImGui::End();




}

void UseImGui::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
void UseImGui::Shutdown() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}