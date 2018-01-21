#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>

extern char **getln();

typedef struct CPU {
	int CPUNumber;
	int CPUTime;
	int IOTime;
	
} CPUBurst;

typedef struct Thread {
	int threadNumber;
	int arrivalTime;
	int numberOfCPU;
	
	CPUBurst CPUBursts[100];	
	
} thread;

typedef struct Process {
	int processNumber;
	int numberOfThreads;
	thread threads[51];
		
	struct Process *next;	
} process;


process *head = NULL;

/*
 displayMessagePrompt: This function displays the perticular message. It takes an string and appends to the default message.
 Pre: It takes a message string i.e. message to display.
 Post: It does not return anything.
*/
void displayMessagePrompt(char* msg);

/*
 displayMessage: This function displays the perticular message. It doesn't have any default message.
 Pre: It takes a message string i.e. message to display.
 Post: It does not return anything.
*/
void displayMessage(char* msg);

/*
 getArgumentLength: This function counts and returns the length of the arguments specified in args array.
	Example: if the args array contains simcpu < testcase1.txt, it will return 3.
 Pre: It takes args array and counts the length of the arguments.
 Post: It returns the total number of arguments.
*/
int getArgumentLength(char** args);

/*
 getArgumentOnPosition: This function returns the argument present in a specified position.
 Pre: It takes args array and position integer starting with 0.
 Post: It returns the argument on that perticular position.
*/
char* getArgumentOnPosition(char** args, int position);

/*
 readLineByLine: This function takes a file pointer and reads and returns a line in a file. It needs to be called n times to read
	n lines in a file.
 Pre: It takes FILE pointer.
 Post: It returns the line in a file.
*/
char* readLineByLine(FILE *fp); 

/*
 isValidInt: This function takes a string and checks if its a valid Integer.
 Pre: It takes a string str.
 Post: It returns 1 if its a valid Integer or else 0.
*/
int isValidInt(char* str);

/*
 getThreadInfo: This function parses the struct to calculate total CPU, total IO and total threads. It takes the address of 3 variables 
	stores the values in them.
 Pre: It takes address of totalCPU, totalIO and totalThrads to store the values.
 Post: It doesn't return anything.
*/
void getThreadInfo(int *totalThreads, float *totalCPU, int *totalIO);

/*
 isListEmpty: This function checks if the struct is empty or not. If it is empty then it return 1 or else 0.
 Pre: It doesn't take any arguments.
 Post: It returns 1 or 0 based on if the struct is empty or not.
*/
int isListEmpty();

/*
 lengthOfList: This function calculates and returns the lengeth of list.
 Pre: It doesn't take any arguments.
 Post: It returns the length of list.
*/
int lengthOfList();

/*
 deleteAndReturnFirstNode: This function deletes and returns the first node of the struct.
 Pre: It doesn't take any arguments.
 Post: It returns deleted node of the struct.
*/
process* deleteAndReturnFirstNode();

/*
 eventSimulation: This function simulates algorithm and calculates total time and total turn around time.
 Pre: It takes address of total time to be stored, verboseMode and detailedInfoMode and address of total turn around time to 
	  be stored.
 Post: It calculates and stores totalTime and totalTurnaroundTime and doesn't return anything.
*/
void eventSimulation(int* totalTime, int verboseMode, int detailedInfoMode, float* totalTurnaroundTime);

/*
 displayOutputDetails: This function displays the output details like total IO time, total service time, total time etc 
	based on the mode selected i.e. verbose and detailed info.
 Pre: It takes various parameters to display like total time, total service time, total IO time etc.
 Post: It doesn't return anything.
*/
void displayOutputDetails(int verboseMode, int detailedInfoMode, int totalArrivalTime, int ran, int threadNum, int processNum, int totalTime, int totalServiceTime, int totalIOTime, int turnaroundTime);

