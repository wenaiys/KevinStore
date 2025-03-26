TARGET = kvstore
SOURCES = kvstore_array.c kvstore.c reactor.c kvstore_rbtree.c ntyco_entry.c
OBJS = $(SOURCES:.c=.o)
CC = gcc
TESTCASE = testcase 
FLAGS = -I ./NtyCo/core/ -L ./NtyCo/ -lntyco -lpthread -ldl
SUBDIR = ./NtyCo/
all:$(SOURCES) $(OBJS) $(TARGET) $(TESTCASE)$(SUBDIR)

$(SUBDIR): ECHO
	make -C $@

ECHO:
	@echo $(SUBDIR)

$(TESTCASE):testcase.c 
	$(CC) -o $@ $^

%.o:%.c
	$(CC) -o $@ -c $^ $(FLAGS)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(FLAGS)

clean:
	rm -rf $(OBJS) $(TARGET) $(TESTCASE)
