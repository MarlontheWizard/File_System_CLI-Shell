//Author: Marlon Dominguez 
//Date: 03/02/2024 
//Operating Systems 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/*
---------------------------------------------------------------------------
|                              STRUCT OBJECTS                             |
---------------------------------------------------------------------------
*/

//Directory object 
typedef struct dir{
	char *name; 
	struct dir *dir_contents_head; //Dynamically allocated. Overall required size is not known at compile-time. (WORKING DIRECTORY)
	struct dir *next; 	           //Link to next obj in current directory
	struct dir *parent;            //Link to parent directory in current directory 
	int size; 	                   //Size of current directory. 
	int file; 
} dir; 

//User object 
typedef struct User{
	char *username; 
	char *password;
	void *dir_root;
	int FS_Size; //File System Size. 
	struct dir *current_dir;
} User;



/*
---------------------------------------------------------------------------
|                                  SEARCH                                 |
---------------------------------------------------------------------------
@param char *temp_ptr: Head of linked list to be searched in.
@param char *target: Name of the file/directory we are searching for.
@objective: Implement a method that will search for the target in File system 
            and return true or false. 
*/
//Search O(n) 
bool search(struct dir *temp_ptr, char *target){
	printf("\n\n");

    struct dir *ptr = temp_ptr->dir_contents_head;
	while(ptr != NULL){
        printf("%s\n", ptr->name);
		if(strcmp(ptr->name, target) == 0){
			return true; 
		}

		ptr = ptr->next;
	}

	return false; 
}


    
/*
---------------------------------------------------------------------------
|                                PWD                                      |
---------------------------------------------------------------------------
@param struct dir *ptr: File system current directory. 
@objective: Implement a method that will print the working directory.
            This method is implemented using recursion, specifically post-order.
            The recursive case works by passing the parent of each directory 
            every call until reaching the root directory. 
*/
void pwd(struct dir *ptr){
    
	//Base case
    if(ptr == NULL){
        return; 
    }
	
	//Recursive Case
	pwd(ptr->parent);
    printf("%s/", ptr->name);
}


/*
---------------------------------------------------------------------------
|                               MKDIR                                      |
---------------------------------------------------------------------------
@param char *path: Name of file/directory to be created.
@struct dir **current_dir: Pointer to a pointer so that we can modify linked list of structs. 
                           Points to current directory. 
@int type: Is it a file{1}? Or a directory{0}? 
@objective: Implement a method that will create a file/directory in the current directory 
            of the file system. 
*/
void mkdir(char *path, struct dir **current_dir, int type){
	
    //Dynamically allocate struct to insert into file system.
    //Also, initialize struct parameters.
    struct dir *insert = (struct dir *) malloc(sizeof(*insert));
    insert->name = strdup(path);
    insert->dir_contents_head = NULL;
    insert->next = NULL;
    insert->file = type;
    insert->parent = *current_dir; 



    //Head of linked list
    struct dir *ptr = (*current_dir)->dir_contents_head;

    //If empty, make new struct the head of the list. 
	if(ptr == NULL){
		(*current_dir)->dir_contents_head = insert;
		return;
	}

	//Else we must append to end of dir_contents_head 
	while(ptr->next != NULL){
        ptr = ptr->next;
	}		
	

	//Set the new directory
	(*ptr).next = insert;

	return;
}


/*
---------------------------------------------------------------------------
|                                 CD                                      |
---------------------------------------------------------------------------
@struct dir **current_dir: Pointer to a pointer so that we can modify linked list of structs. 
                           Points to current directory. 
@param char *path: Name of directory to be switched to.
@objective: Implement a method that will create change the current directory to the 
            target path taken as an argument in the file system. 
*/
void cd(struct dir **current_dir, char *path){

    //If list is empty, let user know directory does not exist
    if((*current_dir)->dir_contents_head == NULL){
        printf("Directory does not exist.\n");
    }
    
    //Create pointer to list head
    struct dir *ptr = (*current_dir)->dir_contents_head;

    //Search for target directory. 
    while(ptr != NULL){

        //If target directory is found, update current directory in system.
        if(strcmp(ptr->name, path) == 0){
            *current_dir = ptr;  
            return;
        }

        //If not yet found, keep iterating.
        ptr = ptr->next; 
    }

    //At this point, the directory is not in the system. 
    printf("Directory does not exist.\n");
    return;
}


/*
---------------------------------------------------------------------------
|                                 LS                                      |
---------------------------------------------------------------------------
@struct dir **current_dir: Pointer to a pointer so that we can modify linked list of structs. 
                           Points to current directory. 
@objective: Implement a method that will print the contents of the current directory.
*/
void ls(struct dir *current_dir){
    //Check if list is empty
	if(current_dir->dir_contents_head == NULL){
		printf("No elements present in directory."); 
		return;
	}
    
	struct dir *ptr = current_dir->dir_contents_head;
	while(ptr != NULL){
		printf("%s    ", ptr->name);
		ptr = ptr->next; 
	}
	
	return;
}