/*
 displayDefaultOutputDetails: This function displays the default output details like total time, average turn around time 
	and CPU utilization.
 Pre: It takes various parameters to display like total time, average turn around time and CPU utilization.
 Post: It doesn't return anything.
*/
void displayDefaultOutputDetails(int verboseMode, int detailedInfoMode, int roundRobinMode, int quantum, int totalTime, float averageTurnaroundTime, float CPUUtilization);

/*
 freeMemory: This function is used to free struct memory.
 Pre: It doesn't take any arguments.
 Post: It doesn't return anything.
*/
void freeMemory();

int main() {



	char **args;
	char* errMsg = NULL;
	char* msg = NULL;
	FILE * fp;
	
	while(1) {
		
			int detailedInfoMode = 0;
			int verboseMode = 0;
			int roundRobinMode = 0;
			int quantum = 0;
			int isValidCommand = 0;
			int argsLength = 0;
			int numberOfProcess = 0;
			int threadSwitch = 0;
			int processSwitch = 0;
			char* fileName = NULL;
			char* numLine = NULL;
			
						
			head = NULL;
			int totalThreads = 0;
			float totalCPU = 0;
			int totalIO = 0;
			int totalTime = 0;
			float totalTurnaroundTime = 0;
			float averageTurnaroundTime = 0.0;
			float CPUUtilization = 0;	
	
	
		args = getln();
		
		if (args[0] == NULL) {
			/* If no command is entered, display this message */
			errMsg=(char*)malloc(70 * sizeof(char));
			strcpy(errMsg, "Please enter a command.");
			displayMessagePrompt(errMsg);
			free(errMsg);
			
		} else {
			
			argsLength = getArgumentLength(args);
			
			/* Exit the shell */
			if ((strcmp(args[0], "exit")) == 0) {
				if (argsLength == 1) {
					msg = (char*)malloc(70 * sizeof(char));
					strcpy(msg, "Thank you for using my program.");
					displayMessage(msg);
					free(msg);
					break;
				} 
				
			} else {
				
				
				if ((strcmp(args[0], "simcpu")) == 0) {
					
					
					/* Only simcpu command is entered */
					if ((argsLength == 1) || (argsLength == 2)) {
						
						/* If no file name is specified, display this message */
						errMsg=(char*)malloc(70 * sizeof(char));
						strcpy(errMsg, "Please enter a file name.");
						displayMessagePrompt(errMsg);
						free(errMsg);
								
					/* Valid argument - FCFS mode : simcpu < inputfile*/			
					} else if ((argsLength == 3) && (strcmp(args[1], "<")) == 0) {
						isValidCommand = 1;	
						
						
						
					/* Valid argument - FCFS mode : simcpu -d < inputfile*/	
					} else if ((argsLength == 4) && ((strcmp(args[1], "-d")) == 0) && ((strcmp(args[2], "<")) == 0)) {
						isValidCommand = 1;
						detailedInfoMode = 1;
						
						
						
					/* Valid argument - FCFS mode : simcpu -v < inputfile*/		
					} else if ((argsLength == 4) && ((strcmp(args[1], "-v")) == 0) && ((strcmp(args[2], "<")) == 0)) {
						isValidCommand = 1;
						verboseMode = 1;	
						
						
					/* Valid argument - RR mode : simcpu -r 10 < inputfile*/	
					} else if ((argsLength == 5) && ((strcmp(args[1], "-r")) == 0) && ((strcmp(args[3], "<")) == 0)) {
						isValidCommand = 1;
						roundRobinMode = 1;
						quantum = atoi(args[2]);	
						
						
					/* Valid argument - FCFS mode : simcpu -d -v < inputfile*/		
					} else if ((argsLength == 5) && ((strcmp(args[1], "-d")) == 0) && ((strcmp(args[2], "-v")) == 0) && ((strcmp(args[3], "<")) == 0)) {
						isValidCommand = 1;
						detailedInfoMode = 1;
						verboseMode = 1;	
					
					
					
					/* Valid argument - RR mode : simcpu -d -r 10 < inputfile*/
					}  else if ((argsLength == 6) && ((strcmp(args[1], "-d")) == 0) && ((strcmp(args[2], "-r")) == 0) && (( strcmp(args[4], "<")) == 0)) {
						isValidCommand = 1;
						detailedInfoMode = 1;
						roundRobinMode = 1;
						quantum = atoi(args[3]);		
					
					
					/* Valid argument - RR mode : simcpu -v -r 10 < inputfile*/
					} else if ((argsLength == 6) && ((strcmp(args[1], "-v")) == 0) && ((strcmp(args[2], "-r")) == 0) && ((    strcmp(args[4], "<")) == 0)) {
						isValidCommand = 1;
						verboseMode = 1;
						roundRobinMode = 1;
						quantum = atoi(args[3]);
					
					/* Valid argument - RR mode : simcpu -d -v -r 10 < inputfile*/		
					} else if ((argsLength == 7) && ((strcmp(args[1], "-d")) == 0) && ((strcmp(args[2], "-v")) == 0) && ((strcmp(args[3], "-r")) == 0) && ((strcmp(args[5], "<")) == 0)) {
						isValidCommand = 1;
						detailedInfoMode = 1;
						verboseMode = 1;	
						roundRobinMode = 1;
						quantum = atoi(args[4]);	
						
						
					}  else {
						
						/* If invalid command argument is entered, display this message */
						errMsg=(char*)malloc(200 * sizeof(char));
						strcpy(errMsg, "Please enter a valid command argument. Make sure you don't have multiple blank spaces between the arguments.");
						displayMessagePrompt(errMsg);
						free(errMsg);
						
					}
					
					
					if (isValidCommand) {
						
						fileName=(char*)malloc(100 * sizeof(char));
						strcpy(fileName, args[argsLength -1]);
						
						if ((fp=fopen(fileName,"r"))==NULL){
						    msg = (char*)malloc(70 * sizeof(char));
							strcpy(msg, "Error in opening file.");
							displayMessage(msg);
							free(msg);
						    exit(1);         /* Program exits if file pointer returns NULL. */
						}
						
						int lengthCount = 0;
												
						
						numLine = readLineByLine(fp);
						
						if (numLine != NULL) {
							int numCount = 0;
							lengthCount++;
							while (numLine != NULL)  {
								numCount++;
								if (lengthCount == 1) {
																		
									if (numCount <= 3 ) {
										if (numCount == 1) {
											numberOfProcess = atoi(numLine);
										} else if(numCount == 2) {
											threadSwitch = atoi(numLine);
										} else if(numCount == 3) {
											processSwitch = atoi(numLine);
										}
																		
									}
										
									
								} 
														
									
								numLine = strtok(NULL, " ");
							}
						}
						
						/*  Remove Later */
						printf("%d %d %d \n", numberOfProcess, threadSwitch, processSwitch);
						/* **************** */
				
						
				for(int i = 0; i < numberOfProcess; i++) {	
							process *newNode = (process*) malloc(sizeof(process));
							
														
							numLine = readLineByLine(fp); /* read a line from the file */
								if (numLine != NULL) {
									int numCount = 0;
									lengthCount++;
									while (numLine != NULL)  {
										numCount++;
										
										
											if (numCount == 1) {
												newNode->processNumber = atoi(numLine);
											} else if (numCount == 2) {
												newNode->numberOfThreads = atoi(numLine);
												
											}
											
											
											/*  Remove Later */
											if (numCount == 2) {
												printf("Process : %d %d \n", newNode->processNumber, newNode->numberOfThreads);
											}
											
											/* **************** */
										
																									
											
										numLine = strtok(NULL, " ");
									}
										

										char* tempLine = NULL;	
										
										for(int j = 0; j < newNode->numberOfThreads; j++) {		
											
											tempLine = readLineByLine(fp);
											if (tempLine != NULL) {
												int numTempCount = 0;
												lengthCount++;
												while (tempLine != NULL)  {
													numTempCount++;
													
													
														if (numTempCount == 1) {
															newNode->threads[j].threadNumber = atoi(tempLine);
														} else if (numTempCount == 2) {
															newNode->threads[j].arrivalTime = atoi(tempLine);
														} else if(numTempCount == 3) {
															  newNode->threads[j].numberOfCPU = atoi(tempLine);
														}
														
													  
													  	/*  Remove Later */
														 if (numTempCount == 3) {
															printf("Thread Info: %d %d %d \n", newNode->threads[j].threadNumber, newNode->threads[j].arrivalTime, newNode->threads[j].numberOfCPU);
														} 
														
														/* **************** */
													  
													
													
													tempLine = strtok(NULL, " ");
												   }
													
													char* tempTwoLine = NULL;		
													
													for(int k = 0; k < newNode->threads[j].numberOfCPU; k++) {		
														
														tempTwoLine = readLineByLine(fp);														
														if (tempTwoLine != NULL) {
															int numTempTwoCount = 0;
															lengthCount++;
															while (tempTwoLine != NULL)  {
																numTempTwoCount++;
																
																
																	if (numTempTwoCount == 1) {
																		newNode->threads[j].CPUBursts[k].CPUNumber = atoi(tempTwoLine);
																	} else if (numTempTwoCount == 2) {
																		newNode->threads[j].CPUBursts[k].CPUTime = atoi(tempTwoLine);
																		if (k == newNode->threads[j].numberOfCPU-1) {
																			newNode->threads[j].CPUBursts[k].IOTime = 0;
																			numTempTwoCount++;
																		}
																	} else if (numTempTwoCount == 3 ) {
																		
																			newNode->threads[j].CPUBursts[k].IOTime = atoi(tempTwoLine);
																		
																		
																	}
																	
																		/*  Remove Later */
																		if (numTempTwoCount == 3) {
																			printf("CPU Info: %d %d %d \n", newNode->threads[j].CPUBursts[k].CPUNumber,newNode->threads[j].CPUBursts[k].CPUTime, newNode->threads[j].CPUBursts[k].IOTime);
																		}
																		
																		/* **************** */
																																	
																	
																																
																															
																tempTwoLine = strtok(NULL, " ");
															}
														}
														
													}	

																			
														
												
											}	
											
											
										}
										
														
										
								}	
							
								newNode->next = head;	
								head = newNode;	
							    
						
						
						
						}	
									
						
						lengthCount = 0;
						fclose(fp);
						
						getThreadInfo(&totalThreads, &totalCPU, &totalIO); /* get the info related to thread */
						
						eventSimulation(&totalTime, verboseMode, detailedInfoMode, &totalTurnaroundTime);
						averageTurnaroundTime = totalTurnaroundTime / totalThreads; /* calculate average turnaround time */
						
						CPUUtilization = (float)(totalCPU / totalTime * 100); /* calculte CPU utilization */
					
						displayDefaultOutputDetails(verboseMode, detailedInfoMode, roundRobinMode, quantum, totalTime,  averageTurnaroundTime, CPUUtilization);	
						
						freeMemory();	
					}
					
					
									
					
				} else {
					
					/* If invalid command is entered, display this message */
					errMsg=(char*)malloc(70 * sizeof(char));
					strcpy(errMsg, "Please enter a valid command i.e. exit / simcpu.");
					displayMessagePrompt(errMsg);
					free(errMsg);
					
				}
				
				
				
			}  
			
			
		}
		
		
		
		if (isValidCommand) {
			free(fileName);
		}
		
		
	}

		
	return 0;	
}

