////////////////
// PlayGame
////////////////

void PlayGame::loopForState() {
  checkButtonForStateChange(GAME_OVER);
}

////////////////
// GameOver
////////////////

void GameOver::loopForState() {
  checkButtonForStateChange(WAITING_TO_START);
}
