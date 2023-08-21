
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 1024

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct superBlock{
    int totalINodes;
    int freeINodes;

}SUPERBLOCK,*PSUPPERBLOCK;

typedef struct iNode{
    char fileName[50];
    int iNodeNumber;
    int fileSize;
    int fileActualSize;
    int fileType;
    char *buffer;
    int linkCount;
    int referenceCount;
    int permission;
    struct iNode * next;

}INODE,*PINODE,**PPINODE;

typedef struct fileTable{

    int readOffset;
    int writeOffset;
    int count;
    int mode;
    PINODE ptrINode;

}FILETABLE,*PFILETABLE;

typedef struct ufdt{

    PFILETABLE ptrFiletable;

}UFDT;

UFDT UFDTArr[50];   
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

void man(char * name){

    if(name == NULL) return;

    if(strcmp(name,"create") == 0){

        printf("Discription: Used to create new Regular File\n");
        printf("Usage: create File_Name Permission\n");

    }
    else if(strcmp(name,"read") == 0){

        printf("Discription: Used to read data from  Regular File\n");
        printf("Usage: read File_Name No_Of_Bytes_to_Read \n");

    }
    else if(strcmp(name,"write") == 0){
        
        printf("Discription: Used to write data into Regular File\n");
        printf("Usage: write File_Name Data We want to write \n");

    }
    else if(strcmp(name,"ls") == 0){
        
        printf("Discription: Used to list all information of files\n");
        printf("Usage: ls \n");

    }
    else if(strcmp(name,"stat") == 0){
        
        printf("Discription: Used to  display information of files\n");
        printf("Usage: stat File_Name \n");

    }
    else if(strcmp(name,"fstat") == 0){
        
        printf("Discription: Used to  display information of files\n");
        printf("Usage: stat File_Descriptor \n");

    }
    else if(strcmp(name,"truncate") == 0){
        
        printf("Discription: Used to  remove data from file\n");
        printf("Usage: truncate File_name \n");

    }
    else if(strcmp(name,"open") == 0){
        
        printf("Discription: Used to  open existing file\n");
        printf("Usage: open File_name mode \n");

    }
    else if(strcmp(name,"close") == 0){
        
        printf("Discription: Used to close opened file\n");
        printf("Usage: open File_name mode \n");

    }
    else if(strcmp(name,"closeall") == 0){
        
        printf("Discription: Used to close all opened file\n");
        printf("Usage: closeall \n");

    }
    else if(strcmp(name,"lseek") == 0){
        
        printf("Discription: Used to change file offset\n");
        printf("Usage: lseek File_ame ChnageInOffset StartPoint \n");

    }
    else if(strcmp(name,"rm") == 0){
        
        printf("Discription: Used to delete the file.\n");
        printf("Usage: rm File_Name\n");

    }
    else{

        printf("ERROR: No Manual Entry Available\n");
    }
}
 

void displayHelp(){

    printf("\t\t Applicable Commands in out Project\n");
    printf("---------------- ------------------- --------------------------\n");
    printf("Command No.\t   Command Name\t\t   Command Description\n");
    printf("---------------- ------------------- --------------------------\n");

    printf("    1       \t      ls    \t\tTo List Out All Files\n");
    printf("    2       \t      clear \t\tTo Clear Console \n");
    printf("    3       \t      open  \t\tTo Open the File\n");
    printf("    4       \t      close  \t\tTo Close the File \n");
    printf("    5       \t      closeall\t\tTo Close All Opened Files\n");
    printf("    6       \t      read  \t\tTo Read all the contents fron File \n");
    printf("    7       \t      write \t\tTo Write Content into File\n");
    printf("    8       \t      exit  \t\tTo Terminate File System \n");
    printf("    9       \t      stat  \t\tTo Display informationof file using name\n");
    printf("   10       \t      fstat  \t\tTo Display infromation file using file descriptor \n");
    printf("   11       \t      truncate\t\tTo Remove all the data from file\n");
    printf("   12       \t      rm    \t\tTo Delete the file \n");


}


