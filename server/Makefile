BINARY= server.exe
INCLUDE= ./Includes .
SOURCES= . ./Sources
RESDIR= ./resources

CURRDIR=echo `pwd`

#CC= clang
CC= gcc
LDLIBS := -pthread -lz -lssl -lcrypto

DEPFLAGS= -MP -MD

CFLAGS= -Wall -DPROGRAMPATHAUX="$(CURRDIR)" -Wpedantic -Werror  -Wextra -g $(foreach D, $(INCLUDE), -I$(D)) $(DEPFLAGS)

#CFLAGS= -Wall -DPROGRAMPATHAUX="$(CURRDIR)" -fsanitize=address -Wpedantic -Werror  -Wextra -g $(foreach D, $(INCLUDE), -I$(D)) $(DEPFLAGS)

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
