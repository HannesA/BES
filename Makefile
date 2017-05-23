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
##
## @version 0.1
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

all: sender empfaenger sharedfile

sender: $(OBJECTS)
	$(CC) -o sender $(OBJECTS)

empfaenger: $(OBJECTS)
	$(CC) -o empfaenger $(OBJECTS)  

sharedfile: $(OBJECTS)
	$(CC) -o sharedfile $(OBJECTS)  
	
.PHONY: clean
clean: 
	$(RM) -f *.o popentest test-pipe

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
