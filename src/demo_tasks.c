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
#include "state_machine.h"
#include "bird.h"
#include "pipes.h"

#include <stdbool.h>

TaskHandle_t Game = NULL;

void vTaskGame(void *pvParameters)
{
	vDrawBird(); //vDrawInitAnimations()
	vDrawBase();
	pipesInit();

	TickType_t xLastFrameTime = xTaskGetTickCount();

    player1.birdPosition = (SCREEN_HEIGHT - tumDrawGetLoadedImageHeight(base_image2)) / 2;
	player1.birdVelocity = 0.0f;
	player1.gravity = 1.0f;


	while (1) {
		if (DrawSignal)
			if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
			    pdTRUE) {
				tumEventFetchEvents(FETCH_EVENT_NONBLOCK |
						    FETCH_EVENT_NO_GL_CHECK);
				xGetButtonInput();
				
				vDrawBackground();
				vDrawPipes();

				vBirdStatus(player1);
				vBirdMovement();
				vDrawSpriteAnimations(xLastFrameTime);
				xLastFrameTime = xTaskGetTickCount();

				
				//vCheckStateInput();
				//vTaskDelay(10);
			}
	}
}

int createTasks(void)
{
	xTaskCreate(vTaskGame, "Game", mainGENERIC_STACK_SIZE * 20, NULL,
		    mainGENERIC_PRIORITY + 1, &Game);
	//vTaskSuspend(Game);

	return 0;
}

void deleteTasks(void)
{
	if (Game) {
		vTaskDelete(Game);
	}
}