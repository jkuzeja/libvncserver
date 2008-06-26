#ifndef RFB_JAVA_H
#define RFB_JAVA_H

#ifdef SWIG
%module rfb
%include <arrays_java.i>
%{

/* this is sort of a "file descriptor" for the proxy */
typedef int resource_t;

/* this is sort of a "file descriptor" for the clients */
typedef int client_t;

%}

#endif // SWIG

typedef int resource_t;
typedef int client_t;

/* init/shutdown */

resource_t connectToServer(const char* server, int serverPort);

void processServerEvents(resource_t server);

client_t addClient(resource_t server);
int processClientMessage(client_t client, signed char buffer[], int len);
int getServerResponse(client_t client, signed char buffer[], int len);
void closeClient(client_t client);

#endif
