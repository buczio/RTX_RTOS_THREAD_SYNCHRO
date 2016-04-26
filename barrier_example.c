
#include "cmsis_os.h"
#include "MKL25Z4.h"                   
#include "Board_LED.h"
void threadBaseCode (void const *argument) ;
 
osThreadDef(threadBaseCode, osPriorityNormal, 5, 0);
osThreadId tsk2_0,tsk2_1,tsk2_2,tsk2_3,tsk2_4;			//Define the task ID variables

osSemaphoreId Turnstile;									
osSemaphoreDef(Turnstile);
osSemaphoreId Turnstile2;									
osSemaphoreDef(Turnstile2);
osSemaphoreId Mutex;									
osSemaphoreDef(Mutex);
unsigned int count = 0;
unsigned int param[5] = { 0x1,0x2,0x4,0x8,0x10};

void threadBaseCode  (void const *argument) 			
{
uint32_t LED_data;
	
	LED_data = (uint32_t) argument;
	while(1)					
	{
		
//---------------------------------------------------------------------------------							

		osSemaphoreWait(Mutex,0xffff);			 				//Allow one task at a time to access the first turnstile
		count = count+1;					 									// Increment count
		if( count == 5) 					 									//When last section of code reaches this point run his code
		{
			osSemaphoreWait (Turnstile2,0xffff);	 		//Lock the second turnstile
			osSemaphoreRelease(Turnstile);				 		//Unlock the first turnstile
		}			
		osSemaphoreRelease(Mutex);									//Allow other tasks to access the turnstile
		osSemaphoreWait(Turnstile,0xFFFF);					//Turnstile Gate			
		osSemaphoreRelease(Turnstile);

//----------------------------------- GATE -----------------------------------

		LED_On(LED_data);
		osDelay(100);				   							//Critical instructions 
		LED_Off(LED_data);
		osDelay((LED_data*100));	
//---------------------------------- GATE ------------------------------------

		osSemaphoreWait(Mutex,0xffff);			 				//Allow one task at a time to access the turnstile
		count = count - 1;
		if(count ==0)						 										//When last section of code reaches this point run his code
		{
			osSemaphoreWait(Turnstile,0xffff);		 		//Lock the second turnstile
			osSemaphoreRelease(Turnstile2);			 			//Unlock the first turnstile
		}
		osSemaphoreRelease(Mutex);					 				//Allow other tasks to access the turnstile
		osSemaphoreWait(Turnstile2,0xffff);		 			//Turnstile Gate
		osSemaphoreRelease(Turnstile2);

//------------------------------------------------------------------------------		

	}
}		

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) 
{
	osKernelInitialize ();                    
	LED_Initialize();
	Turnstile =	osSemaphoreCreate(osSemaphore(Turnstile), 0); // 0 tokens
	Turnstile2 = osSemaphoreCreate(osSemaphore(Turnstile2), 1); // 1 token
	Mutex =	osSemaphoreCreate(osSemaphore(Mutex), 1);	
	tsk2_0 = osThreadCreate(osThread(threadBaseCode),(void *) 1U);
	tsk2_1 = osThreadCreate(osThread(threadBaseCode),(void *) 2U); 
	tsk2_2 = osThreadCreate(osThread(threadBaseCode),(void *) 3U);
	tsk2_3 = osThreadCreate(osThread(threadBaseCode),(void *) 4U);
	tsk2_4 = osThreadCreate(osThread(threadBaseCode),(void *) 5U);

	osKernelStart ();                         
}

