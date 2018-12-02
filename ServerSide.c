#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <io.h>
#include <conio.h>
#include <process.h>
#include <dirent.h>
#include <winsock2.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ws2tcpip.h>
#include <time.h>

#define PORT htons(12345)
#define MAXQUEUE 10

#pragma region Database logic
void SetDatabase (){
    printf("\n*** Requested setting a database ***\n");
    char directory [200] = ".\\Database";
    int code = mkdir(directory);
    if(code)
    {
        printf("Database is already set.\n");
    }
    else
    {
        printf("No database directory found! Creating database.\n");
    }
    printf("Action finished\n");
    return;
}
int CreateFolder (char name []){
    printf("\n*** Requested creating a new folder ***\nFolder name: %s\n", name);
    char directory [200] = ".\\Database\\";
    strcat(directory, name);
    printf("Creating a directory under %s\n", directory);
    int code = mkdir(directory);
    if(code)
    {
        printf("Error: Couldn't create a directory. Error code: %d. Aborting...\n", code);
        return -1;
    }
    else
    {
        printf("Directory successfully created.\n");
        printf("Action finished\n");
        return 0;
    }
}
char *ReturnListOfItems(){
    printf("\n*** Action: Getting list of available items in database ***\n");
    struct dirent *de; // Pointer for directory entry 
    DIR *dr = opendir(".\\Database"); // opendir() returns a pointer of DIR type.

    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Error: Could not access database. Aborting...\n"); 
        return "Error"; 
    } 
    printf("Accessed database\n");

    //allocate memory for result
    char *subdirectories = malloc(2048);
    if(!subdirectories){
        printf("Error: Cannot allocate memory for the result. Aborting...\n");
        return "Error";
    }

    //read directory content
    printf("Reading results\n");
    while ((de = readdir(dr)) != NULL){
        strcat(subdirectories,de->d_name); 
        strcat(subdirectories,"\n");
    }
  
    closedir(dr);

    printf("Results:\n%s",subdirectories);
    printf("Action finished\n");
    return subdirectories;
}
int SearchForItem (char name []){
    printf("\n*** Action: Searching database for item \"%s\" ***\n", name);
    char *items = ReturnListOfItems();
    if(items=="Error"){
        printf("Error: Cannot search database. Aborting...\n");
        return -1;
    }
    if(strstr(items, name) != NULL) {
        printf("Item \"%s\" found\n", name);
        printf("Action finished\n");
        return 0;
        }
    else{
        printf("Item \"%s\" not found\n", name);
        printf("Action finished\n");
        return -1;
    }
}
#pragma endregion

#pragma region Server logic
void ServeClientOut(int clientDescriptior, int clientID){
    printf("\n[%d] *** Requested serving an output to client with ID %d ***\nInitialising...\n", clientID, clientID);
    char pathToFile[512];
    long fileLength, dataSend, dataSendProgress, dataRead;
    struct stat fileInfo;
    FILE* fileToSend;
    unsigned char fileBuffer[1024];
    
    printf("[%d] * Reading path to file... *\n", clientID);
    memset(pathToFile, 0, 512);
    if (recv(clientDescriptior, pathToFile, 512, 0) <= 0)
    {
        printf("[%d] Error: Cannot read path to file. Aborting...\n[%d] ----Client finished action with errors----", clientID);
        return;
    }
    printf("[%d] Client requests file %s\n", clientID, pathToFile);  

    printf("[%d] * Reading file data... *\n", clientID);
    if (stat(pathToFile, &fileInfo) < 0)
    {
        printf("[%d] Error: Cannot get access to file data. Aborting...\n[%d] ----Client finished action with errors----", clientID);
        return;
    }
    
    fileLength = fileInfo.st_size;

    printf("[%d] * Checking file size... *\n", clientID);
    if (fileLength == 0)
    {
        printf("[%d] Error: File size is 0. Aborting...\n[%d] ----Client finished action with errors----", clientID);
        return;
    } 
    printf("[%d] Requested file size: %d\n", clientID, fileLength);
    
    printf("[%d] * Converting file length to network format... *\n", clientID);
    fileLength = htonl((long) fileLength);
    
    printf("[%d] * Sending file length to client... *\n", clientID);
    if (send(clientDescriptior, &fileLength, sizeof(long), 0) != sizeof(long))
    {
        printf("[%d] Error: An unexpected error ocurred while sending client a length of the file. Aborting...\n[%d] ----Client finished action with errors----", clientID);
        return;
    }
    printf("[%d] File length sent succesfully\n", clientID);
    
    dataSendProgress = 0;

    printf("[%d] * Getting required file... *\n", clientID);
    fileToSend = fopen(pathToFile, "rb");
    if (fileToSend == NULL)
    {
        printf("[%d] Error: An unexpected error ocurred while opening a file. Aborting...\n[%d] ----Client finished action with errors----", clientID);
        return;
    }
    printf("[%d] File opened succesfully\n", clientID);

    printf("[%d] * Sending client a file... *\n", clientID);
    while (dataSendProgress < fileLength)
    {
        dataRead = fread(fileBuffer, 1, 1024, fileToSend);
        dataSend = send(clientDescriptior, fileBuffer, dataRead, 0);
        if (dataRead != dataSend)
            break;
        dataSendProgress += dataSend;
        printf("[%d] > Sent %d / %d bytes <\n", clientID, dataSendProgress, fileLength);
    }
    
    if (dataSendProgress == fileLength)
        printf("[%d] File has been sent succesfully\n", clientID);
    else
        printf("[%d] Some errors occured while sending a file to a client\n", clientID);

    printf("[%d] * Closing file... *\n", clientID);
    fclose(fileToSend);

    printf("[%d] Action finished\n", clientID);
    return;    
}