/*
  This function displays the perticular message. It takes an string and appends to the default message.
 */
void displayMessagePrompt(char* msg) {
	printf("\n------------------------------------------------------------------------------ \n");
	printf("Error during command processing -  %s\n\n", msg);
	printf("------------------------------------------------------------------------------ \n");
}

/*
 This function displays the perticular message. It doesn't have any default message.
 
*/
void displayMessage(char* msg) {
	printf("\n------------------------------------------------------------------------------ \n");
	printf(" %s\n\n", msg);
	printf("------------------------------------------------------------------------------ \n");
}

/*
This function counts and returns the length of the arguments specified in args array.
	Example: if the args array contains simcpu < testcase1.txt, it will return 3.
*/
int getArgumentLength(char** args) {
	int i =0;
	for(i = 0; args[i] != NULL; i++) {
			
	}	
	return i;
}

/*
 This function returns the argument present in a specified position.
*/
char* getArgumentOnPosition(char** args, int position) {
	char *arg = NULL;
	
	if (position >= 0) {
		arg=(char*)malloc(100 * sizeof(char));
		for(int i = 0; args[i] != NULL; i++) {
			if (i == position) {
				strcpy(arg, args[i]);
			}
		
	    }
	} else {
		return NULL;
	}
		
	return  (char *)arg;

}

