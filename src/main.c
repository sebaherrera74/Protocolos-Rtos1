
/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "debouncetecla.h"
#include "sem_queues.h"
#include "Tareas.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "sapi.h"

DEBUG_PRINT_ENABLE;
void teclas_config(void);

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
	// ---------- CONFIGURACIONES ------------------------------
	// Inicializar y configurar la plataforma
	boardConfig();

	uartConfig( UART_USB, 115200 );
	uartWriteString( UART_USB,"Driver de espectrofotometros \r\n" );

	// Led para dar señal de vida
	gpioWrite( LED3, ON );

	// configura teclas para debounce
	teclas_config();

	/* funcion que crea semaforos y colas a utilizar */
	sem_queues_init();

	//CREACION DE TAREAS EN  freeRTOS

	//-------------Tarea update de tecla 1,2 para cambiar valor de longitud de onda--------------

	BaseType_t res =xTaskCreate(tarea_tecla,                     // Funcion de la tarea a ejecutar
			(const char *)"update Teclas",   // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,      // Cantidad de stack de la tarea
			tecla_config,                    // Parametros de tarea
			tskIDLE_PRIORITY+1,              // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema

	if (res == pdFAIL) {
		ERROR_CREACION_TAREAS;

	}

	//----------Tarea que incrementara o decrementara el valor de longitud de onda para el ensayo
	res  =xTaskCreate(tarea_seteo_LO,                  // Funcion de la tarea a ejecutar
			(const char *)"tarea set LO ",    // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,        // Cantidad de stack de la tarea
			tecla_config,                                // Parametros de tarea
			tskIDLE_PRIORITY+1,               // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema

	if (res == pdFAIL) {
		ERROR_CREACION_TAREAS;
	}

	//----------Tarea inicio de ensayo -----------------------------
	res  =xTaskCreate(tarea_inicio,                  // Funcion de la tarea a ejecutar
			(const char *)"tarea inicio_ensayo ",    // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,        // Cantidad de stack de la tarea
			tecla_config,                                // Parametros de tarea
			tskIDLE_PRIORITY+1,               // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema

	if (res == pdFAIL) {
		ERROR_CREACION_TAREAS;
	}

	//-------------Tarea de informe de situacion por uart  -----------
	res =xTaskCreate(msjexuart,                  // Funcion de la tarea a ejecutar
			(const char *)"tarea msje uart ",    // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,        // Cantidad de stack de la tarea
			0,                                // Parametros de tarea
			tskIDLE_PRIORITY+1,               // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema

	if (res == pdFAIL) {
		ERROR_CREACION_TAREAS;
	}

	//-------------Tarea de posicionar el motor de acuerdo a la longitud seleccionada-----------
	res =xTaskCreate(tarea_posicionamiento,                  // Funcion de la tarea a ejecutar
			(const char *)"tarea posicionar ",    // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,        // Cantidad de stack de la tarea
			0,                                // Parametros de tarea
			tskIDLE_PRIORITY+1,               // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema

	if (res == pdFAIL) {
		ERROR_CREACION_TAREAS;
	}

	// Inicia Scheduler

	vTaskStartScheduler();

	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE ) {

		// Si cae en este while 1 significa que no pudo iniciar el scheduler
	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}

void teclas_config(void){

	tecla_config[0].tecla= TEC2;
	tecla_config[0].sem_tec_pulsada	= xSemaphoreCreateBinary();

	tecla_config[1].tecla= TEC3;
	tecla_config[1].sem_tec_pulsada	= xSemaphoreCreateBinary();

	tecla_config[2].tecla= TEC1;
	tecla_config[2].sem_tec_pulsada	= xSemaphoreCreateBinary();

	tecla_config[3].tecla= TEC4;
	tecla_config[3].sem_tec_pulsada = xSemaphoreCreateBinary();
}





