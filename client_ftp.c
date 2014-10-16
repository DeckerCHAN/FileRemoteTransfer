/* 
 *	Program:        client_base.c 
 *	Author:         Jeffrey.nam   
 *	TUT_NUMBER      20115877 
 *	Date:           Sept 2013 
 *	
 *	Objective:      Show how to use a tcp socket 
 *	on a client program running on 
 *	Sun/ Solaris ; this program transmits 
 *	a message to a server and receives *                       an acknowledge each time.  A delay of 
 *	one second is introduced between messages. 
 *	
 *	Options to compile on Solaris:           *       ===>    gcc client_ftp.c -lsocket -lnsl -o client_ftp *       Options to compile on Linux:           
 *	===>    gcc client_ftp.c -lrt -lnsl -o client_ftp 
 *	
 *	Execution: 3 parameters: program_name server_name server port_number 
 *	=========>  client_ftp server_machine 5161 
 *	
 *	
 */ 
#include <errno.h>          
#include <string.h>         
#include <sys/types.h>      
#include <sys/socket.h>     
#include <netinet/in.h> 
#include <netdb.h>          
#include <stdio.h> 
#include <time.h> 


/* 
 *	following functions are used in this program 
 *	
 *	socket, gethostbyname, connect,  *               read, write, close. 
 */ 

FILE *fptr;  	 	 	                                 /* pointer to file descriptor  */ 
struct timespec time1 , time2 ,temp; /* time_t tv_sec ; seconds long tv_nsec ; nanoseconds */ 
struct hostent *hp,*gethostbyname(); /* structure to associate the 
                                        server name and its address */ 
struct hostent hpstruct;                  /* structure to access the server address*/     
struct sockaddr_in server2;            /* structure used to assign a name to a socket sccording internet format*/ 
size_t length;                               /* #octets in structure sockadr_in */ 

char local_filename[20];                   /* save the local_file_name which is input by user  */  
char remote_filename[20];             /* save the remote_file_name which is input bu user */  	 
char fileContentBuf[10000] ;          /*read the file buffer*/  
int file_line = 0 , file_size = 0 ;         /*file file_line file_size*/ 
int sock2;                                              /* socket descriptor */  
double delay ; 


int inputFileName(){  
    printf("myftp> "); 
    if(scanf("put %s",local_filename) == EOF || scanf("%s",remote_filename) == EOF )
    { 
        return 0 ; 
    }else{ 
        printf("\nftp command=> put  local file=> %s  remote file=> %s\n",local_filename,remote_filename); 
        return 1; 
    } 
} 
int getConnection(){ 

    /*      5. Try a connection with the server 
    */ 
    printf("\n===================  Start   ======================\n") ; 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID , &time1 ) ; /* read the time1 nanosec */        
    if (connect(sock2, (struct sockaddr *)&server2, sizeof(server2)) < 0) 
    {               
        return 0; 
    }   	else    	 	 
        return 1; 

} 
int getSocketBase(int argc , char *argv[]){  

    /* 
     *	1. Validation of the parameters read on the command file_line 
     */         if (argc != 3)    
    {              
        return 0; 
    } 
    /* 
     *	2. Socket creation parameters:           
     *	- AF_INET is the internet format  *               - SOCK_STREAM specifies a TCP type socket *               - 0 specifies to use the default protocol. 
     */         sock2 = socket(AF_INET, SOCK_STREAM, 0);      
    if (sock2 < 0)      
    {                
        perror("Error in creating a TCP socket");             
        return 0; 
    } 
    /* 
     *	3. Get a pointer to the network server address structure. 
     *	The server name is taken from the command file_line (argv[1]): 
     *	ex.  "sunens.uqac.ca" 
     */         server2.sin_family = AF_INET;   /* internet format*/       
    hp = gethostbyname(argv[1]);         
    if (hp == 0)       
    {              
        fprintf(stderr, "%s: unknown host ===>", argv[1]);            
        return 0; 
    }        
    hpstruct = *hp; /*  save  hostent data */ 

    /* 
     *	4. Fill in the remote address part 
     */        
    server2.sin_family = hpstruct.h_addrtype;      
    server2.sin_port = htons(atoi(argv[2]));  /* char port # ==>integer port # 

                                                 ====> network 16 bits format */ 
    server2.sin_addr = * ((struct in_addr *) hpstruct.h_addr);   
    return 1; 
}
int transferFileName(){     
    if(write(sock2,local_filename ,strlen(local_filename)) < 0) 
        return 0 ; 
    else 
        return 1 ; 
} 
int openFile(){ 

    if( (fptr = fopen(local_filename,"r") ) == NULL) 
    { 
        perror("error opening this file") ; 
        return 0 ; 
    } 	   	 
    return 1 ;  
} 
int sendFile(){  if(openFile()){    /*   start read the file and to transfer the file*/ 

    if(write(sock2 ,remote_filename,strlen(remote_filename) ) < 0 )
    {   	 	
        perror("ERROR in ransmission the remote_filename") ; 
        return 0 ; 

    }
    while(fgets(fileContentBuf,1000,fptr))
    { 
        file_line++ ; 
        file_size+=strlen(fileContentBuf) ;   	 	 
        if(write(sock2, fileContentBuf,strlen(fileContentBuf)) < 0)
        {   	 	 	 	
            perror("ERROR in the ransmission the file") ; 	 
            return 0 ; 
        } 
    }   
} 
fclose(fptr) ;  return 1; 
} 

int main(int argc,char *argv[]) 
{ 
    if( !getSocketBase(argc,argv) ) 
    { 
        perror("Call the program : client_base server_name port_number\n"); 
        return 0 ; 
    } 
    if( !getConnection() ){         
        perror("Error at connect time\n"); 
        return 0 ; 
    } 
    if( !inputFileName() ){ 
        perror("error input format : put  location_file_name  remote_file_name\n") ; 
        return 0 ; 
    } 	 	 
    if( !sendFile() ) {   	 	 
        perror("ERROR in ransmission the File\n") ; 
        return 0 ; 
    } 

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2) ; /*read the timer2 in nanosec */   	
    if((time2.tv_nsec - time1.tv_nsec) < 0) 
    { 
        temp.tv_sec = time2.tv_sec - time1.tv_sec - 1 ;   	 	 
        temp.tv_nsec = 1000000000 + time2.tv_nsec - time1.tv_nsec ; 
    }else{ 
        temp.tv_sec = time2.tv_sec - time1.tv_sec ;  	 	 
        temp.tv_nsec = time2.tv_nsec - time1.tv_nsec ; 
    } 
    delay = (double) (temp.tv_sec * 1000000 ) + (temp.tv_nsec/1000) ;           /* delay in microsec */   	
    delay = (double) delay/1000 ; 
    printf("\nFile %s had %d file_lines and a total of %d octets\n",local_filename,file_line , file_size) ; 
    printf("Transfer rate : %9.3f MO/sec \t Transfer delay:%9.3f msec\n",(double)((file_size*0.9765)/(1024*delay)) ,delay) ;      
    close(sock2);   	
    puts("\nEnd of client program\t\tBy: 20115877_Zhang Yinan\n");   	
    printf("\n===================  END   ======================\n\n") ; 
    return 0;        /* end of client program */ 
} 
