/* run using ./server <port> */
/* run as:
	$ make clean
	$ make
	$ ./server 8080
*/
#include "http_server.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <queue>
#define THREAD_POOL_SIZE 25
pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var=PTHREAD_COND_INITIALIZER;
queue<int> q;
void handle_connection(int newsockfd);
void * thread_function(void *arg);
void handler(int a){
exit(0);
}
void error(const char *msg) {
  
  perror(msg);
  exit(1);
}
int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno;
	signal(SIGINT,handler);
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}
	for(int i=0;i<THREAD_POOL_SIZE;i++){
		pthread_create(&thread_pool[i], NULL, thread_function, NULL);
	}
	/* create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	/* fill in port number to listen on. IP address can be anything (INADDR_ANY)
	*/
	bzero((char *)&serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	/* bind socket to this port number on this machine */
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	/* listen for incoming connection requests */
	listen(sockfd, 100);
	clilen = sizeof(cli_addr);
	while((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen))>0){
		/* accept a new request, create a newsockfd */
		pthread_mutex_lock(&mutex);
		q.push(newsockfd);
		pthread_cond_signal(&condition_var);
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_destroy(&mutex);
	close(sockfd);
	return 0;
}
void* thread_function(void *arg){
	int sock;
	while(1){
		pthread_mutex_lock(&mutex);
		while(q.empty()){
			pthread_cond_wait(&condition_var, &mutex);
		}
		sock=q.front();
		q.pop();
		pthread_mutex_unlock(&mutex);
		handle_connection(sock);
	}
}
void handle_connection(int newsockfd){
	char buffer[4096];
	int n;   	
	bzero(buffer, 4096);
	n = read(newsockfd, buffer, 4095);
  	if (n < 0){
   		error("ERROR reading from socket");
  		exit(0);
  	}
  	else if(n==0){
  		close(newsockfd);
  	}
  	else{
  		//cout<<"hi am here\n";
	  	//cout<<"\n###REQUEST: buffer:\n"<<buffer<<endl;
	    	HTTP_Response *hresp;
	    	hresp=handle_request(buffer);
		string response=hresp->get_string();
		//string response="Bye";
		//cout<<"\n###RESPONSE: string:\n"<<response<<endl;
		n = write(newsockfd, response.c_str(), strlen(response.c_str()));
		if (n < 0)
			error("ERROR writing to socket");
	   	delete hresp;
	   	close(newsockfd);
   	}
}
