#include "TUM_Draw.h"
#include "TUM_Event.h"
#include "TUM_Ball.h"
#include "TUM_Sound.h"
#include "TUM_Utils.h"
#include "TUM_Print.h"

#include "defines.h"
#include "main.h"
#include "demo_tasks.h"
#include "draw.h"
#include "async_message_queues.h"
#include "async_sockets.h"
#include "buttons.h"
#include "sm.h"
#include "buttons.h"
#include "bird.h"
#include "pipes.h"

TaskHandle_t Game = NULL;
TaskHandle_t DemoTask2 = NULL;
TaskHandle_t SinglePlayer = NULL;
TaskHandle_t GameOver = NULL;
TaskHandle_t CheatMode = NULL;
TaskHandle_t ViewScores = NULL;
TaskHandle_t StartSingle = NULL;

// Task to just pereodically run the state machine
void vStatesTask(void *pvParameters)
{
	while (1)
	{
		states_run();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void vTaskGame(void *pvParameters)
{

	// vDrawBird(); // vDrawInitAnimations()

	TickType_t xLastFrameTime = xTaskGetTickCount();

	while (1)
	{
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
				pdTRUE)
			{
				tumEventFetchEvents(FETCH_EVENT_NONBLOCK |
									FETCH_EVENT_NO_GL_CHECK);
				xGetButtonInput();

				if (xSemaphoreTake(buttons.lock, 0) == pdTRUE)
				{

					if (tumEventGetMouseLeft() && vCheckMenuMouse())
						states_set_state(1);
					xSemaphoreGive(buttons.lock);
				}

				vDrawBackground();
				vDrawBase();
				vDrawmenu();

				vDrawSpriteAnimations(xLastFrameTime);
				xLastFrameTime = xTaskGetTickCount();
			}

		// vTaskDelay(20);
	}
}

void vDemoTask2(void *pvParameters)
{
	while (1)
	{
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
				pdTRUE)
			{
				tumEventFetchEvents(FETCH_EVENT_NONBLOCK |
									FETCH_EVENT_NO_GL_CHECK);
				// xGetButtonInput();
				// tumDrawBindThread();
				vDrawBackground();
				vDrawSubmenu();

				if (xSemaphoreTake(buttons.lock, 0) == pdTRUE)
				{

					if (tumEventGetMouseLeft() && vCheckSingle())
						states_set_state(2);
					if (tumEventGetMouseLeft() && vCheckCheatMode())
						states_set_state(4);
					if (tumEventGetMouseLeft() && vCheckViewScores())
						states_set_state(5);

					xSemaphoreGive(buttons.lock);
				}

				// vTaskDelay(20);
			}
	}
}

void vTaskSingle(void *pvParameters)
{
	vDrawBird();
	vDrawBase();
	pipesInit();
	birdInit();
	TickType_t xLastFrameTime = xTaskGetTickCount();

	while (1)
	{
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
				pdTRUE)
			{
				tumEventFetchEvents(FETCH_EVENT_NONBLOCK |
									FETCH_EVENT_NO_GL_CHECK);
				xGetButtonInput();

				// if (xSemaphoreTake(buttons.lock, 0) == pdTRUE)
				// {

				// 	if (tumEventGetMouseLeft() && vCheckSingleTest())
				// 		states_set_state(3);

				// 	xSemaphoreGive(buttons.lock);
				// }

				vDrawBackground();
				vDrawPipes();
				vDrawScore();
				vCheckCollision();
				vBirdStatus();
				vBirdMovement();
				vDrawSpriteAnimations(xLastFrameTime);
				xLastFrameTime = xTaskGetTickCount();
			}
	}
}

void vTaskStartSingle(void *pvParameters)
{

	vDrawBase();
	vDrawBird();
	birdInit();

	while (1)
	{
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
				pdTRUE)
			{
				tumEventFetchEvents(FETCH_EVENT_NONBLOCK |
									FETCH_EVENT_NO_GL_CHECK);
				xGetButtonInput();

				vDrawBackground();
				vDrawStartSingle();
			}
	}
}

