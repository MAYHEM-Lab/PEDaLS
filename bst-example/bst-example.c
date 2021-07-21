/**
 * sample program showing PEDaLS bst function calls
 *
 * this program initializes a PEDaLS bst with one extra pointer,
 * then it performs four update operations,
 * and finally prints the preorder traversal of the four versions
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "BST.h"
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
	BST_init(num_of_extra_links, num_ops, 3 * num_ops, num_ops); // initialize bst

	di.val = 7; BST_insert(di, VS_invalid()); // insert 7
	di.val = 2; BST_insert(di, VS_invalid()); // insert 2
	di.val = 5; BST_insert(di, VS_invalid()); // insert 5
	di.val = 2; BST_delete(di, VS_invalid()); // delete 2

	vs.nodeID = 'A';
	for(idx = 1; idx <= num_ops; ++idx){
		vs.counter = idx;
		BST_preorder(vs);
	}

	return 0;

}
