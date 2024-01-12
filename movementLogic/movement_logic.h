#include <stdbool.h>

void updateMovePlayer(int*, char);
void updateMovePlayerWall(int*, bool);
void updateMoveVerticalHorizontal(int*, bool);
void updateMoveUp(int*);
void updateMoveUpPlayer(int*);
void updateMoveDown(int*);
void updateMoveDownPlayer(int*);
void updateMoveRight(int*);
void updateMoveRightPlayer(int*);
void updateMoveLeft(int*);
void updateMoveLeftPlayer(int*);
void initialPosition();
void updateMoveInDirection(int*,int);
void updateMoveOnPlayerInDirection(int*, int);