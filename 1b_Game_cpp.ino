void GameState::loop() {
  loopForState();
  broadcastCurrentState();
  checkNeighborsForStateChange();
}

void GameState::showColor() {
  setColor(color);
}
    
void GameState::checkNeighborsForStateChange() {
  FOREACH_FACE(face) {
    if (hasNeighbor(face)) {
      byte neighborState = getNeighborState(face);
      if (stateChangeDetected(neighborState)) {
        changeState(neighborState);
      }
    }
  }
}

bool GameState::hasNeighbor(byte face) {
  return !isValueReceivedOnFaceExpired(face);
}

byte GameState::getNeighborState(byte face) {
  return getLastValueReceivedOnFace(face);
}

void GameState::broadcastCurrentState() {
  setValueSentOnAllFaces(state);
}

bool GameState::stateChangeDetected(EGameState neighborState) {
  if (neighborState == nextState) {
    return true;
  }
  return false;
}

void GameState::checkButtonForStateChange(EGameState nextState) {
  if (buttonSingleClicked()) {
    changeState(nextState);
  }
}
