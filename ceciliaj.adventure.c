
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <pthread.h>			
#include <unistd.h>
#include <dirent.h>
typedef enum { false, true } bool;

// Create the mitext thread
pthread_mutex_t mutex;
//Global Arrays*********************************************
// Randomized room names array
char randomRooms[7][10];
// Room types array
char randomTypes[7][10];
// Number of connections array
int totalConnections[7];
// All arrays for room connections
char room1Connections[6][10];
char room2Connections[6][10];
char room3Connections[6][10];
char room4Connections[6][10];
char room5Connections[6][10];
char room6Connections[6][10];
char room7Connections[6][10];	

// Gets the current directory - updated with time on each iteration
char* getDirectory()
{
	int newDirTime = -1;
	char targetDirPrefix[32] = "ceciliaj.rooms.";
	char* newDirName = malloc(sizeof(char) * 48);	
	memset(newDirName, '\0', sizeof(newDirName));

	DIR* dirToCheck;
	struct dirent *fileInDir;
	struct stat dirAtt;

	dirToCheck = opendir(".");

	if(dirToCheck > 0)
	{
		while((fileInDir = readdir(dirToCheck)) != NULL)
		{
			if(strstr(fileInDir->d_name, targetDirPrefix) != NULL)
			{
				stat(fileInDir->d_name, &dirAtt);
				if((int)dirAtt.st_mtime > newDirTime)
				{
					newDirTime = (int)dirAtt.st_mtime;
					memset(newDirName, '\0', sizeof(newDirName));
					strcpy(newDirName, fileInDir->d_name);
				}

			}
		}
		
	}
	return newDirName;
		
	//closedir(dirToCheck);
}

// Read in the data - current file ************************* 
void readData()
{

	char* getRooms[7] = { "ROOM1", "ROOM2", "ROOM3", "ROOM4", "ROOM5", "ROOM6", "ROOM7" };	

	char directory[50];
	char folderLocation[50];
	sprintf(directory, getDirectory());
	int getData = 0;

	while(getData < 7)
	{
		sprintf(folderLocation, "./%s/%s", directory, getRooms[getData]);
	
		//printf("folderLocation: %s\n", folderLocation); fflush(stdout);
	
		FILE* folderFile;
		folderFile = fopen(folderLocation, "r");
		
		// Initilize and increment iterator of connections
		int numOfConnections = 0;
		// While current file does not hit EOF
		while(!feof(folderFile))
		{
			char buffer[50];
			// Get line
			fgets(buffer, 50, folderFile);

			//printf("Buffer is: %s\n", buffer); fflush(stdout);	
			// If target is the room name
			if(strstr(buffer, "NAME") != NULL)
			{
				char dest[12];
				strncpy(dest, buffer+11, 6);
			//	printf("Here\n"); fflush(stdout);
				strcpy(randomRooms[getData], dest);
			//	printf("Yep\n"); fflush(stdout);					
			}
			// Else if target is a connection
			else if(strstr(buffer, "CONNECTION") != NULL)
			{
				
				char dest1[6];
				strncpy(dest1, buffer+14, 6);
				if(getData == 0)
				{
					strcpy(room1Connections[numOfConnections], dest1);
				}	

				else if(getData == 1)
				{
					strcpy(room2Connections[numOfConnections], dest1);
				}
				else if(getData == 2)
				{
					strcpy(room3Connections[numOfConnections], dest1);
				}
				else if(getData == 3)
				{
					strcpy(room4Connections[numOfConnections], dest1);
				}
				else if(getData == 4)
				{
					strcpy(room5Connections[numOfConnections], dest1);
				}
				else if(getData == 5)
				{
					strcpy(room6Connections[numOfConnections], dest1);
				}
				else if(getData == 6)
				{
					strcpy(room7Connections[numOfConnections], dest1);	
				}	
				else
				{
					printf("Error");
				}	
				// Increase connections iterator
				numOfConnections++;	
			}
			// Else if target is a room type
			else if(strstr(buffer, "TYPE") != NULL)
			{
				char dest2[12];
				strncpy(dest2, buffer+11, 11);
				strcpy(randomTypes[getData], dest2);			
				
			}

			else
			{
				printf("Error");
			}

		}		
		// Place numOfConnections into correct array
		totalConnections[getData] = numOfConnections;			
	
		// Increment getData
		getData++;
		// Close file
		fclose(folderFile);

	}
	// Change newlines into \0
	int k;
	for(k = 0; k < 7; k++)
	{
		size_t length = strlen(randomTypes[k]);
	
		if(randomTypes[k][length-1] == '\n')
		{
			randomTypes[k][length-1] = '\0';
		}
	}	
}