int RunServerConnection(){
    printf("\n*** STARTING SERVER ON PORT %d ***\n", PORT);
    
    int clientDescriptor;
    int serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = PORT;
        address.sin_addr.s_addr = INADDR_ANY;
        memset(address.sin_zero, 0, sizeof(address.sin_zero));

    socklen_t addressLength = sizeof(struct sockaddr_in);

    printf("\n*** BINDING PORT %d ***\n", PORT);
    if (bind(serverSocketDescriptor, (struct sockaddr*) &address, addressLength) < 0)
    {
        printf("ERROR: Bind failed. Aborting...\n");
        return 1;
    }
    printf("Bind successful\n");
    
    printf("\n*** LISTENING ON PORT %d. SETTING QUEUE TO %d ***\n", PORT, MAXQUEUE);
    listen(serverSocketDescriptor, MAXQUEUE);
    
    printf("\n*** STARTING INFINITE LOOP ***\n");
    printf("----------------------------------------\n");
    printf("( ͡° ͜ʖ ͡°) | Welcome. Call me Mr Jenkins. I will take care of your clients from now on.\n");
    printf("----------------------------------------\n\n");
    while(1)
    {
        addressLength = sizeof(struct sockaddr_in);

        printf("( ͡° ͜ʖ ͡°) Mr Jenkins | Accepting new client\n");
        clientDescriptor = accept(serverSocketDescriptor, (struct sockaddr*) &address, &addressLength);
        if (clientDescriptor < 0)
        {
            printf("( ͡° ʖ̯ ͡°) Mr Jenkins | But he didn't accept us...\n");
            continue;
        }

        printf("( ͡° ͜ʖ ͡°) Mr Jenkins | Incoming connect from %s:%u\n", 
            inet_ntoa(address.sin_addr),
            ntohs(address.sin_port)
            );

        printf("( ͡° ͜ʖ ͡°) Mr Jenkins | Assigning Mr Harold to take care of new client\n");
        if (fork() == 0) // child process
        {
            printf("(͠≖ ͜ʖ͠≖) Mr Harold | Serving client %s:%u\n", 
                inet_ntoa(address.sin_addr),
                ntohs(address.sin_port)
                );
            ServeClientOut(clientDescriptor, ntohs(address.sin_port));

            printf("(͠≖ ͜ʖ͠≖) Mr Harold | Closing port...\n");
            close(clientDescriptor);

            printf("(͠≖ ͜ʖ͠≖) Mr Harold | Killing this process... I'll be back when you need me\n");
            exit(0);
        }
        else
        {
            printf("( ͡° ͜ʖ ͡°) Mr Jenkins | Back to work. Still listening...\n"); // main process
            continue;
        }
    }
    return 0;
}
#pragma endregion

#pragma region User Interaction logic
void RetrieveItem (){
}
void ReturnItem (char name []){

}
#pragma endregion

int main(int argc, char const *argv[])
{
    printf("Starting server side C application\n");
    RunServerConnection();

    return 0;
}
