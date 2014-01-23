
#include <iostream>
#include <string>
#include <fstream>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring> 	// used for memset.
#include <arpa/inet.h> 	// for inet_ntop function
#include <sys/socket.h>

#include <vector>
#include <list>
#include <iterator>
#include <sstream>

#include <errno.h>

using namespace std;



FILE *output;
//server functions
int server_start_listen() ;
int server_establish_connection(int server_fd);
int server_send(int fd, string data);
void *tcp_server_read(void *arg) ;
void mainloop(int server_fd) ;

//server constants
const  char * PORT = "12345" ; // port numbers 1-1024 are probably reserved by your OS
const int MAXLEN = 1024 ;   // Max lenhgt of a message.
const int MAXFD = 7 ;       // Maximum file descriptors to use. Equals maximum clients.
const int BACKLOG = 5 ;     // Number of connections that can wait in que before they be accept()ted

// This needs to be declared volatile because it can be altered by an other thread. Meaning the compiler cannot
// optimise the code, because it's declared that not only the program can change this variable, but also external
// programs. In this case, a thread.
volatile fd_set the_state;

pthread_mutex_t mutex_state = PTHREAD_MUTEX_INITIALIZER;


string cpy;
int main()
{

	

    cout << "Server started."  << endl ; // don not forgfet endl, or it won't display.

    // start the main and make the server listen on port 12345
    // server_start_listen(12345) will return the server's fd.

    int server_fd = server_start_listen() ;
    if (server_fd == -1)
    {
        cout << "An error occured. Closing program." ;
        return 1 ;
    }

    mainloop(server_fd);

    return 0;
}

int server_start_listen()
{

struct addrinfo hostinfo, *res;

int sock_fd;

int server_fd; // the fd the server listens on
int ret;
int yes = 1;

// first, load up address structs with getaddrinfo():

memset(&hostinfo, 0, sizeof(hostinfo));

hostinfo.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
hostinfo.ai_socktype = SOCK_STREAM;
hostinfo.ai_flags = AI_PASSIVE;     // fill in my IP for me

getaddrinfo(NULL, PORT, &hostinfo, &res);


    server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    //if(server_fd < 0) throw some error;

    //prevent "Error Address already in use"
    ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    // if(ret < 0) throw some error;

    ret = bind(server_fd, res->ai_addr, res->ai_addrlen);

    if(ret != 0)
    {
        cout << "error :" << strerror(errno) << endl;
        return -1 ;
    }

    ret = listen(server_fd, BACKLOG);
    						//if(ret < 0) throw some error;



return server_fd;

}

int server_establish_connection(int server_fd)
// This function will establish a connection between the server and the
// client. It will be executed for every new client that connects to the server.
// This functions returns the socket filedescriptor for reading the clients data
// or an error if it failed.
{
    char ipstr[INET6_ADDRSTRLEN];
    int port;


    int new_sd;
    struct sockaddr_storage remote_info ;
    socklen_t addr_size;

    addr_size = sizeof(addr_size);
    new_sd = accept(server_fd, (struct sockaddr *) &remote_info, &addr_size);
    //if (fd < 0) throw some error here;

    getpeername(new_sd, (struct sockaddr*)&remote_info, &addr_size);

   // deal with both IPv4 and IPv6:
if (remote_info.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&remote_info;
    port = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
} else { // AF_INET6
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&remote_info;
    port = ntohs(s->sin6_port);
    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
}

std::cout << "Connection accepted from "  << ipstr <<  " using port " << port << endl;

    return new_sd;

}

void *tcp_server_read(void *arg)
/// This function runs in a thread for every client, and reads incomming data.
/// It also writes the incomming data to all other clients.

{ 
    int rfd;
    char buf[MAXLEN]={'\0'};
	char file_name[MAXLEN]={'\0'};
    int buflen;
    int wfd;
char cpy[2]={'\0'};

    rfd = (int)arg;
int chk=0;
    for(;;)
    {

        //read incomming message.

{
chk=1;
	std::cout<<"Receiving File Name.."<<std::endl;
	int st_file;
st_file=recv(rfd,file_name,1000,0);
printf("%d\n",st_file);
file_name[st_file]='\0';
printf("%s\n",file_name);
send(rfd,file_name,strlen(file_name),0);
	
        std::cout<<"Receivin msg.."<<std::endl;
	buflen = recv(rfd, buf, 1000,0);

	printf("%s\n",buf);

	send(rfd,buf,strlen(buf),0);

	recv(rfd,cpy,2,0);

if(st_file>0)
{
	output=fopen("output.txt","a+");


	
	if(strcmp(cpy,"Y")==0)
	{
	if(!output)
	{ 
	printf("\nError!");
	}
	fprintf(output,"%s\n",file_name);
	fprintf(output,"%s\n",buf);
fclose(output);
	}

else
{
printf("\nData Incorrect");


}
}	
	

        if (buflen <= 0)
        {
            cout<< "client disconnected. Clearing fd. " << rfd << endl ;
            pthread_mutex_lock(&mutex_state);
            FD_CLR(rfd, &the_state);      // free fd's from  clients
            pthread_mutex_unlock(&mutex_state);
            (rfd);
           pthread_exit(NULL);
        }
}
    }
   
}

void mainloop(int server_fd)

// This loop will wait for a client to connect. When the client connects, it creates a
// new thread for the client and starts waiting again for a new client.
{
   
   pthread_t threads[MAXFD]; //create 10 handles for threads.
   FD_ZERO(&the_state); // FD_ZERO clears all the filedescriptors in the file descriptor set fds.
   while(1) // start looping here
    {
        int rfd;
        void *arg; 

        // if a client is trying to connect, establish the connection and create a fd for the client.
        rfd = server_establish_connection(server_fd);

        if (rfd >= 0)
        {
            cout << "Client connected. Using file desciptor " << rfd << endl;
            if (rfd > MAXFD)
            {
                cout << "To many clients trying to connect." << endl;
                close(rfd);
                continue;
            }

            pthread_mutex_lock(&mutex_state);  // Make sure no 2 threads can create a fd simultanious.

            FD_SET(rfd, &the_state);  // Add a file descriptor to the FD-set.

            pthread_mutex_unlock(&mutex_state); // End the mutex lock

            arg = (void *) rfd;


          

            // now create a thread for this client to intercept all incomming data from it.
            pthread_create(&threads[rfd], NULL, tcp_server_read, arg);
        }
    }
}



