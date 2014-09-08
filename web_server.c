/*#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define ERROR -1
#define BUF 1000
#define DEFAULT_HTTP_PORT 80

//Brent McCorvey

typedef struct {
  const char* ext;
  const char* type;
} extn;

extn extensions[] = {
  {"txt", "text/plain"},
  {"htm", "text/html"}, 
  {"html", "text/html"}, 
  {"jpg", "image/jpg"}, 
  {"jpeg", "image/jpeg"}, 
  {"gif", "image/gif"}
};

// Globals
unsigned short g_usPort;

// Function Prototypes
void parse_args(int argc, char **argv);

int main(int argc, char **argv)
{
  parse_args(argc, argv);
  printf("Starting Web server on port: %hu\n", g_usPort);

  //IMPLEMENT HERE



}

void parse_args(int argc, char **argv)
{
  if (argc > 2)
  {
    fprintf(stderr,
            "too many arguments");
    exit(1);
  }
  if(argc==2) {
    g_usPort = atoi(argv[1]);
  }
  if(argc == 1) {
    g_usPort = DEFAULT_HTTP_PORT;
  }
}*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

unsigned short g_usPort;

#define DEFAULT_HTTP_PORT 80

char *error_return = "HTTP/1.1 404 Not Found\r\n\r\n<html><head>\r\n<title>404 Not Found</title>"
                      "\r\n</head><body>\r\n<h1>Not Found</h1>\r\n<p>The requested URL was not found"
                      " on this server.</p>\r\n</body></html>";
char *all_good = "200 OK";
char *flag;
char ret_buf[32768];

//TO FIX 404 PUT MASSIVE REQUEST IN ERROR RETURN AND MAKE VALUES GLOBAL
//TODO FIX CONTENT TYPE HEADER FIELD
//TODO MAKE WORK WITH IMAGES
//TODO CLEAN UP DEBUG DEFINE STATEMENT

char *read_response(char * buf, int num_buf)
{

  printf("%s\n",buf); //GET /hello.txt ...

  int i;
  struct stat sb;
  char *cp ,*cp2;
  char *res_head = (char*)malloc(100);
  FILE *f;
  cp = buf + 5;
  cp2 = strstr(cp," HTTP");
  if(cp2 != NULL) *cp2 = '\0';

  printf("file is: %s\n",cp);

  const char *stat_buf = cp; //Change to variable
  if(stat(stat_buf,&sb) == -1) {
    perror("stat function error");
    return error_return;
  }

  f = fopen(cp,"r");
  if(f == NULL) {
    flag = error_return;
    return error_return;
  } 
  else {
    flag = all_good;
  }
  i = fread(ret_buf,1,32768,f);

  printf("%d bytes read from file %s\n",i,cp);

  if(i == 0){
    fclose(f);
    flag = error_return;
    return error_return;
  }
  else {
    flag = all_good;
  }

  ret_buf[i] = '\0';
  fclose(f);

  sprintf(res_head, "HTTP/1.1 %s\r\nLast-Modified: %sContent-Length: %d\r\nContent-Type: %d\r\n\r\n",\
                    flag, ctime(&sb.st_mtime),i,sb.st_mode);
  strcat(res_head,ret_buf);

  return res_head;
}

void parse_args(int argc, char **argv)
{
  if (argc > 2)
  {
    fprintf(stderr, "too many arguments");
    exit(1);
  }
  if(argc==2) {
    g_usPort = atoi(argv[1]);
  }
  if(argc == 1) {
    g_usPort = DEFAULT_HTTP_PORT;
  }
}

int main(int argc,char **argv)
{

  parse_args(argc, argv);
  printf("Starting Web server on port: %hu\n", g_usPort);

  int i, sock;
  char *recvBuffer = (char *)malloc(4001);

  int rc = 0;
  int serv_sock;

  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  
  socklen_t cli_addr_size;

  int totalReceived = 0;
  int size;
  int totalSent;
  int bytesSent;
  char *cbuf;

  serv_sock = socket(AF_INET, SOCK_STREAM,0);
  if(serv_sock == -1){
    perror("Socket not created");
    exit(1);
  }
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(g_usPort);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(&(serv_addr.sin_zero),0,8);

  printf("Binding server socket to port %d\n",g_usPort);

  rc = bind(serv_sock,(struct sockaddr*) &serv_addr,sizeof(struct sockaddr));
  if(rc == -1){
    perror("Bind failure");
    exit(1);
  }
  rc = listen(serv_sock,10);
  if(rc == -1){
    perror("Listen error");
    exit(1);
  }

  printf("Accepting connections ... Yayyyyy\n");

  while(1){
    cli_addr_size = sizeof(struct sockaddr_in);
    do {
      sock =  accept(serv_sock, (struct sockaddr*) &cli_addr, &cli_addr_size);
    }
    while((sock == -1) && (errno == EINTR));
    if(sock == -1){
      perror("Bad accept");
      exit(1);
    }

    i = recv(sock,recvBuffer,4000,0);

    if( i == -1 ) { 
      break;
    }

    if(recvBuffer[i-1] != '\n') {
      break;
    }
    recvBuffer[i] = '\0';

    printf("Receive from client : %s\n",recvBuffer);

    cbuf = read_response(recvBuffer,totalReceived);
    size = strlen(cbuf);
    totalSent = 0;

    do{
      bytesSent = send(sock,cbuf + totalSent, strlen(cbuf + totalSent),0);
      if(bytesSent == -1) {
        break;
      }
      totalSent += bytesSent;
    }while(totalSent < size);

    printf("Connection closed by client\n");

    close(sock);
  }

  //free(recvBuffer);

  return 0;
}