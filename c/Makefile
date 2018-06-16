# Makefile for small projects
# Karel Dolezal, akwky@centrum.cz
#
# for experimental Makefile self-update, use "make depend"
# vim needs ":set noexpandtab"

# what to run during "make run"
MKRUN = server

# what to build during "make all"
MKALL = server client

# compressed file names (zip or tar.gz)
PKGNAME = akwky
PKGTYPE = tar.gz

# all source files
ALLSOURCES = `ls *.c *.h *.cpp 2> /dev/null`

# variables CC, CFLAGS a LDFLAGS (+ LDLIBS ?) for default rules
CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g 
LDLIBS = -lm 

# phony commands
.PHONY: all
.PHONY: clean
.PHONY: zip
.PHONY: tar.gz
.PHONY: run
.PHONY: depend
.PHONY: pack

# default rules, specific rules 
all: $(MKALL)

clean: 
	rm -f *.o $(PKGNAME).zip $(PKGNAME).tar.gz $(MKALL) *.log
pack: $(PKGTYPE)
	wc -L $(ALLSOURCES)
zip:
	zip $(PKGNAME).zip $(ALLSOURCES) Makefile
tar.gz:
	tar -zvcf $(PKGNAME).tar.gz $(ALLSOURCES) Makefile
run: $(MKALL)
	./$(MKRUN)

# auto dependency update (uses head command for compatibility with eva.fit.vutbr.cz)
# ( commands; joined; ) > into_common_output_file
depend:
	mv Makefile Makefile~
	( head -n `sed -n "/^[#]CUT_HERE/=" < Makefile~` < Makefile~;   gcc -MM *.c; ) > Makefile

# target rules
server: server.o common.o tasks.o
client: client.o common.o

# auto generated rules by "make depend"
# Warning: everything will be deleted starting from the token below
#CUT_HERE
client.o: client.c common.h
common.o: common.c common.h
server.o: server.c common.h tasks.h
tasks.o: tasks.c tasks.h