// Find the starting room for the game
int startingRoom()
{
	int i;
	int var;
	for(i = 0; i < 7; i++)
	{

		if(randomTypes[i][0] == 'S')
		{	
			var = i;		
		}
	}
	
	return var;
}
// Returns the current room in the array
int currentRoom(char* input)
{
	int i;
	int herpderp = 7;
	for(i = 0; i < 7; i++)
	{
		// If rooms are the same, return i'th room
		if(strcmp(randomRooms[i], input) == 0)
		{
			herpderp = i;
			break;
		}
	}	
	return herpderp;
}	

// Get the current time and place it into a file
void* userTime()
{
	// Lock the thread
	pthread_mutex_lock(&mutex);
	// Create file
	FILE* timeFile;
	// Open file, name it currentTime.txt
	timeFile = fopen("currentTime.txt", "w+");
	// Create character buffer to write the current time to
	char timeBuffer[50];
	// Create struct that contains a calendar date and time broken into components
	struct tm *currentTime;
	// Create a time variable set to time 0
	time_t current = time(0);
	// Uses the value pointed to by time_t to fill the tm struct
	currentTime = gmtime(&current);
	// Format the time represented by the struct tm and places it in timeBuffer
	strftime(timeBuffer, sizeof(timeBuffer), "%I:%M%p, %A, %B %d, %Y", currentTime); 	
	//Writes the string in timeBuffer into timeFile
	fputs(timeBuffer, timeFile);
	// Close the file
	fclose(timeFile);
	// Unlock the thread
	pthread_mutex_unlock(&mutex);

}

// Create threads for printing the time
void threads()
{
	// Create/define thread reference variable
	pthread_t thread;
	// Initilize the mutex, set attributes to NULL
	pthread_mutex_init(&mutex, NULL);
	// Lock the mutex - returns 0 - if already locked, calling thread blocked until it becoes available
	pthread_mutex_lock(&mutex);
	// Create a new thread - referenced thread, NULL attributes, the start routine function(userTime()) and NULL arguments on the start routine
	int id = pthread_create(&thread, NULL, userTime, NULL);
	// Unlock the current referenced mutex being used
	pthread_mutex_unlock(&mutex);
	// Join threads
	pthread_join(thread, NULL);
	// Destroy the current reference mutex to uninitilize the mutex for future use
	pthread_mutex_destroy(&mutex);
	// Suspends execution of the calling thread for 20 microseconds
	usleep(20);	
}

