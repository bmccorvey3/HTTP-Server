#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
 
#define DEFAULT_HTTP_PORT 80
#define BUF 100000
#define ERROR -1
 
typedef struct url_s
{
  unsigned short usPort;   // in host byte order
  char          *szServer; // allocated by parse_url(), must be freed by the caller
  char          *szFile;   // allocated by parse_url(), must be freed by the caller
} url_t;
 
 
url_t parse_url(const char *szURL)
{
  url_t url;
  memset(&url, 0, sizeof(url));
 
  unsigned int urllen = strlen(szURL) + 1;
  url.szServer = (char*) malloc(urllen * sizeof(char));
  assert(NULL != url.szServer);
  url.szFile = (char*) malloc(urllen * sizeof(char));
  assert(NULL != url.szFile);
 
  char server[urllen];
  int result = sscanf(szURL, "http://%[^/]/%s", server, url.szFile);
  if (EOF == result)
  {
    fprintf(stderr, "Failed to parse URL: %s\n", strerror(errno));
    exit(1);
  }
  else if (1 == result)
  {
    url.szFile[0] = '\0';
  }
  else if (result < 1)
  {
    fprintf(stderr, "Error: %s is not a valid HTTP request\n", szURL);
    exit(1);
  }
 
  result = sscanf(server, "%[^:]:%hu", url.szServer, &url.usPort);
  if (EOF == result)
  {
    fprintf(stderr, "Failed to parse URL: %s\n", strerror(errno));
    exit(1);
  }
  else if (1 == result)
  {
    url.usPort = DEFAULT_HTTP_PORT;
  }
  else if (result < 1)
  {
    fprintf(stderr, "Error: %s is not a valid HTTP request\n", szURL);
    exit(1);
  }
 
  assert(NULL != url.szServer);
  assert(NULL != url.szFile);
  assert(url.usPort > 0);
  return url;
}
 
 
int main(int argc, char **argv)
{
  if (argc < 2)
  {
    fprintf(stderr,
            "Usage: http_client URL\n");
    exit(1);
  }
 
  // Parse the URL
  url_t url = parse_url(argv[1]);
  fprintf(stderr, "Server: %s:%hu\nFile: /%s\n",
          url.szServer, url.usPort, url.szFile);
 
  /*Create a buffer on which to put the receiving message
  from the server*/
  char buffer[BUF];
  char output[BUF];
  char message[BUF];
  int len = 0, bytes = 0;
  int sockfd;
 
  /*Create a TCP socket*/
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
    perror("socket error client");
    exit(-1);
  }
 
  struct sockaddr_in addr;
  struct hostent *server;
  server = gethostbyname(url.szServer);
  if(server == NULL) {
    perror("gethostbyname failed");
    exit(-1);
  }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(url.usPort);
  bcopy((char *) server -> h_addr, (char *)&addr.sin_addr.s_addr, server -> h_length);
 
  if(connect(sockfd, (struct sockaddr* ) &addr, sizeof(addr)) == ERROR) {
    perror("connect error");
    exit(-1);
  }
 
  sprintf(buffer, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", url.szFile, url.szServer);
  fprintf(stderr,"Get request:\n%s\n", buffer);
 
  send(sockfd, buffer, strlen(buffer), 0);
  len = recv(sockfd, message, BUF, 0);
  bytes += len;
  while(len>0) {
    message[len] = '\0';
    strcat(output,message);
    len = recv(sockfd, message, BUF, 0);
    bytes += len;
  }
    message[bytes] = '\0';
  //printf("Size of output in bytes: %d\n", bytes);
 
 
 
  const char *manipulate;
  manipulate = strstr(output, "\r\n\r\n"); //CHANGE BACK TO \r\n\r\n
  manipulate = manipulate + 4;
  //printf("%s",manipulate);
  char body[BUF];
  strcpy(body,manipulate);
 
  //printf("Length of body : %d\n", strlen(body) );
 
 // const char *token;
 // token = strtok(output, "\r\n");
  int count = strlen(output) - strlen(manipulate);
  int i = 0;
  while(count > 2) { //>2 due to extra \r\n. don't want that to appear
    fprintf(stderr, "%c", output[i]);
    i++;
    count--;
    //token = strtok(NULL, "\r\n");
  }
  fprintf(stderr,"\r\n\r\n"); 
 
  body[bytes] = '\0';
 //printf("%s\n", body);
 // printf("Size of body is: %d\n",sizeof(body));

 fwrite(&body,1,bytes,stdout);
 
 
 
  /*char *header, *body;
  char num = output[234];
    char num1 = output[235];
  char num2 = output[236];
  char num3 = output[237];
    char num4 = output[238];
 
  char num5 = output[239];
  char num6 = output[240];
 
 
  printf("Char is: %c\n", num);
    printf("Char is: %c\n", num1);
      printf("Char is: %c\n", num2);
        printf("Char is: %c\n", num3);
                printf("Char is: %c\n", num4);
        printf("Char is: %c\n", num5);
        printf("Char is: %c\n", num6);
 
 
 
  printf("%s\n", output);
  header = strtok(output, "\r\n\r\n\r\n");
  body = strtok(NULL, "\r\n\r\n\r\n");
 
 
 // sscanf(output, "%s\r\n\n\r\n\n%s", header, body);
 
  printf("Header is: %s\n", header);
    printf("Body is: %s\n", body); */
 
 
  close(sockfd);
 
  free(url.szServer);
  free(url.szFile);
 
  return 0;
}