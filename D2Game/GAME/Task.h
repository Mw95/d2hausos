#ifndef D2RE_D2GAME_TASK_H
#define D2RE_D2GAME_TASK_H

#include "../D2game.h"

void __cdecl TaskInitTimer(void);
void __cdecl TaskFreeQueues(void);
int __cdecl TaskCreateQueue(void);
BOOL __fastcall TaskQueueGame(TASK *ptTask, DWORD eType, DWORD dwGameId);
int __fastcall TaskGetDue(DWORD nQueue, TASK **pptTask);
int __fastcall TaskGetWait(DWORD nQueue);
void __fastcall TaskProcessGame(DWORD nQueue, TASK *ptTask);

#endif