/*
---------------------------------------------------------------------------
|                                 HELP                                     |
---------------------------------------------------------------------------
@objective: Display the help menu to the user by opening the file and reading it to console. 
*/
void help(){
   //Open file
   FILE *file_ptr = fopen("help.txt", "r");
   int buf;
   if(file_ptr) {
   while ((buf = getc(file_ptr)) != EOF)
        printf("%c", buf);
   }
    fclose(file_ptr);

}

/*
---------------------------------------------------------------------------
|                                 HELP                                     |
---------------------------------------------------------------------------
@objective: Display the help menu to the user by opening the file and reading it to console. 
*/
void cp(struct dir **current_dir, char *source, char *dest){
    
    //Search to see if source file already exists. If not, let user know.
    if(!search(*current_dir, source)){
        printf("Source file is not in directory\n");
        return; 
    }
    
    //Search to see if dest file already exists. If not, create file in file system. 
    if(!search(*current_dir, dest)){
        printf("Target file is not in directory\n");
        mkdir(dest, current_dir, 1); //create file
    }

    
    //Copy contents.
    FILE *file_ptr = fopen(source, "r");
    FILE *dest_ptr = fopen(dest, "w");

    if (file_ptr == NULL || dest_ptr == NULL){ 
        printf("Cannot open one, or both, file(s).\n"); 
        return;
    } 

    char buf = fgetc(file_ptr);
    while (buf != EOF)
    {
        fputc(buf, dest_ptr);
        buf = fgetc(file_ptr);
    }
   

    fclose(file_ptr);
    fclose(dest_ptr); 

    return;
}


/*
---------------------------------------------------------------------------
|                                   RM                                    |
---------------------------------------------------------------------------
@param struct dir **current_dir: Pointer to a pointer so that we can modify linked list of structs. 
                           Points to current directory.
@param char *file_name: Name of the source file to be removed.
@param int type: Type of file, file{1} or directory{0}? 
@objective: Removes the the file from target directory(current dir).
*/
void rm(struct dir **current_dir, char *file_name, int type){
	
	struct dir *ptr = (*current_dir)->dir_contents_head;

	if(ptr == NULL){
		printf("No files in directory.\n");
		return;
	}
	
    if(ptr->next == NULL){
        free((*current_dir)->dir_contents_head); 
        return;
    }

    //Edge case: Removing First item
	if(strcmp(file_name, ptr->name) == 0){
        struct dir *temp = (struct dir *) malloc(sizeof(*temp));
        temp = ptr->next;
        ptr = NULL;
        (**current_dir).dir_contents_head = temp;
        return;
	}

	//Iterate through linked list looking for file
    struct dir *temp = ptr; 
	while(ptr->next != NULL){
		if(strcmp(ptr->name, file_name) == 0 && ptr->file == type){
            
            (*temp).next = (*ptr).next;
            free(ptr);
		    return; 
		}
        
		temp = ptr;
        
		ptr = ptr->next;
	}
	
	//Ensure we refer to correct object.
	char types[][10] = {"Directory", "File"};
	printf("%s does not exist.\n", types[type]);
	return;
}

/*
---------------------------------------------------------------------------
|                                   RMDIR                                 |
---------------------------------------------------------------------------
@param struct dir **current_dir: Pointer to a pointer so that we can modify linked list of structs. 
                           Points to current directory.
@param char *to_delete: Name of the source file to be removed.
@objective: Removes the directory from target directory(current dir).
            This method is very similar to rm, therefore it is implemented
            as such. We just pass in an argument to differentiate between
            the object type to be deleted.
*/
void rmdir(struct dir **current_dir, char *to_delete){
    rm(current_dir, to_delete, 0);
}


/*
---------------------------------------------------------------------------
|                                   MV                                    |
---------------------------------------------------------------------------
@param struct dir **current_dir: Pointer to a pointer so that we can modify linked list of structs. 
                           Points to current directory.
@param char *file: Name of the source file to be moved.
@param char *destination: Name of the target directory to move the file to.
@objective: Move the source file to target directory. Delete source file from original directory. 
****If file does not exist, the method returns without creating the file. However, it will 
    create a directory if not found.
*/
void mv(struct dir **current_dir, char *file, char *destination){

    //Make sure file exists
    if(search((*current_dir), file) == false){
        printf("File does not exist.\n");
        return;
    }

    //Make sure target directory exists, if not create it. 
    if(search((*current_dir), destination) == false){
        printf("Destination does not exist.\n");
        mkdir("Something_Raddddd", current_dir, 0); //create file
    }
    
    //Remove source file from current directory. 
    rm(current_dir, "dummy_data_5.txt", 1);
    
    struct dir *ptr = (*current_dir)->dir_contents_head;
   

    //If all arguments exist within the file system, move the file to directory. 
    //Search for file and store in temp 

    
    struct dir *destination_dir = NULL;
    while(ptr != NULL){
        if(strcmp(ptr->name, destination) == 0){
            destination_dir = ptr;
            break;
        }

        ptr = ptr->next;
    }

    if(ptr == NULL){
        printf("Didnt find destination. Creating it...\n");
        mkdir(destination, &destination_dir, 1); 
    }

    mkdir(file, &destination_dir, 1); 
    return;
}

