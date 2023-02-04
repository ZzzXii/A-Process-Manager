/* 
CSC 360
Assignment 1
Zexi Luo
V00929142
*/

#include <string.h>			// strcmp()
#include <stdbool.h>		// 
#include <stdlib.h>			// malloc()
#include <unistd.h>			// fork(), execvp()
#include <stdio.h>			// printf()
#include <sys/types.h>		// pid_t
#include <sys/wait.h>		// waitpid()
#include <signal.h>			// kill(), SIGTERM, SIGKILL, SIGSTOP, SIGCONT
#include <errno.h>			// errno
#include <ctype.h>



/*---------------global variables---------------*/
#define MAX_LINE_LEN 1024

/*---------------Typedef---------------*/
typedef struct node{
    pid_t pid;
    char path[20];
    struct node* next;
}node_t;

node_t *head = NULL;


/*----------------------------Linked list Functions-------------------------------*/

//Create a new node for store into the process 
void add_newNode(pid_t pid, char* path){
  node_t *new = (node_t *)malloc(sizeof(node_t));
  new->pid = pid;
  strcpy(new->path, path);
  new->next = NULL;

  if(head == NULL){
    head = new;
  }else{
    node_t* current = head;
    while(current->next != NULL){
      current = current->next;
    }
    current->next = new;
  }
  // printf("pid %d\n",new->pid);
  // printf("path %s\n",new->path);
}

node_t *remove_Node(pid_t pid){
	node_t *prev;
	node_t *temp = head;

  if(temp != NULL && temp->pid == pid){
    head = temp->next;
    free(temp);
    return head;
  }

  while(temp != NULL){
    if(temp->pid == pid){
      prev->next = temp->next;
      free(temp);
      return head;
    }
    prev = temp;
    temp = temp->next;
  }
  return head;
}  


/*---------------Help Function---------------*/
// Check if the process exist
int check_process_exist(pid_t pid){
  // printf("%s", "Inside the check_process_exist\n");
	node_t* curr= head;
	while(curr != NULL){
		if(curr->pid == pid){
			return 1;
		}
		curr = curr->next;
	}
  	return 0;
}

//read process state
void read_stat(char* path_stat){
  FILE *file = fopen(path_stat, "r");
  char file_buffer[MAX_LINE_LEN];
  char* p;

  if(file == NULL){
    fprintf(stderr, "unable to open %s\n", path_stat);
  }else{
    int i=1;

    while(fgets(file_buffer, sizeof(file_buffer)-1, file) != NULL){
      
      /* get the first token */
      char* token = strtok(file_buffer, " ");

      /* walk through other tokens */
      while(token != NULL){
        token = strtok(NULL, ": ");
        i++;
        /*Reference:
        https://www.programiz.com/c-programming/c-switch-case-statement
        */
        switch(i){
          case 2: // comm
            printf("comm: %s\n", token);
            break;
          case 3: // state
            printf("state: %s\n", token);
            break;
          case 14: ; // utime
            long unsigned int utime = strtoul(token, &p, 10)/ sysconf(_SC_CLK_TCK);
            printf("utime: %lu\n", utime);
            break;
          case 15: ;// stime
            long unsigned int stime = strtoul(token, &p, 10)/ sysconf(_SC_CLK_TCK);
            printf("stime: %lu\n", stime);
            break;
          case 24:// rss
            printf("rss: %s\n", token);
            break;
          default:
            break;
        }
      }
    }
	fclose(file);
  }
  return;
}

void read_status(char* path_status){
  FILE *file = fopen(path_status, "r");
	if (file == NULL){
		fprintf(stderr, "unable to read %s\n", path_status);
	}else{
		char buffer[MAX_LINE_LEN];
		while (fgets(buffer, sizeof(buffer) - 1, file) != NULL){
			char *token = strtok(buffer, " :");
			if (token != NULL){
        char* voluntary = "voluntary_ctxt_switches";
        char* nonvoluntary = "nonvoluntary_ctxt_switches";

				if (!strcmp(voluntary, token)){
					token = strtok(NULL, "\n");
					printf("voluntary_ctxt_switches:%s\n",token);
				}
				if (!strcmp(nonvoluntary, token)){
					token = strtok(NULL, "\n");
					printf("nonvoluntary_ctxt_switches:%s\n",token);
				}
			}
		}
		fclose(file);
  }
  return;
}



/*-----------------For BG------------------------*/


