SRCS := phils.c prod_cons.c read_write.c
CC := gcc
CFLAGS := -Wall -lpthread

EXECS := $(SRCS:.c=)

all: $(EXECS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

test: $(EXECS)
	bash experiments.sh
	python3 graphs.py

clean:
	rm -f $(EXECS)
	rm -f *.csv

.PHONY: all test clean