int getFdFromName(char *name){

    int iCnt = 0;

    while(iCnt < MAXINODE){

        if(UFDTArr[iCnt].ptrFiletable != NULL)
                if(strcmp((UFDTArr[iCnt].ptrFiletable->ptrINode->fileName),name) == 0)
                    break;
        iCnt++;
    }   

    if(iCnt == 50) return -1;
    else 
        return iCnt;
}


PINODE getINode(char * name){

    PINODE temp = head;
    int iCnt = 0;
    if(name == NULL){
        return NULL;
    }
    else{
        while(temp!= NULL){
            if(strcmp(name,temp->fileName)== 0){
                break;
            }
            temp= temp->next;
        }
    }
    return temp;
}

void initialiseSuperBlock(){  // Initializes the SuperBlock.
    int iCnt = 0;
    while(iCnt < MAXINODE){

        UFDTArr[iCnt].ptrFiletable = NULL;
        iCnt++;
    }
    SUPERBLOCKobj.totalINodes = MAXINODE;
    SUPERBLOCKobj.freeINodes = MAXINODE;
}


void createDILB(){  // Creating INODE LINKEDLIST.

    int iCnt = 1;
    PINODE newn = NULL;
    PINODE temp = head;

    while(iCnt <= MAXINODE){

        newn = (PINODE)malloc(sizeof(INODE));
        newn->linkCount = 0;
        newn->referenceCount = 0;
        newn->fileType = 0;
        newn->fileSize = 0;
        newn->buffer = NULL;
        newn->next = NULL;
        newn->iNodeNumber = iCnt;

        if(temp == NULL){

            head = newn;
            temp = head;
        }
        else{
            temp->next = newn;
            temp = temp->next;
        }

        iCnt++;
    }

    printf("DILB Created Successfully\n");
}


int createFile(char * name, int permission){

    int fd = 3;

    PINODE temp = head;

    if(name == NULL || (permission == 0) || (permission >3) ){
        return -1;
    }
    if(SUPERBLOCKobj.freeINodes == 0){
        return -2;
    }
    (SUPERBLOCKobj.freeINodes)--;
    if(getINode(name)!= NULL){
        return -3;
    }

    while(temp != NULL){

        if(temp->fileType == 0)
            break;
        temp = temp->next;    
    }

    while(fd < 50){
        if(UFDTArr[fd].ptrFiletable == NULL)
            break;
        fd++;    
    }   

    UFDTArr[fd].ptrFiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    UFDTArr[fd].ptrFiletable->count = 1;
    UFDTArr[fd].ptrFiletable->mode = permission;
    UFDTArr[fd].ptrFiletable->readOffset = 0;
    UFDTArr[fd].ptrFiletable->writeOffset = 0;

    UFDTArr[fd].ptrFiletable->ptrINode = temp;

    strcpy(UFDTArr[fd].ptrFiletable->ptrINode->fileName,name);
    UFDTArr[fd].ptrFiletable->ptrINode->fileType = REGULAR;
    UFDTArr[fd].ptrFiletable->ptrINode->referenceCount = 1;
    UFDTArr[fd].ptrFiletable->ptrINode->linkCount = 1;
    UFDTArr[fd].ptrFiletable->ptrINode->fileSize = MAXFILESIZE;
    UFDTArr[fd].ptrFiletable->ptrINode->permission=permission;
    UFDTArr[fd].ptrFiletable->ptrINode->buffer =(char *)malloc(MAXFILESIZE);

    return fd;

}

int rm_File(char * name){

    int fd = 0;

    fd = getFdFromName(name);
    if(fd == -1)
        return -1;
    (UFDTArr[fd].ptrFiletable->ptrINode->linkCount)--;

    if(UFDTArr[fd].ptrFiletable->ptrINode->linkCount == 0){

        UFDTArr[fd].ptrFiletable->ptrINode->fileType = 0;
        free(UFDTArr[fd].ptrFiletable->ptrINode->buffer);
        free(UFDTArr[fd].ptrFiletable);
    }    

    UFDTArr[fd].ptrFiletable = NULL;
    (SUPERBLOCKobj.freeINodes)++;

    return fd;
} 

