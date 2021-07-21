#ifndef CLIENT_H
#define CLIENT_H

#include "Config.h"
#include "Options.h"

#define DS_DIRNAME_SIZE 50
#define CLIENT_DIRNAME_SIZE 50
#define DIRNAME_SIZE 128
#define CLIENT_STR_SIZE 500

struct Client{
	char local_ip[IP_ADDR_SIZE]; /* local IP address */
	char client_dirname[CLIENT_DIRNAME_SIZE]; /* client directory, e.g. client */
	char local_dir[DIRNAME_SIZE]; /* parent of client directory, e.g. /home/centos/PDS */
	char remote_ip[IP_ADDR_SIZE]; /* IP address of the machine at the other end of comm. */
	char ds_dirname[DS_DIRNAME_SIZE]; /* data strcuture directory, e.g. bst */
	char remote_dir[DIRNAME_SIZE]; /* parent of data structure directory, e.g. /home/centos/PDS */
};

typedef struct Client CLIENT;

CLIENT *CLIENT_create(
	char *local_ip, 
	char *client_dirname, 
	char *local_dir,
	char *remote_ip,
	char *ds_dirname,
	char *remote_dir
	);
char *CLIENT_get_remote_sequencer_woof_name(CLIENT c);
char *CLIENT_get_remote_woof_name(CLIENT c, char *woof_name);
char *CLIENT_get_local_woof_name(CLIENT c, char *woof_name);
char *CLIENT_get_sequencer_woof_name(CLIENT c);
char *CLIENT_get_sequencer_completion_woof_name(CLIENT c);
char *CLIENT_str(CLIENT c);

#endif