// Gets the time file and prints it to the screen if "time" is typed
void userWriteTime()
{
	// Create file
	FILE* timeFile;
	// open the currentTime.txt file for reading
	timeFile = fopen("currentTime.txt", "r");
	// Create character buffer to write the time to
	char timeBuffer[50];
	// Read the line from timeFile and place it in time buffer
	fgets(timeBuffer, 50, timeFile);
	// Print contents of timeBuffer to the screen
	printf("\n%s\n", timeBuffer);
	// Close the file
	fclose(timeFile);	

}
// Check the connections of the rooms to see if it is actually a connection
int connections(char* input, int i)
{
	
	int checker = 0;

	if(i == 0)
	{
		int j;
		for(j = 0; j < totalConnections[i]; j++)
		{
			if(strcmp(room1Connections[j], input) == 0)
			{
				checker = 1;
			}
		}
				
	}	
	else if(i == 1)
	{
		int j;
		for(j = 0; j < totalConnections[i]; j++)
		{
			if(strcmp(room2Connections[j], input) == 0)
			{
				checker = 1;
			}
		}
	}
	else if(i == 2)
	{
		int j;
		for(j = 0; j < totalConnections[i]; j++)
		{
			if(strcmp(room3Connections[j], input) == 0)
			{
				checker = 1;
			}
		}
	}
	else if(i == 3)
	{
		int j;
		for(j = 0; j < totalConnections[i]; j++)
		{
			if(strcmp(room4Connections[j], input) == 0)
			{
				checker = 1;
			}
		}
	}	
	else if(i == 4)
	{
		int j;
		for(j = 0; j < totalConnections[i]; j++)
		{
			if(strcmp(room5Connections[j], input) == 0)
			{
				checker = 1;
			}
		}
	}
	else if(i == 5)
	{
		int j;
		for(j = 0; j < totalConnections[i]; j++)
		{
			if(strcmp(room6Connections[j], input) == 0)
			{
				checker = 1;
			}
		}
	}
	else if(i == 6)
	{	
		int j;
		for(j = 0; j < totalConnections[i]; j++)
		{
			if(strcmp(room7Connections[j], input) == 0)
			{
				checker = 1;
			}
		}
	}
	else if(i == 7)
	{
		checker = 0;
	}	
	else
	{
		printf("Something awful happened");
	}

return checker;
	
}	
// Play the game
void playGame()
{
	// Number of steps player takes
	int stepCount = 0;
	// Character buffer for reading player input
	char userInput[50];
	// Char array to store user movement room to room
	char userPath[50][12];
	// Initilized iterator for game play
	int i = startingRoom();	
	// while user has not gotten to the last room, continue	
	while(strcmp(randomTypes[i], "END_ROOM") != 0)
	{
		printf("\nCURRENT LOCATION: %s\n", randomRooms[i]);			
		printf("POSSIBLE CONNECTIONS:");
		int j;
		for(j = 0; j < totalConnections[i]; j++)
		{
			if(i == 0)
			{
				printf(" %s,", room1Connections[j]);		
			}	
			else if(i == 1)
			{
				printf(" %s,", room2Connections[j]);
			}
			else if(i == 2)
			{
				printf(" %s,", room3Connections[j]);
			}
			else if(i == 3)
			{
				printf(" %s,", room4Connections[j]);		
			}	
			else if(i == 4)
			{
				printf(" %s,", room5Connections[j]);
			}
			else if(i == 5)
			{
				printf(" %s,", room6Connections[j]);
			}
			else if(i == 6)
			{	
				printf(" %s,", room7Connections[j]);
			}
			else
			{
				printf("Not printing correctly");
			}
		}
	
		printf(". \nWHERE TO? >");
		scanf("%s", userInput);

		// If user inputs time, print time to screen	
		if(strcmp(userInput, "time") == 0)
		{
			// Create the thread
			threads();
			// Writes current time to stdout
			userWriteTime();
		}
		// Check to see if the input is a valid connection
		else if(connections(userInput, i) == 0)
		{
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN. \n"); 
		}
		// User input is valid
		else	
		{

			// Copies user input into path
			strcpy(userPath[stepCount], userInput);
			// Step count is incremented
			stepCount++;
			// Get current room information
			i = currentRoom(userInput);
		}	
	}

	// User found the END_ROOM and won the game
	printf("\nCONGRATULATIONS, YOU HAVE APPARATED TO THE END_ROOM!\n");
	// Print # of steps and path in order	
	printf("YOU TOOK %i APPARATIONS AND YOUR PATH WAS:\n", stepCount);
	int p;
	for(p = 0; p < stepCount; p++)
	{
		printf("%s\n", userPath[p]);
	}
}


int main()
{
	readData();
	playGame();		

	return 0;
}


/* References
 * http://www.cplusplus.com/reference/ctime/gmtime/
 * https://stackoverflow.com/questions/13658756/example-of-tm-use
 * https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
 * https://www.tutorialspoint.com/c_standard_library/c_function_fputs.htm
 * https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
 * http://man7.org/linux/man-pages/man3/pthread_mutex_init.3p.html
 * http://timmurphy.org/2010/05/04/pthreads-in-c-a-minimal-working-example/comment-page-1/
 * http://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html
 * http://man7.org/linux/man-pages/man3/pthread_create.3.html
 * https://linux.die.net/man/3/usleep
 */