int readFile(int fd, char *arr, int iSize){

        int read_size = 0;

        if(UFDTArr[fd].ptrFiletable == NULL) return -1;

        if(UFDTArr[fd].ptrFiletable->mode != READ && UFDTArr[fd].ptrFiletable->mode != READ+WRITE)
            return -2;
        if(UFDTArr[fd].ptrFiletable->readOffset == UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize) return -3;

        if(UFDTArr[fd].ptrFiletable->ptrINode->fileType != REGULAR) return -4;

        read_size = (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize)-(UFDTArr[fd].ptrFiletable->readOffset);

        if(read_size<iSize){

            strncpy(arr,(UFDTArr[fd].ptrFiletable->ptrINode->buffer)+(UFDTArr[fd].ptrFiletable->readOffset),read_size);
            UFDTArr[fd].ptrFiletable->readOffset = UFDTArr[fd].ptrFiletable->readOffset+read_size;
        }  

        return iSize;  
}


int writeFile(int fd,char *data,int iSize){

        int result = 0;

        if(((UFDTArr[fd].ptrFiletable->mode)!=WRITE) && ((UFDTArr[fd].ptrFiletable->mode)!=(READ+WRITE)))
            return -1;

        if(((UFDTArr[fd].ptrFiletable->ptrINode->permission)!=WRITE) && ((UFDTArr[fd].ptrFiletable->ptrINode->permission)!=(READ+WRITE)))
            return -1;

        if((UFDTArr[fd].ptrFiletable->writeOffset) == MAXFILESIZE)
            return -2;

        if((UFDTArr[fd].ptrFiletable->ptrINode->fileType)!= REGULAR)
            return -3;

        strncpy((UFDTArr[fd].ptrFiletable->ptrINode->buffer)+(UFDTArr[fd].ptrFiletable->writeOffset),data,iSize);

        (UFDTArr[fd].ptrFiletable->writeOffset) = (UFDTArr[fd].ptrFiletable->writeOffset)+iSize;

        (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize) = (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize)+iSize;

        return iSize;

}

int openFile(char *name,int mode){

    int i = 0;
    PINODE temp = NULL;

    if(name == NULL || mode <= 0)
        return -1;

    temp = getINode(name);

    if(temp == NULL)
        return -2;
       
    if(temp->permission < mode)
        return -3;

    while(i < 50){

        if(UFDTArr[i].ptrFiletable == NULL)
                break;
        i++;        
    }    

    UFDTArr[i].ptrFiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrFiletable == NULL) return -1;
    UFDTArr[i].ptrFiletable->count = 1;
    UFDTArr[i].ptrFiletable->mode = mode;
    if(mode == READ + WRITE){

        UFDTArr[i].ptrFiletable->readOffset = 0;
        UFDTArr[i].ptrFiletable->writeOffset = 0;
    }
    else if(mode == READ){

        UFDTArr[i].ptrFiletable->readOffset  = 0;
    }
    else if(mode == WRITE){

        UFDTArr[i].ptrFiletable->writeOffset = 0;
    }

    UFDTArr[i].ptrFiletable->ptrINode = temp;
    (UFDTArr[i].ptrFiletable->ptrINode->referenceCount)++;

    return i;
}

void closeFileByFd(int fd){

    UFDTArr[fd].ptrFiletable->readOffset=0;
    UFDTArr[fd].ptrFiletable->writeOffset=0;
    (UFDTArr[fd].ptrFiletable->ptrINode->referenceCount)--;

}

int closeFileByName(char *name){

    int iCnt = 0;
    iCnt = getFdFromName(name);
    if(iCnt == -1)
        return -1;

    UFDTArr[iCnt].ptrFiletable->readOffset = 0;
    UFDTArr[iCnt].ptrFiletable->writeOffset = 0;
    (UFDTArr[iCnt].ptrFiletable->ptrINode->referenceCount)--;

    return 0;    
}

void closeAllFiles(){

    int iCnt = 0;
    while(iCnt < MAXINODE){

        if(UFDTArr[iCnt].ptrFiletable != NULL){
            UFDTArr[iCnt].ptrFiletable->readOffset = 0;
            UFDTArr[iCnt].ptrFiletable->writeOffset = 0;
            (UFDTArr[iCnt].ptrFiletable->ptrINode->referenceCount)--;
            break;
        }
        iCnt++;
    }
}


