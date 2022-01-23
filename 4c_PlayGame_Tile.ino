
void PlayGame::runTile() {
  switch(tileState) {
    case TILE_SYNC_STATE:
      tileSyncState();
      break;
    case WAITING_TO_SHOW_MOVE:
      waitToShowMove();
      break;
    case WAITING_FOR_SHOW_MOVE_DONE:
      tileWaitForShowMoveDone();
      break;
    case TILE_WAITING_FOR_PLAYER_MOVE:
      tileWaitForPlayerMove();
    case WAIT_FOR_PLAYER_MOVE_ACK:
      waitForPlayerMoveAck();
  }
}

void PlayGame::tileSyncState() {
  if (getLastValueReceivedOnFace(game->leaderFace) == SYNC_STATE_DONE) {
    tileState = WAITING_TO_SHOW_MOVE;
  }
}

void PlayGame::waitToShowMove() {
  if (getLastValueReceivedOnFace(game->leaderFace) == SHOW_MOVE_FLAG) {
    setValueSentOnFace(SHOW_MOVE_FLAG, game->leaderFace);
    showAssignedColor();
    timer.set(SHOW_MOVE_DURATION);
    tileState = WAITING_FOR_SHOW_MOVE_DONE;
  } else if (getLastValueReceivedOnFace(game->leaderFace) == PLAYER_TURN_STARTED) {
    tileState = TILE_WAITING_FOR_PLAYER_MOVE;
  }
}

void PlayGame::tileWaitForShowMoveDone() {
  if (timer.isExpired()) {
    setValueSentOnFace(SHOW_MOVE_DONE_FLAG, game->leaderFace);
    turnOffColor();
    tileState = WAITING_TO_SHOW_MOVE;
  }
}

void PlayGame::tileWaitForPlayerMove() {
  if (buttonSingleClicked()) {
    setValueSentOnFace(PLAYER_MOVED_FLAG, game->leaderFace);
    tileState = WAIT_FOR_PLAYER_MOVE_ACK;
    timer.set(ACK_MOVE_DURATION);
    showAssignedColor();
  } else if (getLastValueReceivedOnFace(game->leaderFace) == GAME_WON) {
    game->isWinner = true;
  }
}

void PlayGame::waitForPlayerMoveAck() {
  if (getLastValueReceivedOnFace(game->leaderFace) == PLAYER_MOVED_ACK) {
    setValueSentOnFace(PLAY_GAME, game->leaderFace);
    moveAcked = true;
  }
  if (moveAcked && timer.isExpired()) {
    turnOffColor();
    moveAcked = false;
    tileState = TILE_WAITING_FOR_PLAYER_MOVE;
  }
}

void PlayGame::showAssignedColor() {
  game->color = assignedColor;
  showColor();
}

void PlayGame::turnOffColor() {
    game->color = OFF;
    showColor();
}