/*
This function takes a file pointer and reads and returns a line in a file. It needs to be called n times to read
	n lines in a file.
*/
char* readLineByLine(FILE *fp) {
	char line[500];
	fgets(line, sizeof line, fp);
	char *p = NULL;
	if (line != NULL) {
		
							
		p = strtok(line," ");
	}
	
	return (char *) p;
}

/*
 This function takes a string and checks if its a valid Integer.
 */
int isValidInt(char* str) {
	while (*str) {
      if (!isdigit(*str)) {
		return 0;  
	  } else {
		++str;  
	  }
     
         
   }

   return 1;
	
}

/*
This function parses the struct to calculate total CPU, total IO and total threads. It takes the address of 3 variables 
	stores the values in them.
*/
void getThreadInfo(int *totalThreads, float *totalCPU, int*totalIO) {
	
	process *node = head;
	
	while(node != NULL) {
		*totalThreads += node->numberOfThreads;
		
		for(int i = 0; i < node->numberOfThreads; i++) {
			for(int j = 0; j < node->threads[i].numberOfCPU; j++) {
				*totalCPU += node->threads[i].CPUBursts[j].CPUTime;
				if (node->threads[i].CPUBursts[j].IOTime) {
					*totalIO += node->threads[i].CPUBursts[j].IOTime;
				}
			}
		}
			
		node = node->next;
	}	
	

}