void func_BG(char **cmd){
  //printf("%s", "Inside the bg\n");
  	pid_t pid = fork();
    
    if(pid == 0){ //child process
      char* command = cmd[1];
      execvp(command,&cmd[1]);
      printf("Error: failed to execute %s\n",command);
      exit(1);
    }
    else if(pid >0){ // store information of child process into node
      printf("Process created with id %d\n", pid);
      add_newNode(pid,cmd[1]);

    }else{
      printf("Error: fork failed\n");
    }
	
}


void func_BGlist() {
	//rintf("%s", "Inside the bglist\n");
	node_t *curr = head;
  // printf("pid %d\n", curr->pid);
  // printf("path %s\n", curr->path);
  // printf("head-path %s\n", head->path);
	int counter = 0;
  // while(curr != NULL){
    while(curr != NULL){
      counter++;
      printf("%d:  /%s\n", curr->pid, curr->path);
		  curr = curr->next;
  }
	printf("Total background jobs: %d\n", counter);
}



void func_BGkill(char * str_pid){
  //printf("%s", "Inside the bgkill!\n");
  pid_t pid = atoi(str_pid);
  printf("%d\n",pid);
	if(check_process_exist(pid) == 1){
    printf("in\n");
    if(!kill(pid, SIGTERM)){
      sleep(1);
    }else{
      printf("Error: failed to execute bgkill\n");
    }
  }else{
    printf("Error: invalid pid\n");
  }
  
}


void func_BGstop(char * str_pid){
  //printf("%s", "Inside the bgstop\n");
  pid_t pid = atoi(str_pid);
  if(check_process_exist(pid) == 1){
    if(!kill(pid, SIGSTOP)){
      sleep(1);
    }else{
      printf("Error: failed to execute bgstop\n");
    }
  }else{
    printf("Error: invalid pid\n");
  }
}


void func_BGstart(char * str_pid){
  //printf("%s", "Inside the bgstart\n");
  pid_t pid = atoi(str_pid);
  if(check_process_exist(pid) == 1){
    if(!kill(pid, SIGCONT)){
      sleep(1);
    }else{
      printf("Error: failed to execute bgstart\n");
    }
  }else{
    printf("Error: invalid pid\n");
  }
	
}

void func_pstat(char * str_pid){
	//printf("%s", "Inside the pstat\n");
  pid_t pid = atoi(str_pid);
  if(check_process_exist(pid) == 1){
    char path_stat[MAX_LINE_LEN];
    char path_status[MAX_LINE_LEN];
    sprintf(path_stat,"/proc/%d/stat", pid);
    sprintf(path_status, "/proc/%d/status", pid);
    read_stat(path_stat);
	  read_status(path_status);
  }else{
    printf("Error: Process %d does not exist.\n", pid);
  }
}

void zombie_process(){
  int status;
  pid_t pid;

  while(1){
    pid = waitpid(-1,&status,WNOHANG);

    if(pid >0){
      if(WIFSIGNALED(status)){
        head = remove_Node(pid);
        printf("Background process %d was killed.\n", pid);

      }
      if(WIFEXITED(status)){
        head = remove_Node(pid);
        printf("Background process %d was terminated.\n", pid);
        
      }
    }else{
      break;
    }
  }
  return;
}

 
int main(){
    //get user input
    char user_input_str[50];
    while (true) {
      printf("Pman: > ");
      fgets(user_input_str, 50, stdin);
      printf("User input: %s \n", user_input_str);
      char * ptr = strtok(user_input_str, " \n");
      if(ptr == NULL){
        continue;
      }
      char * lst[50];
      int index = 0;
      lst[index] = ptr;
      index++;
      while(ptr != NULL){
        ptr = strtok(NULL, " \n");
        lst[index]=ptr;
        index++;
      }
      //zombie_process();
      if (strcmp("bg",lst[0]) == 0){
        func_BG(lst);
      } else if (strcmp("bglist",lst[0]) == 0) {
        func_BGlist(lst);
      } else if (strcmp("bgkill",lst[0]) == 0) {
        func_BGkill(lst[1]);
      } else if (strcmp("bgstop",lst[0]) == 0) {
        func_BGstop(lst[1]);
      } else if (strcmp("bgstart",lst[0]) == 0) {
        func_BGstart(lst[1]);
      } else if (strcmp("pstat",lst[0]) == 0) {
        func_pstat(lst[1]);
      } else if (strcmp("q",lst[0]) == 0) {
        printf("Bye Bye \n");
        exit(0);
      } else {
        printf("Pman:> %s: command not found\n", lst[0]);
      }
      zombie_process();
    }

  return 0;
}