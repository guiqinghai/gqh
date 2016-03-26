CC=gcc
TG=test_threadpool

SRCS=$(wildcard *.c */*.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

$(TG):$(OBJS)
	$(CC) -o $@ $^ -lpthread
clean:
	rm $(OBJS) $(TG)