/*
---------------------------------------------------------------------------
|                                 cd_back {cd.}                           |
---------------------------------------------------------------------------
@param struct dir **current_dir: Pointer to a pointer so that we can modify linked list of structs. 
                           Points to current directory.
@objective: Retrace to the previous directory 
*/
void cd_back(struct dir **current_dir){
    *current_dir = (*current_dir)->parent;
    return;
}





int main() {
    char input[1024]; //Input Stream 

    //Create User (Implemented for use in complete operating system later) 
    struct User * default_user = (struct User *) malloc(sizeof(*default_user));

    /*
    ---------------------------------------------------------------------------
    |                            TEST DATA Initialization                     |
    ---------------------------------------------------------------------------
    */
    default_user->username = "Marlon_Dominguez"; 
    default_user->password = "admin"; //Protection not focused on here. 
    default_user->FS_Size = 7; //Starts with Home directory as default. 
    
    struct dir *home = (struct dir *) malloc(sizeof(*home));
    struct dir *level_1 = (struct dir *) malloc(sizeof(*level_1));
    struct dir *level_2 = (struct dir *) malloc(sizeof(*level_2));

    
    level_1->name = "folder";
    
    level_1->file = 0;
    level_1->next = NULL;
   
    level_2->name = "Misc";
    level_2->next = NULL;
    level_2->parent = level_1;
    level_2->file = 0; 

    level_1->dir_contents_head = level_2;

    home->name = "Home"; 
    home->next = NULL;
    home->parent = NULL;
    home->file = 0;

    User users[1] = {*default_user};  //User 
	    
    //Initialize empty file_directory 
    struct dir *file1 = (struct dir *) malloc(sizeof(*file1));
    file1->name = "dummy_data_1.txt";
    file1->file = 1;
    struct dir *file2 = (struct dir *) malloc(sizeof(*file2));
    file2->name ="dummy_data_2.txt";
    file2->file = 1;

    file1->next = file2;
    file1->parent = home;

    struct dir *file3 = (struct dir *) malloc(sizeof(*file3));
    file3->name="dummy_data_3.txt";
    file3->file = 1;

    file2->next = file3;
    file2->parent = home;

    struct dir *file4 = (struct dir *) malloc(sizeof(*file4));
    file4->name="Random.java";
    file4->file=1;

    file3->next = file4;
    file3->parent = home;

    file4->next = level_1;
    file4->parent = home;

    level_1->next = NULL;
    level_1->parent = home; 


    home->dir_contents_head = file1;
    home->size = 4; 

    default_user->dir_root = home;  
    default_user->current_dir = home;

    /*
    ---------------------------------------------------------------------------
    |                               END TEST DATA                             |
    ---------------------------------------------------------------------------
    */


    /*
    ---------------------------------------------------------------------------
    |                                  DRIVER                                 |
    ---------------------------------------------------------------------------
    */
    
    while (1) {
        printf("myshell> ");
        fgets(input, 1024, stdin);

        input[strcspn(input, "\n")] = 0;

        char *command = strtok(input, " ");
        char *arg1 = strtok(NULL, " ");
        // You can extend this code to support more arguments

        if (strcmp(command, "cd") == 0) {
            cd(&(default_user->current_dir), arg1);
        }
       
        else if (strcmp(command, "pwd") == 0) {
            pwd(default_user->current_dir);
        }
        	
        else if (strcmp(command, "exit") == 0) {
            return 1;
        }
	
        else if (strcmp(command, "help") == 0) {
            help();
        }

        else if (strcmp(command, "mkdir") == 0) {
            mkdir(arg1, &(default_user->current_dir), 0);
        }
	
        else if (strcmp(command, "rmdir") == 0) {
            rm(&(default_user->current_dir), arg1, 0);
        }
	
        else if (strcmp(command, "ls") == 0) {
            ls(default_user->current_dir);
        }

	
        else if (strcmp(command, "cp") == 0) {
	        char *arg2 = strtok(NULL, " ");
            cp(&(default_user->current_dir), arg1, arg2);
        }


        else if (strcmp(command, "mv") == 0) { 
	        char *arg2 = strtok(NULL, " ");
	        mv(&(default_user->current_dir), arg1, arg2);
        }
		
        else if (strcmp(command, "rm") == 0) {
    	    rm(&(default_user->current_dir), arg1, 1);
        }

        else if (strcmp(command, "cd.") == 0) {
    	    cd_back(&(default_user->current_dir));
        }

	 // You will need to extend this code to support other commands
        else {
            printf("Unknown command: %s\n", command);
        }
    }
    
    /*
    ---------------------------------------------------------------------------
    |                              END DRIVER                                 |
    ---------------------------------------------------------------------------
    */
    return 0;
}
