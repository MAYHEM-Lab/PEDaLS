#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Client.h"

CLIENT *CLIENT_create(
	char *local_ip, 
	char *client_dirname, 
	char *local_dir,
	char *remote_ip,
	char *ds_dirname,
	char *remote_dir
){
	CLIENT *c;

	c = (CLIENT *)malloc(sizeof(CLIENT));
	strcpy(c->local_ip, local_ip);
	strcpy(c->client_dirname, client_dirname);
	strcpy(c->local_dir, local_dir);
	strcpy(c->remote_ip, remote_ip);
	strcpy(c->ds_dirname, ds_dirname);
	strcpy(c->remote_dir, remote_dir);

	return c;
}

char *sequencer_woof_name_helper(CLIENT c, char *woof_name, bool remote){

	char *retval;

	retval = (char *)malloc(MAX_SIZE_WOOF_NAME * sizeof(char));
	memset(retval, 0, MAX_SIZE_WOOF_NAME * sizeof(char));

	strcpy(retval, "woof://");
	(remote == true) ? strcat(retval, c.remote_ip) : strcat(retval, c.local_ip);
	(remote == true) ? strcat(retval, c.remote_dir) : strcat(retval, c.local_dir);
	strcat(retval, "/");
	(remote == true) ? strcat(retval, c.ds_dirname) : strcat(retval, c.client_dirname);
	strcat(retval, "/cspot/");
	strcat(retval, woof_name);

	return retval;
}

char *CLIENT_get_remote_sequencer_woof_name(CLIENT c){
	return sequencer_woof_name_helper(c, REMOTE_SEQUENCER_WOOF_NAME, true);
}

char *CLIENT_get_remote_woof_name(CLIENT c, char *woof_name){
	return sequencer_woof_name_helper(c, woof_name, true);
}

char *CLIENT_get_local_woof_name(CLIENT c, char *woof_name){
	return sequencer_woof_name_helper(c, woof_name, false);
}

char *CLIENT_get_sequencer_woof_name(CLIENT c){
	return sequencer_woof_name_helper(c, SEQUENCER_WOOF_NAME, false);
}

char *CLIENT_get_sequencer_completion_woof_name(CLIENT c){
	return sequencer_woof_name_helper(c, SEQUENCER_COMPLETION_WOOF_NAME, false);
}

char *CLIENT_str(CLIENT c){
	char *retval;
	int retsize;
	
	retsize = CLIENT_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));
	sprintf(retval, "local:(%s|%s|%s) remote:(%s|%s|%s) ", c.local_ip, c.client_dirname, c.local_dir, c.remote_ip, c.ds_dirname, c.remote_dir);

	return retval;
}
