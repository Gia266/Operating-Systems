#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>

//+
// File:	shell.c
//
// Purpose:	This program implements a simple shell program. It does not start
//		processes at this point in time. However, it will change directory
//		and list the contents of the current directory.
//
//		The commands are:
//		   cd name -> change to directory name, print an error if the directory doesn't exist.
//		              If there is no parameter, then change to the home directory.
//		   ls -> list the entries in the current directory.
//			      If no arguments, then ignores entries starting with .
//			      If -a then all entries
//		   pwd -> print the current directory.
//		   exit -> exit the shell (default exit value 0)
//				any argument must be numeric and is the exit value
//
//		if the command is not recognized an error is printed.
//-

#define CMD_BUFFSIZE 1024
#define MAXARGS 10

int splitCommandLine(char * commandBuffer, char* args[], int maxargs);
int doInternalCommand(char * args[], int nargs);
int doProgram(char * args[], int nargs);

//+
// Function:	main
//
// Purpose:	The main function. Contains the read
//		eval print loop for the shell.
//
// Parameters:	(none)
//
// Returns:	integer (exit status of shell)
//-

int main() {

    char commandBuffer[CMD_BUFFSIZE];
    // note the plus one, allows for an extra null
    char *args[MAXARGS+1];

    // print prompt.. flush is needed because
    // stdout is line buffered, and won't
    // write to terminal until newline
    printf("%%> ");
    fflush(stdout);

    while(fgets(commandBuffer,CMD_BUFFSIZE,stdin) != NULL){
        //printf("%s",commandBuffer);

	// remove newline at end of buffer
	int cmdLen = strlen(commandBuffer);
	if (commandBuffer[cmdLen-1] == '\n'){
	    commandBuffer[cmdLen-1] = '\0';
	    cmdLen--;
            //printf("<%s>\n",commandBuffer);
	}

	// split command line into words.(Step 2)
	// add a null to end of array (Step 2)
    int nargs = splitCommandLine(commandBuffer, args, MAXARGS);
    args[nargs] = NULL;


	// debugging
    /*
    printf("%d\n", nargs);
	int i;
	for (i = 0; i < nargs; i++){
	  printf("%d: %s\n",i,args[i]);
	}
	//element just past nargs
	printf("final: %d: %s\n",i, args[i]);
    */
    
    int retValue;
    if (nargs > 0 ){ //if there are arguments (otherwise do nothing)
    retValue = doInternalCommand(args, nargs); //try internal command

    if (retValue == 0){ //if no internal command recognized
        retValue = doProgram(args, nargs); //try to execute external program

        if (retValue == 0) //if no internal or external program found, error message
        printf("No internal or external program found! \n");
    }


    }
    

	// print prompt
	printf("%%> ");
	fflush(stdout);
    }
    return 0;
}

////////////////////////////// String Handling (Step 1) ///////////////////////////////////

//+
// Function:	skipChar
//
// Purpose:	This function skips over a given char in a string
//		For security, will not skip null chars.
//
// Parameters:
//    charPtr	Pointer to string
//    skip	character to skip
//
// Returns:	Pointer to first character after skipped chars
//		ID function if the string doesn't start with skip,
//		or skip is the null character
//-

const char *skipChar(const char *charPtr, char skip) {
        
    while (*charPtr != '\0' && *charPtr == skip) { //when not at the end, and a skippable char
        charPtr++; //check next char
    }
    
    return charPtr; //returns end of string or non-skip char
}


//+
// Funtion:	splitCommandLine
//
// Purpose: Takes input string from shell, and splits it up into readable commands/arguments
//
// Parameters:
//	commandBuffer   holds incoming string typed in chat
//  args            where split up arguments will be stored
//  maxargs         maximum number of arguments allowed
//
// Returns:	Number of arguments (< maxargs).
//
//-

