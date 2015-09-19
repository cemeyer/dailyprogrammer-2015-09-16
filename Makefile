WFLAGS=	-Wall -Wextra -Wno-missing-field-initializers

S=	../../c/libakdtree

granmda: main.c $S/akdtree.h $S/akdtree.c
	cc $(WFLAGS) -I$S -g -O3 -std=gnu11 -o $@ $< $S/akdtree.c