int lSeekFile(int fd,int iSize,int from){

    if((fd<0) ||(from >2)) 
        return -1;
    if((UFDTArr[fd].ptrFiletable) == NULL) 
        return -1;;

    if((UFDTArr[fd].ptrFiletable->mode == READ) || (UFDTArr[fd].ptrFiletable->mode== (READ+WRITE)) ){

        if(from == CURRENT){
            if(((UFDTArr[fd].ptrFiletable->readOffset)+iSize) > (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize)){
                return -1;
            }

            if(((UFDTArr[fd].ptrFiletable->readOffset)+iSize) < 0){
                return -1;
            }

            (UFDTArr[fd].ptrFiletable->readOffset) = ((UFDTArr[fd].ptrFiletable->readOffset)+iSize);
        }   
        else if(from == START){

            if(iSize >((UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize))){
                return -1;
            }
            if(iSize < 0){
                return -1;
            }
            (UFDTArr[fd].ptrFiletable->readOffset) = iSize;
        }
        else if(from == END){

            if(((UFDTArr[fd].ptrFiletable->readOffset)+iSize)>MAXFILESIZE){
                return -1;
            }
            if(((UFDTArr[fd].ptrFiletable->readOffset)+iSize)<0){
                return -1;
            }

            (UFDTArr[fd].ptrFiletable->readOffset) = (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize) + iSize;
        }
    }

    else if((UFDTArr[fd].ptrFiletable->mode)== WRITE){

            if(from == CURRENT){

                if(((UFDTArr[fd].ptrFiletable->writeOffset)+iSize) > MAXFILESIZE){
                    return -1;
                }

                if(((UFDTArr[fd].ptrFiletable->writeOffset)+iSize) < 0){
                    return -1;
                }

                if(((UFDTArr[fd].ptrFiletable->writeOffset)+iSize) > (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize)){

                    (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize) = ((UFDTArr[fd].ptrFiletable->writeOffset)+iSize); 
                }

                (UFDTArr[fd].ptrFiletable->writeOffset) = ((UFDTArr[fd].ptrFiletable->writeOffset)+iSize);
        }   
        else if(from == START){

            if(iSize > MAXFILESIZE){
                return -1;
            }
            if(iSize < 0){
                return -1;
            }
            if(iSize > (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize)){

                        (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize) = iSize;
            }

            (UFDTArr[fd].ptrFiletable->writeOffset) = iSize;
        }
        else if(from == END){

            if(((UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize)+iSize)>MAXFILESIZE){
                return -1;
            }
            if(((UFDTArr[fd].ptrFiletable->writeOffset)+iSize)<0){
                return -1;
            }

            (UFDTArr[fd].ptrFiletable->writeOffset) = (UFDTArr[fd].ptrFiletable->ptrINode->fileActualSize) + iSize;
        }
    }

    return 0;
}
void ls_file(){  // Provides Details of Files created in our File System.
    
    int iCnt = 0;
    PINODE temp = head;

    if(SUPERBLOCKobj.freeINodes == MAXINODE){
        printf("Error: There are No Files\n");
        return;
    }

    printf("\n File Name \t INode Number \t File Size \t Link Count\n");
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    while(temp != NULL){

        if(temp->fileType != 0 ){

            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->fileName,temp->iNodeNumber,temp->fileActualSize,temp->linkCount);

        }
        temp = temp->next;

    }
    printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

}

int fstat_file(int fd){

    PINODE temp = head;
    int iCnt = 0;

    if(fd < 0 ) return -1;

    if(UFDTArr[fd].ptrFiletable == NULL) return -2;

    temp = UFDTArr[fd].ptrFiletable->ptrINode;

    printf("\n+++++++++++++ Statistical Information About File ++++++++++\n");
    printf("File Name: %s\n",temp->fileName);
    printf("Inode Number: %d\n",temp->iNodeNumber);
    printf("File Size: %d\n",temp->fileSize);
    printf("Actual File Size: %d\n",temp->fileActualSize);
    printf("Link count: %d\n",temp->linkCount);
    printf("Reference count: %d\n",temp->referenceCount);

    if(temp->permission == 1)
        printf("File Permission: Read Only\n");
    else if(temp->permission == 2)
        printf("File Permission: Write Only\n");
    else if(temp->permission == 3)  
        printf("File Permission: Read & Write\n");

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");  

    return 0;

}

