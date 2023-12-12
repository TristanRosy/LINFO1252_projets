#!/usr/bin/time

# Suppression du contenu précédent des fichiers
> primitives_attente_active.csv
> compilation_times.csv
> compilation_times_attente_active.csv

exec > /dev/null # Redirige la sortie standard (stdout) vers /dev/null.

for i in 2 4 8 16 32 64; do
  for j in {1..5}; do
    phil_time=""
    prod_cons_time=""
    read_write_time=""

    for k in "phils" "prod_cons" "read_write"; do
      N=$i
      if [[ "$k" == "prod_cons" || "$k" == "read_write" ]]; then
        N=$((i/2))
      fi

      elapsed_time=$(/usr/bin/time -f "%e" -q ./"$k" $N $N 2>&1)

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

    echo "$i,$phil_time,$prod_cons_time,$read_write_time" >> compilation_times.csv

    my_phil_time=""
    my_prod_cons_time=""
    my_read_write_time=""

    for k in "my_phils" "my_prod_cons" "my_read_write"; do
      N=$i
      if [[ "$k" == "my_prod_cons" || "$k" == "my_read_write" ]]; then
        N=$((i/2))
      fi

      elapsed_time=$(/usr/bin/time -f "%e" -q ./"$k" $N $N 2>&1)

      case "$k" in
        "my_phils")
          my_phil_time="$elapsed_time"
          ;;
        "my_prod_cons")
          my_prod_cons_time="$elapsed_time"
          ;;
        "my_read_write")
          my_read_write_time="$elapsed_time"
          ;;
      esac
    done
    echo "$i,$my_phil_time,$my_prod_cons_time,$my_read_write_time" >> compilation_times_attente_active.csv

    tas=""
    tatas=""
    botatas=""

    NUM_SECTIONS=$((6400 / i))
    for k in {1..3}; do
      elapsed_time=$(/usr/bin/time -f "%e" -q ./spinlocks_test $i $NUM_SECTIONS $k 2>&1)
      case "$k" in
        "1")
          tas="$elapsed_time"
          ;;
        "2")
          tatas="$elapsed_time"
          ;;
        "3")
          botatas="$elapsed_time"
          ;;
      esac
    done
    echo "$i,$tas,$tatas,$botatas" >> primitives_attente_active.csv

  done
done
