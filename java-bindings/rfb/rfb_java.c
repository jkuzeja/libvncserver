#include <assert.h>
#include <string.h>
#include <rfb/rfb.h>
#include <rfb/rfbclient.h>

#include "rfb_java.h"

static inline int alloc_nr(alloc) {
	return (alloc + 4) * 3 / 2;
}

#define ALLOC_GROW(x, nr, alloc) \
        do { \
                if ((nr) > alloc) { \
                        if (alloc_nr(alloc) < (nr)) \
                                alloc = (nr); \
                        else \
                                alloc = alloc_nr(alloc); \
                        x = realloc((x), alloc * sizeof(*(x))); \
                } \
        } while(0)

/* resource management */

struct resource_pool {
	void **pool;
	int count, used, alloc;
};

static void *get_resource(struct resource_pool *pool, int resource)
{
	if(resource >= pool->count || resource < 0 || !pool->pool[resource])
		return NULL;
	return pool->pool[resource];
}

static int add_resource(struct resource_pool *pool, void *resource)
{
	int number;

	if (pool->used >= pool->alloc) {
		ALLOC_GROW(pool->pool, pool->count + 1, pool->alloc);
		number = pool->count++;
	}
	else
		for (number = 0; pool->pool[number]; number++)
			; /* do nothing */
	pool->pool[number] = resource;
fprintf(stderr, "resource %d is %p\n", number, resource);
	pool->used++;
	return number;
}

static void unset_resource(struct resource_pool *pool, int resource)
{
	if (pool->pool[resource]) {
		pool->pool[resource] = NULL;
		pool->used--;
	}
}

/* this is a VNC connection */

struct server {
	rfbScreenInfo *server;
	rfbClient *client;
};

struct resource_pool server_pool, client_pool;

static rfbBool malloc_frame_buffer(rfbClient* cl)
{
	struct server *server = (struct server *)cl->clientData;

	if (!server->server) {
		int w = cl->width, h = cl->height;
		
		server->client->frameBuffer = malloc(w * 4 * h);
		
		server->server = rfbGetScreen(NULL, NULL, w, h, 8, 3, 4);
		server->server->screenData = server;
		server->server->port = -1;
		server->server->frameBuffer = server->client->frameBuffer;
		rfbInitServer(server->server);
	} else {
		int w = cl->width, h = cl->height;
		char *frameBuffer =
			realloc(server->server->frameBuffer, w * 4 * h);

		if (!frameBuffer)
			return FALSE;
		server->server->frameBuffer = frameBuffer;
		/* TODO: copy what we have */
	}
	return TRUE;
}

static void got_frame_buffer(rfbClient* cl,int x,int y,int w,int h)
{
	struct server *server = (struct server *)cl->clientData;
	
	assert(server->server);
	
	if(server->server)
		rfbMarkRectAsModified(server->server, x, y, x + w, y + h);
}

/* init/shutdown functions */

resource_t connectToServer(const char* remote_server, int server_port)
{
	struct server *server = calloc(sizeof(struct server), 1);
	int dummy = 0;

	if(!server)
		return -1;

fprintf(stderr, "server: %p\n", server);
	server->client = rfbGetClient(8,3,4);
fprintf(stderr, "got client: %p\n", server->client);
	server->client->clientData = (void *)server;
	server->client->GotFrameBufferUpdate = got_frame_buffer;
	server->client->MallocFrameBuffer = malloc_frame_buffer;
	server->client->serverHost = strdup(remote_server);
	server->client->serverPort = server_port;
	server->client->appData.encodingsString = "raw hextile";
fprintf(stderr, "Hello!\n");
	if(!rfbInitClient(server->client, &dummy, NULL)) {
fprintf(stderr, "Oh, no!");
		server->client = NULL;
		return -1;
	}
	int i = add_resource(&server_pool, server);
fprintf(stderr, "resource: %d, address: %p\n", i, get_resource(&server_pool, i));
struct server *s = (struct server *)get_resource(&server_pool, i);
fprintf(stderr, "s: %p, client: %p\n", s, s->client);
	return i;
}

void closevnc(resource_t resource)
{
	struct server *server = get_resource(&server_pool, resource);

	if (!server)
		return;

	if (server->server)
		rfbScreenCleanup(server->server);

	assert(server->client);

	rfbClientCleanup(server->client);

	unset_resource(&server_pool, resource);
}

/* process messages from the remote server */

void processServerEvents(resource_t resource)
{
	struct server *server = get_resource(&server_pool, resource);
	int number;

fprintf(stderr, "resource: %d, %p", resource, server->client);
	number = WaitForMessage(server->client, 500);

	if (number <= 0)
		return;
	HandleRFBServerMessage(server->client);
}

client_t addClient(resource_t server_handle)
{
	struct server *server = get_resource(&server_pool, server_handle);
	rfbClientPtr client = rfbNewClient(server->server, -1);

	client->readBufferAlloc = 256;
	client->readBuffer = malloc(client->readBufferAlloc);
	if (!client->readBuffer) {
		rfbClientConnectionGone(client);
		return -1;
	}
	client->writeBufferAlloc = 16384;
	client->writeBuffer = malloc(client->writeBufferAlloc);
	if (!client->writeBuffer) {
		free(client->readBuffer);
		rfbClientConnectionGone(client);
		return -1;
	}

	return add_resource(&client_pool, client);
}

int processClientMessage(client_t client_handle, signed char buffer[], int len)
{
	rfbClientPtr client = get_resource(&client_pool, client_handle);
	if (client->readBufferLen + len < client->readBufferAlloc) {
		int alloc = client->readBufferLen + len;
		char *buf = realloc(client->readBuffer, alloc);

		if (!buf)
			return -1;
		client->readBuffer = buf;
		client->readBufferAlloc = alloc;
	}
	memcpy(client->readBuffer + client->readBufferLen, buffer, len);
	client->readBufferLen += len;

	rfbProcessClientMessage(client);
	return 0;
}

int getServerResponse(client_t client_handle, signed char buffer[], int len)
{
	rfbClientPtr client = get_resource(&client_pool, client_handle);

	if (client->writeBufferLen < len)
		len = client->writeBufferLen;

	memcpy(buffer, client->writeBuffer, len);
	client->writeBufferLen -= len;

	if (client->writeBufferLen)
		memmove(client->writeBuffer, client->writeBuffer + len,
			client->writeBufferLen);

	return len;
}

void closeClient(client_t client_handle)
{
	rfbClientPtr client = get_resource(&client_pool, client_handle);

	rfbClientConnectionGone(client);
	unset_resource(&client_pool, client_handle);
}
