SUBDIRS=auxiliary-structures data-structures helpers bst linked-list im-bst im-linked-list impds-bst impds-linked-list bst-example linked-list-example

.PHONY: all clean

all clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f makefile $@; \
	done
