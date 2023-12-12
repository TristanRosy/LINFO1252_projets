SRCS := phils.c prod_cons.c read_write.c my_phils.c my_prod_cons.c my_read_write.c spinlocks_test.c
CC := gcc
CFLAGS := -Wall -lpthread my_lock.c my_sem.c

EXECS := $(SRCS:.c=)

all: $(EXECS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Lance le script bash qui enregistres tous les temps dans des fichiers csv,
# puis un script python affiche les graphiques des performances testées.
test: $(EXECS)
	bash experiments.sh
	python3 graphs.py

clean:
	rm -f $(EXECS)
	rm -f *.csv

.PHONY: all test clean
