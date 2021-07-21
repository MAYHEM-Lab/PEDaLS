/**
 * sample program showing PEDaLS linked-list function calls
 *
 * this program initializes a PEDaLS linked-list with one extra pointer,
 * then it performs four update operations,
 * and finally prints four versions of the linked-list
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "LinkedList.h"
#include "DataItem.h"
#include "CSPOTHelpers.h"

#include "woofc.h"
#include "woofc-host.h"

int main(int argc, char *argv[]){

	DI di;
	int num_of_extra_links; // number of extra links for node copy method
	int num_ops; // number of update operations
	unsigned long idx;
	VS vs;

	num_of_extra_links = 1;
	num_ops = 4;
	WooFInit(); // cspot specific initialization
	LL_init(num_of_extra_links, num_ops, 2 * num_ops, num_ops); // initialize linked-list

	di.val = 7; LL_insert(di, VS_invalid()); // insert 7
	di.val = 2; LL_insert(di, VS_invalid()); // insert 2
	di.val = 5; LL_insert(di, VS_invalid()); // insert 5
	di.val = 2; LL_delete(di, VS_invalid()); // delete 2

	vs.nodeID = 'A';
	for(idx = 1; idx <= num_ops; ++idx){
		vs.counter = idx;
		LL_print(vs);
	}

	return 0;

}
