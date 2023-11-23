#!/usr/bin/env bash

OUTPUT_FILE="compilation_times.csv"

exec >/dev/null # Désactiver toute sortie sur STDOUT.

for i in 2 4 8 16 32 64; do
  for j in {1..5}; do
    phil_time=""
    prod_cons_time=""
    read_write_time=""
    for k in "phils" "prod_cons" "read_write" ; do
      N=$i
      if [[ "$k" == "prod_cons" || "$k" == "read_write" ]]; then
        N=$((i/2))
      fi
      start_time=$(date +%s.%N)
      ./"$k" $N $N
      end_time=$(date +%s.%N)
      elapsed_time=$(echo "$end_time - $start_time" | bc)

      case "$k" in
        "phils")
          phil_time="$elapsed_time"
          ;;
        "prod_cons")
          prod_cons_time="$elapsed_time"
          ;;
        "read_write")
          read_write_time="$elapsed_time"
          ;;
      esac
    done
    echo "$i,$phil_time,$prod_cons_time,$read_write_time" >> $OUTPUT_FILE
  done
done
