/*
*       Program:        client_base.c
*       Author:         Paul Girard Ph.D., DIM, UQAC
*       Date:           Sept 2013
*
*       Objective:      Show how to use a tcp socket
*                       on a client program running on
*                       Sun/ Solaris ; this program transmits
*                       a message to a server and receives
*                       an acknowledge each time.  A delay of
*                       one second is introduced between messages.
*
*       Options to compile on Solaris:          
*       ===>    gcc client_base.c -lsocket -lnsl -o client_base
*       Options to compile on Linux:          
*       ===>    gcc client_base.c -lnsl -o client_base
*
*       Execution: 3 parameters: program_name server_name server port_number
*       =========>  client_base server_machine port_number
*               ex. client_base dimensxcn1.uqac.ca 5---
*
*
*       Files to include
*/
#include <errno.h>         
/* #include <strings.h> */ 
#include <string.h>        
#include <sys/types.h>     
#include <sys/socket.h>    
#include <netinet/in.h>
#include <netdb.h>         
#include <stdio.h>
#include <time.h>


#define END "end"
/*
*      The following functions are used in this program
*
*               socket, gethostbyname, connect, 
*               read, write, close.
*/

		struct timespec time1 , time2 ,temp; /* time_t tv_sec ; seconds long tv_nsec ; nanoseconds */
		double delay ; 
		char local_filename[20];  /* save the local_file_name which is input by user  */ 
		char remote_filename[20]; /* save the remote_file_name which is input bu user */ 
		
		int c , line = 0 , size = 0 ;
        int sock2, msgsock;             /* socket descriptor */
        struct sockaddr_in server2;     /* structure used to assign a name
                                   to a socket sccording internet format*/
        size_t length;                  /* #octets in structure sockadr_in */
        struct hostent *hp,*gethostbyname(); /* structure to associate the
                                        server name and its address */
        struct hostent hpstruct; /* structure to access the server address*/
        char buf[128];         /* message buffer*/ 
        int rval;               /* status code for read */
		FILE *fptr; 			 /* pointer to file descriptor  */
		char fileContentBuf[81] ; /*read the file buffer*/

int inputFileName(){
		printf("myftp> ");
		if(scanf("put %s",local_filename) == EOF || scanf("%s",remote_filename) == EOF ){
	   		 return 0 ;
		}else{
			printf("ftp command=> put  local file=> %s  remote file=> %s\n",local_filename,remote_filename);
			return 1;
		}

}
int getConnection(){
		
		/*      5. Try a connection with the server
		*/
		
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID , &time1 ) ; /* read the time1 nanosec */
        if (connect(sock2, (struct sockaddr *)&server2, sizeof(server2)) < 0)
        {
                perror("Error at connect time");
                return 0;
        }else 
			return 1;
}
int getSocketBase(int argc , char *argv[]){ 

/*
*       1. Validation of the parameters read on the command line
*/
        if (argc != 3)
        {
          printf("Call the program : client_base server_name port_number\n");
          return 0;
        }
/*
*       2. Socket creation parameters:          
*               - AF_INET is the internet format 
*               - SOCK_STREAM specifies a TCP type socket
*               - 0 specifies to use the default protocol.
*/
        sock2 = socket(AF_INET, SOCK_STREAM, 0);
        if (sock2 < 0) 
        {
                perror("Error in creating a TCP socket");
                return 0;
        }
/*
*       3. Get a pointer to the network server address structure.
*          The server name is taken from the command line (argv[1]):
*               ex.  "sunens.uqac.ca"
*/
        server2.sin_family = AF_INET;   /* internet format*/
        hp = gethostbyname(argv[1]);    /* pointer to the network structure
                                        of the server listening on port
                                        argv[1] */
        if (hp == 0)
        {
                fprintf(stderr, "%s: unknown host ===>", argv[1]);
                return 0;
        }
        hpstruct = *hp; /*  save  hostent data */

/*
*       4. Fill in the remote address part
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
int sendFile(){
	if(openFile()){    /*   start read the file and to transfer the file*/
		
		if(write(sock2 ,remote_filename,strlen(remote_filename) ) < 0 ){
			perror("ERROR in ransmission the remote_filename") ;
			return 0 ;
		}
		while(fgets(fileContentBuf,81,fptr)){
			
			if(write(sock2, fileContentBuf,strlen(fileContentBuf)) < 0) {
					perror("ERROR in the ransmission the file") ;	
					return 0 ;
			}
		}
	}
	fclose(fptr) ;
	return 1;
}
int caculateFile(){
	
		if( (fptr = fopen(local_filename,"r") ) == NULL)
		{
			perror("error opening this file") ;
			return 0 ;
		}	
		while(!feof(fptr))
		{
			c = fgetc(fptr) ;
			if( c == '\n')
				line++ ;
			size = size + 1 ; 
		}
		fclose(fptr) ;
		return 1;
}
int receiveResponse(){

        if ((rval = read(sock2, buf, 1024)) < 0){
             perror("Error in reading a message");  
			 return 0 ;
		}
	    else if (rval == 0){
             printf("End of connection\n");
			 return 0 ;
		}
        else
        {
             buf[rval]='\0';
             printf("===>%s\n", buf);
			 return 1 ;
        }
}
int save(){

/*
*       6. Trnsmit 10 messages to the server and 
*       receive an ackknowledge from the server each time.
*/
		int i ;
        for (i=0; i<10; i++)
         {
                //   sleep(1);               /* zzzz: wait 1 second (option) */
				printf("%s --> %s",local_filename,remote_filename) ;
                if(write(sock2, buf, strlen(buf)) < 0)
                        perror("Error in the transmission of message");
                else { 
                        /*  read a message from the server */

                        if ((rval = read(sock2, buf, 1024)) < 0)
                           perror("Error in reading a message");
                        if (rval == 0)
                           printf("End of connection\n");
                        else
                        {
                           buf[rval]='\0';
                           printf("===>%s\n", buf);
                        }
                     }
         }
}

int main(int argc,char *argv[])

{

		if( !getSocketBase(argc,argv) )
		{
			return 0 ;
		}
		if( !getConnection() ){
			return 0 ;
		}
		printf("1111111111\n") ;
		if( !inputFileName() ){
			return 0 ;
		}		
		printf("2222222222\n") ;
		if( !sendFile() ) {
			return 0 ;
		}
		printf("3333333\n");
		if( !caculateFile()){
			return 0;
		}
		printf("4444444\n") ;

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2) ; /*read the timer2 in nanosec */
		if((time2.tv_nsec - time1.tv_nsec) < 0)
		{
			temp.tv_sec = time2.tv_sec - time1.tv_sec - 1 ;
			temp.tv_nsec = 1000000000 + time2.tv_nsec - time1.tv_nsec ;
		}else{
			temp.tv_sec = time2.tv_sec - time1.tv_sec ;
			temp.tv_nsec = time2.tv_nsec - time1.tv_nsec ;
		}
		delay = (double) (temp.tv_sec * 1000000 ) + (temp.tv_nsec/1000) ; /* delay in microsec */
		delay = (double) delay/1000 ;
		printf("File %s had %d lines and a total of %d octets\n",local_filename,line , size) ;
		printf("Transfer rate : %9.3f MO/sec \t Transfer delay:%9.3f msec\n",(double)((size*0.9765)/(1024*delay)) ,delay) ;
        close(sock2);
        puts("End of client program");
        return 0;        /* end of client program */
}