int stat_file(char *name){

    PINODE temp = head;
    int iCnt = 0;

    if(name == NULL) return -1;

    while(temp != NULL){
        if(strcmp(name,temp->fileName)==0){
            break;
        }
        temp = temp->next;
    }

    if(temp == NULL) return -2;

    printf("\n+++++++++++++ Statistical Information About File ++++++++++\n");
    printf("File Name: %s\n",temp->fileName);
    printf("Inode Number: %d\n",temp->iNodeNumber);
    printf("File Size: %d\n",temp->fileSize);
    printf("Actual File Size: %d\n",temp->fileActualSize);
    printf("Link count: %d\n",temp->linkCount);
    printf("Reference count: %d\n",temp->referenceCount);

    if(temp->permission == 1)
        printf("File Permission: Read Only\n");
    else if(temp->permission == 2)
        printf("File Permission: Write Only\n");
    else if(temp->permission == 3)  
        printf("File Permission: Read & Write\n");

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");  

    return 0;        
}       


int truncate(char *name){

    int fd = getFdFromName(name);
    if(fd == -1){
        return -1;
    }
    memset((UFDTArr[fd].ptrFiletable->ptrINode->buffer),0,1024);
    UFDTArr[0].ptrFiletable->readOffset = 0;
    UFDTArr[0].ptrFiletable->writeOffset = 0;
    UFDTArr[0].ptrFiletable->ptrINode->fileActualSize = 0;

    return 0;
}


