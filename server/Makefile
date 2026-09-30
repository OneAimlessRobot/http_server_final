BINARY= server.exe
INCLUDE= ./Includes .
SOURCES= . ./Sources
RESDIR= ./resources

CURRDIR=echo `pwd`

#CC= clang
#CC= gcc
CC= cc
LDLIBS := -pthread -lz -lssl -lcrypto -lm

DEPFLAGS= -MP -MD

CFLAGS= -Wall -DPROGRAMPATHAUX="$(CURRDIR)" -Wpedantic -Wextra -g $(foreach D, $(INCLUDE), -I$(D)) $(DEPFLAGS) -std=c99 -O3

#CFLAGS= -Wall -DPROGRAMPATHAUX="$(CURRDIR)" -fsanitize=address -Wpedantic   -g $(foreach D, $(INCLUDE), -I$(D)) $(DEPFLAGS) -std=c99 -O3

SOURCEFILES=$(foreach D,$(SOURCES), $(wildcard $(D)/*.c))


RESOURCEFILES=$(wildcard $(RESDIR)/*.o)


OBJECTS=$(patsubst %.c,%.o,$(SOURCEFILES))

ALLMODULES= $(RESOURCEFILES) $(OBJECTS)

DEPFILES= $(patsubst %.c,%.d,$(SOURCEFILES))

all: $(BINARY)
	echo $(LDLIBS)
	echo $(CURRDIR)


$(BINARY): $(OBJECTS)
	$(CC) -g -v  $(CFLAGS) -o  $@ $^ $(RESOURCEFILES)  $(LDLIBS)


%.o:%.c
	$(CC) -g  $(CFLAGS) -c -o $@ $<
	echo $(CFLAGS)

clean:
	rm -rf $(BINARY) $(ALLMODULES) $(DEPFILES)
