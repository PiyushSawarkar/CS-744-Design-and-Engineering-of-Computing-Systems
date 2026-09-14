/* run using: ./load_gen localhost <server port> <number of concurrent users> <think time (in s)> <test duration (in s)> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
//#include "http_server.hh"
#include <pthread.h>
#include <sys/time.h>

int time_up;
FILE *log_file;

// user info struct
struct user_info {
  // user id
  int id;

  // socket info
  int portno;
  char *hostname;
  float think_time;

  // user metrics
  int total_count;
  float total_rtt;
};

// error handling function
void error(char *msg) {
  perror(msg);
  exit(0);
}

// time diff in seconds
float time_diff(struct timeval *t2, struct timeval *t1) {
  return (t2->tv_sec - t1->tv_sec) + (t2->tv_usec - t1->tv_usec) / 1e6;
}

// user thread function
void *user_function(void *arg) {

  /* get user info */
  struct user_info *info = (struct user_info *)arg;


  int sockfd, n;
  char buffer[4096];
  struct timeval start, end;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  while (1) {
    /* start timer */
    gettimeofday(&start, NULL);

    /* TODO: create socket */
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
       error("ERROR opening socket");
  
    /* TODO: set server attrs */
    server = gethostbyname(info->hostname);

    if (server == NULL) {
       fprintf(stderr, "ERROR, no such host\n");
       exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(info->portno);
  
    /* TODO: connect to server */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
       error("ERROR connecting");
    /* TODO: send message to server */

    bzero(buffer, 4096);
    char *request="GET /apart3/flat32/index.html HTTP/1.1";
    //cout<<"\n###RESPONSE: string:\n"<<response<<endl;
    //write a http request to the server
    
    n = write(sockfd, request, strlen(request));
    //fgets(buffer, 4096, stdin);   
    // n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
  	  error("ERROR writing to socket");
  
    bzero(buffer, 4096);
    /* TODO: read reply from server */
   n = read(sockfd, buffer, 4095);
  
   if (n < 0){
      error("ERROR reading from socket");
      exit(0);
   }
   else if(n==0){
   	close(sockfd);
   }
   //printf("Echoed response from Server: %s\n", buffer);
    /* TODO: close socket */
   close(sockfd);
    /* end timer */
    gettimeofday(&end, NULL);
    /* if time up, break */
    if (time_up)
      break;
    /* TODO: update user metrics */
     info->total_count++;
     info->total_rtt+=time_diff(&end, &start);
    /* TODO: sleep for think time */

    sleep(info->think_time);
  
  }

  /* exit thread */
  fprintf(log_file, "User #%d finished\n", info->id);
  fflush(log_file);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int user_count, portno, test_duration;
  float think_time;
  char *hostname;
  
  if (argc != 6) {
    fprintf(stderr,"Usage: %s <hostname> <server port> <number of concurrent users> <think time (in s)> <test duration (in s)>\n",argv[0]);
    exit(0);
  }

  hostname = argv[1];
  portno = atoi(argv[2]);
  user_count = atoi(argv[3]);
  think_time = atof(argv[4]);
  test_duration = atoi(argv[5]);
  printf("Hostname: %s\n", hostname);
  printf("Port: %d\n", portno);
  printf("User Count: %d\n", user_count);
  printf("Think Time: %f s\n", think_time);
  printf("Test Duration: %d s\n", test_duration);
	
  /* open log file */

  log_file = fopen("load_gen.log", "a+");
	 
  if(log_file==NULL){
   //handle the error
   
   printf("something went wrong: %s", strerror(errno));
   exit(1);
}

  pthread_t threads[user_count];

  struct user_info info[user_count];

  struct timeval start, end;

  /* start timer */
  gettimeofday(&start, NULL);
  time_up = 0;
      
  for (int i = 0; i < user_count; ++i) {
    /* TODO: initialize user info */
	 info[i].id=i;
  	// socket info
	  info[i].portno=portno;
	  info[i].hostname=hostname;
	  info[i].think_time=think_time;
	  
	//user metric
	 info[i].total_count=0;
	 info[i].total_rtt=0;
	  
    /* TODO: create user thread */
	pthread_create(&threads[i], NULL, user_function, &info);
	
    fprintf(log_file, "Created thread %d\n", i);
    
    
  }
  /* TODO: wait for test duration */
	sleep(test_duration);
	//sleep(1);
	
  fprintf(log_file, "Woke up\n");

  /* end timer */
  time_up = 1;
  gettimeofday(&end, NULL);

  /* TODO: wait for all threads to finish */
  
 for (int i = 0; i < user_count; i++)
       pthread_join(threads[i], NULL);
  
  /* TODO: print results */
	//avg throughput
	int total_successfull_requests=0;
	for (int i = 0; i < user_count; i++){
              total_successfull_requests+=info[i].total_count;
	}
	float avg_throughput=(float)total_successfull_requests/test_duration;
	printf("\nAverage Throughput: %f",avg_throughput);
	//avg response time
	float total_response_time=0;
	for (int i = 0; i < user_count; i++){
              total_response_time+=info[i].total_rtt;
	}
	float avg_response_time=total_response_time/total_successfull_requests;
	printf("\nAverage Response Time: %f\n",avg_response_time);
	
	//accumulating results
	
	FILE *result_file = fopen("results.csv", "a+");
	if(result_file==NULL){
        //handle the error
   
	   printf("something went wrong: %s", strerror(errno));
	   exit(1);
        }
        fprintf(result_file, "%d,%f,%f\n", user_count, avg_throughput, avg_response_time);
        fclose(result_file);
  /* close log file */
  fclose(log_file);

  return 0;
}
