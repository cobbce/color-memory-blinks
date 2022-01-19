////////////////
// State
////////////////

Game game = Game();
GameState* currentState;

WaitingToStart w = WaitingToStart(game);
SetupGame s = SetupGame(game);
PlayGame p = PlayGame(game);
GameOver g = GameOver(game);

GameState* waitingToStart = &w;
GameState* setupGame = &s;
GameState* playGame = &p;
GameState* gameOver = &g;

////////////////
// Utils
////////////////

void changeState(EGameState nextState) {
  switch(nextState) {
    case WAITING_TO_START:
      currentState = waitingToStart;
      break;
    case SETUP_GAME:
      currentState = setupGame;
      break;
    case PLAY_GAME:
      currentState = playGame;
      break;
    case GAME_OVER:
      currentState = gameOver;
      break;
  }

  currentState->initState();
  currentState->showColor();
}

////////////////
// Main
////////////////

void setup() {
  Serial.begin();
  changeState(WAITING_TO_START);
}

void loop() {
  currentState->loop();
}