int splitCommandLine(char *commandBuffer, char *args[], int maxargs) {
    int count = 0;
    commandBuffer = skipChar(commandBuffer, ' '); // Find starting pointer to next word
    if (*commandBuffer == '\0') //catches zero arguments edge case
        return 0;

    for (int i = 0; i < maxargs; i++) {
        //loop starts at index of an arg

        if (*commandBuffer == '\0') {// if end char at end of cmdbuffer
            count = i;
            break; 
        }

        args[i] = commandBuffer; // Log word location

        //char *spacePtr = strchr(commandBuffer, ' '); //caused a weird issue
        char *spacePtr = commandBuffer;
        while(*spacePtr != '\0' && *spacePtr != ' '){
                spacePtr ++;
        }// Find next empty space (end of that word) 

        if (spacePtr == '\0'){ //end of cmd without a space
            count = i;
            break; // End of the input string
        }
        //there is a space after the word

        *spacePtr = '\0'; // Replace the space character with null-terminated
        commandBuffer = spacePtr + 1; // Move to the character after the space
        commandBuffer = skipChar(commandBuffer, ' '); // Find starting pointer to next word
        //loop to find next arg
    }
    return count; //maybe +1??
}

////////////////////////////// External Program  (Note this is step 4, complete doeInternalCommand first!!) ///////////////////////////////////

// list of directorys to check for command.
// terminated by null value
char * path[] = {
    ".",
    "/usr/bin",
    NULL
};

//+
// Funtion:	doProgram
//
// Purpose:	checks if given command is valid and matches up to an external command
//                      if command is valid, it is executed
//
// Parameters:
//	        args    array of arguments entered by user
//          nargs   number of arguments in the args array
//
// Returns	int
//      2 = failed, don't print error message
//		1 = found and executed the file
//		0 = could not find and execute the file (return and print error message)
//-

int doProgram(char *args[], int nargs) {
    //first part searches path array
    char *path[] = { ".", "/usr/bin", NULL };
    char *commandPath = NULL;
    struct stat st;
    for (int i = 0; path[i] != NULL; i++) {
        //gets length of path components
        int path_len = strlen(path[i]);
        int cmd_len = strlen(args[0]);
        //allocate memory for path, plus a slash and end char
        commandPath = malloc(path_len + cmd_len + 2);
        sprintf(commandPath, "%s/%s", path[i], args[0]); //concat the path string
        /* Check if the file exists and is executable */

        //check if input exits and can be executed
        if (stat(commandPath, &st) == 0 && S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR)) {
            //if the path works, move onto next
            break;
        }
        //no valid path found, exit
        free(commandPath); //free up memory
       commandPath = NULL;
    }
    if (commandPath == NULL) {
        //no path found, print error and exit
        return 0;
    }
    //fork cmd to make a child for the executable
    pid_t pid = fork(); //process id
    if (pid == -1) {
        //fail

        fprintf(stderr, "Fork Failed\n");
        free(commandPath);
        return 2;
    } else if (pid == 0) {
        //child process
        execv(commandPath, args);
        //if code continues here, execute func failed

        fprintf(stderr, "Failed to execute command: %s\n", args[0]);
        //free memory and exit
        free(commandPath);
        exit(1);
    } else {
        //parent process
        free(commandPath);
        int stat; 
        wait(&stat);//wait for child
        if (WIFEXITED(stat)) { //if normal, return status
            return 1; //all went well
        } else { //invalid, something weird
            fprintf(stderr, "Child process encountered an issue\n");
            return 2;
        }
    }

}




////////////////////////////// Internal Command Handling (Step 3) ///////////////////////////////////

// define command handling function pointer type
typedef void(*commandFunc)(char * args[], int nargs);

// associate a command name with a command handling function
struct cmdStruct{
   char 	* cmdName;
   commandFunc 	cmdFunc;
};

// prototypes for command handling functions
// TODO: add prototype for each comamand function
commandFunc exitFunc(char *args[], int nargs);
commandFunc pwdFunc(char *args[], int nargs);
commandFunc lsFunc(char *args[], int nargs);
commandFunc cdFunc(char *args[], int nargs);
// list commands and functions
// must be terminated by {NULL, NULL} 
// in a real shell, this would be a hashtable.
struct cmdStruct commands[] = {
   // TODO: add entry for each command
   {"exit", exitFunc},
   {"pwd", pwdFunc},
   {"ls", lsFunc},
   {"cd", cdFunc},
   { NULL, NULL}		// terminator
};

