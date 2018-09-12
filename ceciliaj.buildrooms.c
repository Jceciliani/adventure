/*Author: Josh Ceciliani
 *Date: 7/18/2018
 *Description: This program takes input from 10 rooms, and selects 7 of them. It then makes 1 room the start room, 1 the end room and the other 5 mid rooms. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h> 
#include <unistd.h>
typedef enum { false, true } bool;


struct room
{
	int id;
	char *name;
	int numOutboundConnections;
	struct room* outboundConnections[6];
	char *roomType;
};
//Prototypes
bool isGraphFull(struct room * roomsArray);
void addRandomConnection(struct room * roomsArray);
bool canAddConnectionFrom(struct room *x);
bool connectionAlreadyExists(struct room *x, struct room *y);
void connectRoom(struct room *x, struct room *y);
bool isSameRoom(struct room *x, struct room *y);
void arrayShuffle(void *a[], int N);

// Returns true if all rooms have 3-6 outbound connections, false otherwise
bool isGraphFull(struct room * roomsArray)
{

	if(roomsArray[0].numOutboundConnections > 2 && roomsArray[1].numOutboundConnections > 2 && roomsArray[2].numOutboundConnections > 2 && roomsArray[3].numOutboundConnections > 2 && roomsArray[4].numOutboundConnections > 2 && roomsArray[5].numOutboundConnections > 2 && roomsArray[6].numOutboundConnections > 2)
	{
		return true;
	}
	else
	{
		return false;
	}

}	

// Adds a random, valid outbound connection from a Room to another Room
void addRandomConnection(struct room * roomsArray)
{
	// Potentially structs 
	struct room* A;
	struct room* B;

	int shuffleA;
	int shuffleB;

	int canAddFlag;
	int sameRoomFlag;
	int connectionFlag;

	while(1)
	{
		shuffleA = (rand() % (6 + 1 - 0) + 0);
		A = &roomsArray[shuffleA];

		if(canAddConnectionFrom(A) == true)
		{
			break;
		}	
	}
	do
	{
		shuffleB = (rand() % (6 + 1 - 0) + 0);
		B = &roomsArray[shuffleB];

	}
	while(canAddConnectionFrom(B) == false || isSameRoom(A, B) == true || connectionAlreadyExists(A, B) == true);

	connectRoom(A, B);

}

// Returns true if a connection can be added from room x (< 6 outbound connections), false otherwise
bool canAddConnectionFrom(struct room *x)
{
	// If the number of outbound connetions is less than 6, return true
	if(x->numOutboundConnections <= 5)
	{
		return true;
	}
	// Else, return false (greater than or equal to 6 connections
	else
	{
		return false;
	}
	
}

// Returns true if a connection from room x to room y already exists, false otherwise
bool connectionAlreadyExists(struct room *x, struct room *y)
{

	int i;
	int flag = 0;
	//Loop through all outbound connections of room x
	for(i = 0; i < x->numOutboundConnections; i++)
	{
		// if a connection matches, return true then 
		if(strcmp(x->outboundConnections[i]->name, y->name) == 0)
		{
			flag = 1;
			break;
		}
	}

	if(flag == 1)
	{
		return true;
	}	
	else
	{
		return false;
	}
}


// Connects rooms x and y together, does not check if this connection is valid
void connectRoom(struct room *x, struct room *y)
{

	//Go to out bound connections, use numConnections to find which location to add, have it be equal to to name of other location
	x->outboundConnections[x->numOutboundConnections] = y;
	y->outboundConnections[y->numOutboundConnections] = x;
	
	// Increase count on number of connections
	x->numOutboundConnections++;
	y->numOutboundConnections++;
	
	return;


}

// Returns true if rooms x and y are the same room, false otherwise
bool isSameRoom(struct room *x, struct room *y)
{

	// If the names of rooms are different - return false
	if(strcmp(x->name, y->name) != 0)
	{
		return false;
	}
	// Else, return true
	else
	{
		return true;
	}	
}
// Shuffles an array, used for randomly assigning room types
void arrayShuffle(void *a[], int N)
{

	int i, j;
	void* temp;

	for(i = 0; i < N; i++)
	{
		j = (rand() % (i+1));
		temp = a[j];
		a[j] = a[i];
		a[i] = temp;
	}
}

int main()
{
	//Randomize everything
	srand(time(NULL));


	// Hardcode 10 different names into string literal
	char* roomNames[10] = {"Alleys", "Leakys", "Ollies", "Diagon", "Supply", "Weasly", "Magicl", "Hogwar", "Hogsmd", "Brooms" };

	// Hard-coded 7 room types to provide randomness algorithm 
	char* roomTypes[7] = {"START_ROOM", "MID_ROOM", "MID_ROOM", "MID_ROOM", "MID_ROOM", "MID_ROOM",  "END_ROOM" };

	struct room roomsArray[7];
	
	// Create array of room structs
	int i;
	for(i = 0; i < 7; i++)
	{
		roomsArray[i].id = i;
		roomsArray[i].name = calloc(16, sizeof(char));
		roomsArray[i].numOutboundConnections = 0;
		roomsArray[i].roomType = calloc(16, sizeof(char));		

	}

	int j;
	int chosenOne;
	for(j = 0; j < 7; j++)
	{
		chosenOne = (rand() % (9 + 1 - 0) + 0);
		// Go through each individual room
		while(roomNames[chosenOne] == "NULL")
		{
			chosenOne = (rand() % (9 - 0 + 1) + 0);
		}
		strcpy(roomsArray[j].name, roomNames[chosenOne]);
		roomNames[chosenOne] = "NULL";

	
	}
	// Add all of the roomTypes, 1 starting room, 1 ending and the rest middle 
	int l;
	arrayShuffle((void **) roomTypes, 7);	
	
	for(l = 0; l < 7; l++)
	{
		strcpy(roomsArray[l].roomType, roomTypes[l]);
	}
	// While grap is not full
	while(isGraphFull(roomsArray) == false)
	{
		addRandomConnection(roomsArray);
	}

	// For printing to files properly - assists in calling them for game 
	char* printRooms[7] = { "ROOM1", "ROOM2", "ROOM3", "ROOM4", "ROOM5", "ROOM6", "ROOM7" };

	//Create Directory for files 
	char str[60];
	sprintf(str, "ceciliaj.rooms.%d", getpid());
	mkdir(str, 0700);
	//Change to that directory
	chdir(str);
	// Write all of the names, and graph connections to file 
	int printTemp = 0;

	while(printTemp < 7)
	{
		//Create file
		FILE * roomFile;
		//open file for printing
		roomFile = fopen(printRooms[printTemp], "w+");
		//Write current room name to file
		fprintf(roomFile, "ROOM NAME: %s\n", roomsArray[printTemp].name);		
		int i;
		int j = 1;
		//Write connections to file	
		for(i = 0; i < roomsArray[printTemp].numOutboundConnections; i++)
		{
			fprintf(roomFile, "CONNECTION %d: %s\n", j, roomsArray[printTemp].outboundConnections[i]->name);
			j++;
		}
		//Write room type to file
		fprintf(roomFile, "ROOM TYPE: %s\n", roomsArray[printTemp].roomType);
		//Close current file
		fclose(roomFile);
		//Increment printTemp value
		printTemp++;
	}	
	
	//Free 	
	int f;
	for(f = 0; f < 7; f++)
	{
		free(roomsArray[f].name);
		free(roomsArray[f].roomType);
	}


	return 0;
}

