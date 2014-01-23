#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
int main(int argc,char *argv[])
{

    int status;
    struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
    struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

    // The MAN page of getaddrinfo() states "All  the other fields in the structure pointed
    // to by hints must contain either 0 or a null pointer, as appropriate." When a struct
    // is created in c++, it will be given a block of memory. This memory is not nessesary
    // empty. Therefor we use the memset function to make sure all fields are NULL.

    memset(&host_info, 0, sizeof host_info);
	char msg[1000];
		FILE *ptr_file;
    		char buf[1000];

    		ptr_file =fopen(argv[1],"r");
    		if (!ptr_file)
        		printf("\nFilE Error");

    		while (fgets(buf,1000, ptr_file)!=NULL)
		{

			strcat(msg,buf);
		}
		printf("%s\n",msg);
		fclose(ptr_file);



    std::cout << "Setting up the structs..."  << std::endl;

    host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

    // Now fill up the linked list of host_info structs with google's address information.
    status = getaddrinfo("127.0.0.1", "12345", &host_info, &host_info_list);
    
	// getaddrinfo returns 0 on succes, or some other value when an error occured.
	    // (translated into human readable text by the gai_gai_strerror function).

    if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;


    std::cout << "Creating a socket..."  << std::endl;
    int socketfd ; // The socket descripter
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                      host_info_list->ai_protocol);
    if (socketfd == -1)  std::cout << "socket error " ;


    std::cout << "Connect()ing..."  << std::endl;
    status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)  std::cout << "connect error" ;


    std::cout << "Sending File Name..."  << std::endl;
printf("%s\n",argv[1]);
    send(socketfd,argv[1],strlen(argv[1]),0);

char file_name[1000]={'\0'};
recv(socketfd,file_name,1000,0);

std::cout<<"Sending msg..."<<std::endl;
printf("%s\n",msg);

    int len;
    ssize_t bytes_sent;
    len = strlen(msg);
    bytes_sent = send(socketfd, msg, len, 0);
	

    std::cout << "Receiving data for approval..."  << std::endl;
    ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(socketfd, incomming_data_buffer,10000, 0);
    // If no data arrives, the program will just wait here until some data arrives.
    if (bytes_recieved == 0) std::cout << "host shut down." << std::endl ;
    if (bytes_recieved == -1)std::cout << "recieve error!" << std::endl ;
    std::cout << bytes_recieved << " bytes recieved :" << std::endl ;
    incomming_data_buffer[bytes_recieved] = '\0' ;
	
	char ch[2]={'\0'};
	     if(strcmp(msg,incomming_data_buffer)==0)
		ch[0]='Y';
	     else
		ch[0]='N';

	std::cout<<"Sending Acknowledgemen..."<<std::endl;
	send(socketfd, ch,strlen(ch),0);
    
    	std::cout << "Data Sent Successfully..." << std::endl;
    //freeaddrinfo(host_info_list);
    //close(socketfd);

}
