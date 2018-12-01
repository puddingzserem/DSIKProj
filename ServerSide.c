#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <io.h>
#include <conio.h>
#include <process.h>
#include <dirent.h>

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
void RetrieveItem (){
}
void AddItem (){
    return;
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
void ReturnItem (char name []){

}
void SetServerConnection(){

}
int main(int argc, char const *argv[])
{
    printf("Starting server side C application\n");
    SetDatabase();
    CreateFolder("elo");
    CreateFolder("ziomus");
    CreateFolder("cotam");
    CreateFolder("test");
    SearchForItem("test");
    SearchForItem("heh");

    // FILE *fp;
    // char ch;
    // fp = fopen("database/one.txt", "w");
    // printf("Enter data...");
    // while( (ch = getchar()) != EOF) {
    //     putc(ch, fp);
    // }
    // fclose(fp);
    // fp = fopen("one.txt", "r");
 
    // while( (ch = getc(fp)! = EOF)
    // printf("%c",ch);
    
    // closing the file pointer
    //fclose(fp);

    return 0;
}