//+
// Function:	doInternalCommand
//
// Purpose:	takes given arguments, checks them against an index of internal commands
//              if argument matches an internal command, calls required function
//
// Parameters:
//	        args    array of arguments entered by user
//          nargs   number of arguments in the args array
//
// Returns	int
//		1 = args[0] is an internal command
//		0 = args[0] is not an internal command
//-

int doInternalCommand(char * args[], int nargs){
    // TODO: function contents (step 3)
    for (int i = 0; i >= 0; i++){ //infinite loop

        if (commands[i].cmdName == NULL) //if end of command list
            break;

        if (strcmp(commands[i].cmdName, args[0]) == 0){ //if command is found in table
            commands[i].cmdFunc(args, nargs); //execute commands
            return 1;
        }

    }
    //else no command found
    //printf("no internal command found \n");
    return 0;
}

///////////////////////////////
// Command Handling Functions //
///////////////////////////////

//+
// Function:	exitFunc
// Purpose: exits the shell
//
// Parameters:
//	        args    array of arguments entered by user
//          nargs   number of arguments in the args array
//
// Returns:
//      nothing
//-

commandFunc exitFunc(char *args[], int nargs){
    if (nargs > 1){
    printf("Too many arguments! \n");
    return 0;
    }
    exit(0);
}
//+
// Function:	pwdFunc
// Purpose: prints working directory
//
// Parameters:
//	        args    array of arguments entered by user
//          nargs   number of arguments in the args array
//
// Returns:
//       nothing
//-
commandFunc pwdFunc(char *args[], int nargs){
    if (nargs > 1){
    printf("Too many arguments! \n");
    return 0;
    }
    char * cwd = getcwd(NULL,0); //allocates and gets value
    printf("%s \n", cwd);
    free(cwd); //free up allocated memory
}
//+
// Function:	lsFilterHelper
// Purpose:  a filter for ls command, exclused hidden '.' files
//
// Parameters:
//	       d    a file name
//
// Returns:
//          0 if file is filtered out
//          1 if file is not filtered
//-
int lsFilterHelper(const struct dirent *d){
    if (d->d_name[0] == '.'){
        return 0;
    }
    else return 1;
}
//+
// Function:	lsFunc
// Purpose:     lists files in current directory (unfiltered with -a)
//
// Parameters:
//	        args    array of arguments entered by user
//          nargs   number of arguments in the args array
//
// Returns:
//       0 if invalid argument
//-
commandFunc lsFunc(char *args[], int nargs){
   struct dirent ** namelist;
   int numEnts = 0;
    if (nargs > 2){
        printf("Too many arguments! \n");
         return 0;
    }
    if (nargs == 1) //if noargs
        numEnts = scandir(".",&namelist, lsFilterHelper ,NULL);
    else if (strcmp(args[1], "-a") == 0) //if correct extra argument
        numEnts = scandir(".",&namelist, NULL ,NULL);
    else{
        printf("Invalid argument\n");
        return 0;
    }
    //valid arguments and list, now print
    for (int i = 0; i < numEnts; i++)
    printf("%s ", namelist[i]->d_name);

    printf("\n");
}

//+
// Function:	cdFunc
// Purpose:      changes working directory
//
// Parameters:
//	        args    array of arguments entered by user
//          nargs   number of arguments in the args array
//
// Returns:
//       0 if error
//-
commandFunc cdFunc(char *args[], int nargs){
struct passwd *pw = getpwuid(getuid());
if (nargs > 2){
    printf("Too many arguments! \n");
    return 0;
}
if (pw == NULL){
    printf("Password Error! \n");
    return 0;
}
//valid password
if (nargs == 1){ //default no args
       if (chdir(pw->pw_dir) != 0){//change to home (if it fails)
    printf("Error changing to home directory \n");
    return 0;
    }
}
else {
    if (chdir(args[1]) != 0){//change to spec with check
    printf("Error changing to specified directory \n");
    return 0;
    }
}


}