/*
This function checks if the struct is empty or not. If it is empty then it return 1 or else 0.
*/
int isListEmpty() {
	if (head == NULL) {
		return 1;
	} else {
		return 0;
	}
	
}

/*
This function calculates and returns the lengeth of list.
*/
int lengthOfList() {
	int len = 0;
	
   process *node = head;
   while(node != NULL) {
	    len++;
		node = node->next;
   }   
   
   return len;
}
/*
This function deletes and returns the first node of the struct.
*/
process* deleteAndReturnFirstNode() {
	
	
   process *node = head;
   head = head->next;
	
   return node;
}

/*
 This function simulates algorithm and calculates total time and total turn around time.
 
 */
void eventSimulation(int* totalTime, int verboseMode, int detailedInfoMode, float* totalTurnaroundTime) {
	int totalArrivalTime = 0;
	int totalServiceTime = 0;
	int totalIOTime = 0;
	
	int count = 0;
	float turnaroundTime = 0;
	process *node = head;
	srand(time(NULL));
	int r = (rand() % 9) + 1; /* Random number to simulate time for change of states */
	
	while(node != NULL) {
		
		if (node) {
			for (int i =0; i < node->numberOfThreads; i++) {
				totalServiceTime = 0;
				totalIOTime = 0;
				turnaroundTime = 0;
				
				totalArrivalTime = *totalTime + r;
				
				for(int j = 0; j < node->threads[i].numberOfCPU; j++) {
					totalServiceTime += node->threads[i].CPUBursts[j].CPUTime; /* calculate CPU burst */
					*totalTime += node->threads[i].CPUBursts[j].CPUTime;
					
					
					totalIOTime += node->threads[i].CPUBursts[j].IOTime;  /* calculate IO burst */
					*totalTime += node->threads[i].CPUBursts[j].IOTime;
					
					
					
				}
				turnaroundTime = (*totalTime - node->threads[i].arrivalTime) / node->threads[i].numberOfCPU;
				
				displayOutputDetails(verboseMode, detailedInfoMode, totalArrivalTime, r, i+1, count +1, *totalTime, totalServiceTime, totalIOTime, turnaroundTime);
				
				*totalTurnaroundTime += *totalTime - node->threads[i].arrivalTime; /* calculte total turnaround time */
				
			}
			
			
			
		}
		
		
		node = node->next;
		count++;
		

	}
}