int main(){

    char *ptr = NULL;
    int ret = 0,fd=0,count=0;
    char command[4][80],str[80],arr[1024];
    char ch;

    initialiseSuperBlock(); // Defined At Line 61.
    createDILB(); // Defined At Line 72.
    
    while(1){

            fflush(stdin);  // Removes Every Character From Input Buffer.
            strcpy(str,""); // Clears str.

            printf("\nCVFS:>");
            fgets(str,80,stdin); // scanf("%[^'\n']s",str); Takes Command Input.

            // Counting No. Of Tokens(Words) in command for further Exectution:
            count = sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]); // Taking Each word of command in command array (Tokenization).

            if(count == 1){

                if(strcmp(command[0],"ls") == 0){

                    ls_file();  // Defined At Line 105
                }
                else if(strcmp(command[0],"exit") == 0){

                    printf("Terminating File System....");
                    printf("Bye Bye👋🏻\n");
                    break;
                }
                else if(strcmp(command[0],"closeall") == 0){
                    
                    closeAllFiles();
                    printf("All Files Closed Successfully\n");
                    continue;
                }
                else if(strcmp(command[0],"clear") == 0){
                    
                    system("clear");
                    continue;
                }
                else if(strcmp(command[0],"help") == 0){

                    displayHelp();
                    continue;
                }
                else{

                    printf("\nERROR: Command Not Found !!! \n");
                    continue;
                }

            }
            else if(count == 2){

                if(strcmp(command[0],"stat") == 0){

                        ret = stat_file(command[1]);
                        if(ret == -1){
                            printf("ERROR: Incorrect Parameters\n");
                        }
                        else if(ret == -2){
                            printf("ERROR: There is no such file\n");
                        }
                        continue;
                }
                else if(strcmp(command[0],"fstat") == 0){

                        ret = fstat_file(atoi(command[1]));
                        if(ret == -1){
                            printf("ERROR: Incorrect Parameters\n");
                        }
                        else if(ret == -2){
                            printf("ERROR: There is no such file\n");
                        }
                        continue;
                }
                else if(strcmp(command[0],"close")==0){

                        ret = closeFileByName(command[1]);
                        if(ret == -1)
                            printf("ERROR: There is no such File\n");
                        continue;    
                }
                else if(strcmp(command[0],"rm") == 0){

                        ret = rm_File(command[1]);
                        if(ret == -1)
                            printf("ERROR : There is no such file\n");

                        continue;    
                } 
                else if(strcmp(command[0],"man") == 0){

                        man(command[1]);
                }
                else if(strcmp(command[0],"write") == 0){

                        fd = getFdFromName(command[1]);
                        if(fd == -1){
                            printf("ERROR: Incorrect parameter\n");
                            continue;
                        }
                        printf("Enter the Data :\n");
                        scanf("%[^'\n']s",arr);

                        ret = strlen(arr);
                        if(ret == 0){

                            printf("ERROR: Incorrect parameter\n");
                            continue;
                        }

                        ret = writeFile(fd,arr,ret);
                        if(ret == -1)
                            printf("ERROR: Permission Denied\n");
                        if(ret == -2)
                            printf("ERROR: There is no sufficient memory to write\n");
                        if(ret == -3)
                            printf("ERROR: It is not regular file\n"); 

                        printf("%d bytes written in your file\n",ret);       
                }
                else if(strcmp(command[0],"truncate") == 0){

                        ret = truncate_File(command[1]);
                        if(ret == -1)
                            printf("ERROR: Incorrect Parameters");
                }
                else{

                    printf("\nERROR: Command Not Found !!! \n");
                    continue;
                }  

            }
            else if(count == 3){

                if(strcmp(command[0],"create")==0){

                    ret = createFile(command[1],atoi(command[2]));
                    if(ret >= 0)
                        printf("File is successfully created with file descriptor: %d",ret);

                    if(ret == -1)    
                        printf("ERROR: Incorrect parameter\n");
                    if(ret == -2) 
                        printf("ERROR: There is no iNodes\n");
                    if(ret == -3)
                        printf("ERROR: File Already Exists\n");
                    if(ret == -4)
                        printf("ERROR: Memory allocation failure\n");
                    continue;               
                }
                else if(strcmp(command[0],"open") == 0){
                    
                    ret = openFile(command[1],atoi(command[2]));
                    if(ret > 0)
                        printf("File is successfully opened with file descriptor : %d\n",ret);
                    if(ret == -1)
                        printf("ERROR: Incorrect Parameters\n");
                    if(ret == -2)
                        printf("ERROR: File not present\n");
                    if(ret == -3)
                        printf("ERROR: Permission denied\n");
                    continue;
                }
                else if(strcmp(command[0],"read") == 0){

                    fd = getFdFromName(command[1]);
                    if(fd == -1){
                        printf("ERROR: Incorrect Parameter\n");
                        continue;
                    }
                    ptr = (char *)malloc(sizeof(atoi(command[2]))+1);

                    if(ptr == NULL){

                        printf("ERROR: Memory allocation failure\n");
                        continue;
                    }

                    ret = readFile(fd,ptr,atoi(command[2]));
                    if(ret == -1)
                        printf("ERROR: File not exist\n");
                    if(ret == -2)
                        printf("ERROR: Permission Denied\n");
                    if(ret == -3) 
                        printf("ERROR: Reached at end of file\n");       
                    if(ret == -4)   
                        printf("ERROR: It is not Regular file\n");
                    if(ret == 0)
                        printf("ERROR: File Empty\n");
                    if(ret>0){
                        printf("%s",ptr);
                        //write(2,ptr,ret);
                        scanf("%c",&ch);
                    }        
                    continue;
                }


            }
            else if(count == 4){

                if(strcmp(command[0]="lseek")==0){

                    fd = getFdFromName(command[1]);
                    if(fd == -1){
                        printf("Error: Incorrect Paramenter\n");
                        continue;
                    }

                    ret = lSeekFile(fd,atoi(command[2]),atoi(command[]3));
                    if(ret == -1){
                        printf("Error: Unable to perform Lseek\n");
                    }
                }
                else{
                    printf("Error: Command not Found !!\n");
                }

            }
            else{
                printf("Error: Coommand not Found\n");
                continue;
            }

    }

    return 0;
}
