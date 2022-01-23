////////////////
// State
////////////////

Game game = Game();
GameState* currentState;

WaitingToStart w = WaitingToStart(game);
SetupGame s = SetupGame(game);
PlayGame p = PlayGame(game);
GameOver g = GameOver(game);

////////////////
// Utils
////////////////

void changeState(EGameState nextState) {
  switch(nextState) {
    case WAITING_TO_START:
      currentState = &w;
      break;
    case SETUP_GAME:
      currentState = &s;
      break;
    case PLAY_GAME:
      currentState = &p;
      break;
    case GAME_OVER:
      currentState = &g;
      break;
  }

  currentState->init();
  currentState->showColor();
}

////////////////
// Main
////////////////

void setup() {
  randomize();
  Serial.begin();
  changeState(WAITING_TO_START);
}

void loop() {
  currentState->loop();
}