/*
This function displays the output details like total IO time, total service time, total time etc 
	based on the mode selected i.e. verbose and detailed info.
*/
void displayOutputDetails(int verboseMode, int detailedInfoMode, int totalArrivalTime, int ran, int threadNum, int processNum, int totalTime, int totalServiceTime, int totalIOTime, int turnaroundTime) {
	if (verboseMode) {
		
		/* At time X: Thread {id} of Process {id} moves from {state} to {state} */
		printf("------------------------------------------------------------------------------ \n");
		printf("At time %d : Thread %d of Process %d moves from NEW to READY \n", totalArrivalTime, threadNum, processNum);
		printf("At time %d : Thread %d of Process %d moves from READY to RUNNING \n", totalArrivalTime + ran, threadNum, processNum);
		printf("At time %d : Thread %d of Process %d moves from RUNNING to TERMINATED \n", totalTime, threadNum, processNum);
		printf("------------------------------------------------------------------------------ \n");
		
	}
	
	if (detailedInfoMode) {
		/*
		    Thread 1 of Process 2:
			arrival time: 10 
			service time: 22 units 
			I/O time: 3 units 
			turnaround time: 45 units 
			finish time: 55 units
		*/
		printf("------------------------------------------------------------------------------ \n");
		printf("Thread %d of Process %d: \n\n", threadNum, processNum);
		printf("Arrival time: %d \n", totalArrivalTime);
		printf("Service time: %d units \n", totalServiceTime); 
		printf("I/O time: %d units \n", totalIOTime); 
		printf("Turnaround time: %d units \n", turnaroundTime); 
		printf("Finish time: %d units \n", totalTime); 	
		printf("------------------------------------------------------------------------------ \n");
		
	}
	
}

/*
This function displays the default output details like total time, average turn around time 
	and CPU utilization.
*/
void displayDefaultOutputDetails(int verboseMode, int detailedInfoMode, int roundRobinMode, int quantum, int totalTime, float averageTurnaroundTime, float CPUUtilization) {
	
	if(!verboseMode) {
		/*
			Total Time required is 344 time units 
			Average Turnaround Time is 27.9 time units 
			CPU Utilization is 94%
		
		*/
		if (!roundRobinMode) {
			printf("\n FCFS Mode \n\n");
				
			printf("Total Time required is %d time units\n", totalTime);
			printf("Average Turnaround Time is %0.1f time units\n", averageTurnaroundTime);
			printf("CPU Utilization is %.1f%% \n", CPUUtilization);	
		} else {
			printf("\n RR Mode \n\n");
				
			printf("Total Time required is %d time units\n", totalTime);
			printf("Average Turnaround Time is %0.1f time units\n", averageTurnaroundTime);
			printf("CPU Utilization is %.1f%% \n", CPUUtilization);	
		}
							
	}
	
}

/*
This function is used to free struct memory.
*/
void freeMemory() {
	process *temp = head;
	
	while(head != NULL) {
		temp = head;		
		head = head->next;
		free(temp);
	}
}