void vTaskGameOver(void *pvParameters)
{
	while (1)
	{
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
				pdTRUE)
			{
				tumEventFetchEvents(FETCH_EVENT_NONBLOCK |
									FETCH_EVENT_NO_GL_CHECK);
				xGetButtonInput();

				vDrawBackground();

				vDrawGameOver();

				if (xSemaphoreTake(buttons.lock, 0) == pdTRUE)
				{

					if (tumEventGetMouseLeft() && vCheckReplay())
						vBirdReset();
						states_set_state(2);
					if (tumEventGetMouseLeft() && vCheckGameOverBack())
						states_set_state(1);

					xSemaphoreGive(buttons.lock);
				}

				// vTaskDelay(20);
			}
	}
}

void vCheatMode(void *pvParameters)
{
	vDrawBird();
	vDrawBase();
	pipesInit();
	birdInit();
	TickType_t xLastFrameTime = xTaskGetTickCount();

	while (1)
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
				pdTRUE)
			{
				tumEventFetchEvents(FETCH_EVENT_NONBLOCK |
									FETCH_EVENT_NO_GL_CHECK);
				xGetButtonInput();

				vDrawBackground();

				vDrawCheatMode();

				vDrawPipes();
				vDrawScore();

				vBirdStatus();
				vBirdMovement();
				vDrawSpriteAnimations(xLastFrameTime);
				xLastFrameTime = xTaskGetTickCount();

				if (xSemaphoreTake(buttons.lock, 0) == pdTRUE)
				{

					if (buttons.buttons[KEYCODE(ESCAPE)]) //tumEventGetMouseLeft() && vCheckCheatModeBack()
						states_set_state(1);

					xSemaphoreGive(buttons.lock);
				}

				// vTaskDelay(20);
			}
}

void vViewScores(void *pvParameters)
{
	while (1)
	{
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
				pdTRUE)
			{
				tumEventFetchEvents(FETCH_EVENT_NONBLOCK |
									FETCH_EVENT_NO_GL_CHECK);
				xGetButtonInput();

				vDrawBackground();

				vDrawCheatMode();

				if (xSemaphoreTake(buttons.lock, 0) == pdTRUE)
				{

					if (tumEventGetMouseLeft() && vCheckCheatModeBack())
						states_set_state(1);

					xSemaphoreGive(buttons.lock);
				}

				// vTaskDelay(20);
			}
	}
}

int createTasks(void)
{
	xTaskCreate(vTaskGame, "Game", mainGENERIC_STACK_SIZE * 20, NULL,
				mainGENERIC_PRIORITY + 4, &Game);
	xTaskCreate(vDemoTask2, "DemoTask2", mainGENERIC_STACK_SIZE * 2, NULL,
				mainGENERIC_PRIORITY + 1, &DemoTask2);
	xTaskCreate(vTaskSingle, "SinglePlayer", mainGENERIC_STACK_SIZE * 2, NULL,
				mainGENERIC_PRIORITY + 4, &SinglePlayer);
	xTaskCreate(vTaskGameOver, "Game OVer", mainGENERIC_STACK_SIZE * 2, NULL,
				mainGENERIC_PRIORITY + 1, &GameOver);
	xTaskCreate(vCheatMode, "Cheat Mode", mainGENERIC_STACK_SIZE * 2, NULL,
				mainGENERIC_PRIORITY + 1, &CheatMode);
	xTaskCreate(vViewScores, "View Scores", mainGENERIC_STACK_SIZE * 2, NULL,
				mainGENERIC_PRIORITY + 1, &ViewScores);

	xTaskCreate(vTaskStartSingle, "Start Single", mainGENERIC_STACK_SIZE * 2, NULL, mainGENERIC_PRIORITY + 1, &StartSingle);

	vTaskSuspend(Game);
	vTaskSuspend(DemoTask2);
	vTaskSuspend(SinglePlayer);
	vTaskSuspend(GameOver);
	vTaskSuspend(CheatMode);
	vTaskSuspend(ViewScores);
	vTaskSuspend(StartSingle);

	return 0;
}

void deleteTasks(void)
{
	if (Game)
	{
		vTaskDelete(Game);
	}
	if (DemoTask2)
	{
		vTaskDelete(DemoTask2);
	}
	if (SinglePlayer)
	{
		vTaskDelete(SinglePlayer);
	}
	if (GameOver)
	{
		vTaskDelete(GameOver);
	}
	if (CheatMode)
	{
		vTaskDelete(CheatMode);
	}
	if (ViewScores)
	{
		vTaskDelete(ViewScores);
	}
	if (StartSingle)
	{
		vTaskDelete(StartSingle);
	}
}