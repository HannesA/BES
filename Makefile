## @file Makefile
## BES - SS2017 
## Ringbuffer
## Bsp 3
##
## @author Hannes Aurednik <ic15b004@technikum-wien.at>
## @author Eva Dorottya Gergely <ic16b064@technikum-wien.at>
## @author Daniel Scheidl <ic16b073@technikum-wien.at>
## @author Raphael Szabo <ic16b062@technikum-wien.at>
##
## @date 2017/06/02
## 
## @version 0.2
##
##
##-------------------------------------------------------------------------------------------------
CC = gcc52
CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -fno-common -g -O3 -std=gnu11
CP = cp
CD = cd
MV = mv
OBJECTS = sharedfile.o
DOXYGEN = doxygen
RM = rm
GREP=grep
EXCLUDE_PATTERN=footrulewidth
##------------------------------------------------------------------------------------------------


%.o: %.c %.h
	$(CC) $(CFLAGS) -c $^
##------------------------------------------------------------------------------------------------

all: sender empfaenger

sender: sender.o sharedfile.o
	$(CC) $(CFLAGS) -o sender sender.o sharedfile.o -lsem182

empfaenger: empfaenger.o sharedfile.o
	$(CC) $(CFLAGS) -o empfaenger empfaenger.o sharedfile.o -lsem182

empfaenger.o: empfaenger.c sharedfile.h
	$(CC) $(CFLAGS) -c empfaenger.c

sender.o: sender.c sharedfile.h
	$(CC) $(CFLAGS) -c sender.c

sharedfile.o: sharedfile.c sharedfile.h
	$(CC) $(CFLAGS) -c sharedfile.c	

	
.PHONY: clean
clean: 
	$(RM) -f *.o sender empfaenger

.PHONY: distclean
distclean: clean
	$(RM) -rf doc

.PHONY: doc
doc: html pdf

html:
	$(DOXYGEN) Doxyfile

pdf: html
	$(CD) doc/pdf && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) -f *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile && \
	$(MV) refman.save refman.pdf
