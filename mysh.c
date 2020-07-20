/* Ergastiriaki Askisi 1
 * 
 * Kallitsopoulou Styliani	A.M.: 2113121	
 * Dollas Nikolaos		A.M.: 2113007
 *	
 * Compile: gcc -o mysh mysh.c
 * Run: ./mysh
 * Ignore warnings from gets()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_PATH_LEN 128
#define MAX_COMMAND_LEN 10
	
	int ls_ownedby(const char*, const char*);	// prototype

int main(int argc, char** argv){
	char currentDirectory[MAX_PATH_LEN];	// string to save current directory
	char bin[10] = "/bin/";		// string to make first argument for execl	
	char command[MAX_COMMAND_LEN];	// string to save user command
	pid_t pid;
	int status = 0;
	FILE* fp;
	//char kill[15] = "kill ";
	int counter;
	char s[255][1024], s1[255], s2[255], s3[255], s4[255];	// strings for exit command
	char ls[1][10], s5[10], s6[10], s7[2], s8[2];	// strings for ls_ownedby username [-a] [-l]
	char h[255][1024], h1[255], h2[255], h3[255], h4[255], h5[255], h6[255], h7[255], h8[255], h9[255];	// strings for ls -a -l command
	
// 1	
	if(getcwd(currentDirectory, MAX_PATH_LEN) != NULL)
		printf("\nCurrent directory: %s\n", currentDirectory);
	else
		printf("\nERROR!\n");

// 2	
	while(1){
		strcpy(s5, "");strcpy(s6, "");strcpy(s7, "");strcpy(s8, "");	// initialize	
		printf("\nGive the command to execute: ");
		gets(command);	// scanf("%s", command); cant read string with spaces	

		if(!ls_ownedbyCMP(command, "ls_ownedby")){	// checking if user command is ls_ownedby
			strcpy(ls[0], command);		
			sscanf(ls[0], "%s %s %s %s", s5 , s6 , s7 , s8);	// we distinguish the main string(ls) in 4 strings(s5,s6,s7,s8)
			
			if(!strcmp(s6, "") ||	// ls_ownedby without username
			(s6[0] >= 33 && s6[0] <= 46) || // some symbols from ASCII table
			(strcmp(s7, "") && strcmp(s7, "-a") && strcmp(s7, "-l")) ||	// only [-a] and [-l] as parameters
			(strcmp(s8, "") && strcmp(s8, "-a") && strcmp(s8, "-l"))){	// only [-a] and [-l] as parameters
				printf("\nls_ownedby username [-a] [-l]\n");
				continue;
			}
		}		
		else if(strlen(command) > MAX_COMMAND_LEN){	// max command length for execl = 10
			printf("\nMAX_COMMAND_LEN: %d\n", MAX_COMMAND_LEN);
			continue;
		}	
		strcat(bin, command);	// making /bin/[command] for execl first argument

		pid = fork();
		if(pid >= 0){	// fork was successful
			if(pid == 0){	// child				
				if(!strcmp(command, "exit")){	// checking if user command is exit
					system("ps > output.txt");	// making file to save ps output
					fp = fopen("output.txt", "r");
					
					fgets(s[0], 1024, fp);

					counter = 1;
					while(fgets(s[counter], 1024, fp)){	
						sscanf(s[counter], "%s %s %s %s", s1, s2, s3, s4);	// we distinguish the main string(s) in 4 strings(s1,s2,s3,s4)
						if(!strcmp(s4, "mysh")){
							//strcat(kill, s1);
							//system(kill);
							system("rm output.txt");	// delete ps output file
							execl("/bin/kill", "kill", s1, NULL);	// kill PID
							return 0;
						}
						counter++;
					}
					fclose(fp);
					break;
				}
				else if(!strcmp(s5, "ls_ownedby")){	// checking if user command is ls_ownedby 				
					system("ls -a -l > output1.txt");	// making file to save ls -a -l output
					fp = fopen("output1.txt", "r");
					
					fgets(s[0], 1024, fp);
					printf("\n\n");
					counter = 1;
					while(fgets(s[counter], 1024, fp)){	
						sscanf(s[counter], "%s %s %s %s %s %s %s %s %[^\n]s", h1, h2, h3, h4, h5, h6, h7, h8, h9);	// we distinguish the main string(s) in 9 strings(h1,h2,...,h9)
						if(!strcmp(h3, s6)){	// if username for ls -a -l(h3) = username from user(s6)
							if(!strcmp(s7, "") && !strcmp(s8, "")){	// ls_ownedby username
								if(strcmp(h9, ".") && strcmp(h9, "..")){	// blocking output from -a parameter
									printf("Owner: %s\t%s:%s%s\n", h3, (h1[0] == 'd') ? "Directory" : "File",(h1[0] == 'd') ? "\t" : "\t\t", h9);	
								}						
							}
							else if(strcmp(s7, "") && strcmp(s8, "")){	// ls_ownedby username [-a] [-l]
								printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", h1, h2, h3, h4, h5, h6, h7, h8, h9);
							}	
							else if(!strcmp(s8, "") && !strcmp(s7, "-a")){	// ls_ownedby username [-a]
								printf("Owner: %s\t%s:%s%s\n", h3, (h1[0] == 'd') ? "Directory" : "File",(h1[0] == 'd') ? "\t" : "\t\t", h9);								
							}
							else if(!strcmp(s8, "") && !strcmp(s7, "-l")){	// ls_ownedby username [-l]
								if(strcmp(h9, ".") && strcmp(h9, "..")){	// blocking output from -a parameter
									printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", h1, h2, h3, h4, h5, h6, h7, h8, h9);
								}						
							}				
						}
						counter++;
					}
					printf("\n");
					system("rm output1.txt");	// delete ls -a -l output file
					fclose(fp);
				}					
				else if(execl(bin, command, NULL) == -1){	// run users command without parameters
					printf("\nUnknown command type for execl ** \"%s\" **\n", command);
					strcpy(bin, "/bin/");
				}
			exit(0);
			}
			else{	// parent			
				if(waitpid(pid, &status, 0) != pid){	// parent wating for childs ends
					status = -1;
				exit(0);			
				}
				strcpy(bin, "/bin/");
			}		
		}
		else{	// fork failed
			printf("\nFORK FAILED!\n");
			status = -1;
		exit(0);
		}
	}	
return status;
}

// function to check if user wrote ls_ownedby command
int ls_ownedbyCMP(const char* s1, const char* s2){	
int i;	
int len = strlen(s2);	
	
	for (i = 0; i < len; s1++, s2++, i++){
		if (*s1 != *s2)
		    return -1;
	}
return 0;
}
