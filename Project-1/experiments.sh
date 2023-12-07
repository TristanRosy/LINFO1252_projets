#!/usr/bin/env bash

# Suppression du contenu précédent des fichiers
> primitives_attente_active.csv
> compilation_times.csv
> compilation_times_attente_active.csv

exec > /dev/null # Redirige la sortie standard (stdout) vers /dev/null.

# Exécution du programme pour les primitives de verrouillage
for i in 1 2 4 8 16 32 64; do
  # Répéter chaque nombre de threads cinq fois
  for repeat in {1..5}; do
    echo -n "$i," >> primitives_attente_active.csv

    for lock_type in "tas" "tatas" "botatas"; do
      NUM_SECTIONS=$((6400 / i))

      start_time=$(date +%s.%N)
      elapsed_time=$(./prog $i $NUM_SECTIONS $lock_type)
      end_time=$(date +%s.%N)
      elapsed_time=$(echo "$end_time - $start_time" | bc)

      # Ajouter le temps après le nombre de threads
      echo -n "$elapsed_time," >> primitives_attente_active.csv
    done
    # Aller à la ligne pour la prochaine série de mesures avec le même nombre de threads
    echo "" >> primitives_attente_active.csv
  done
done


# Exécution des programmes phil, prod_cons et read_write
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

    echo "$i,$phil_time,$prod_cons_time,$read_write_time" >> compilation_times.csv

    phil_time=""
    prod_cons_time=""
    read_write_time=""

    for k in "my_phils" "my_prod_cons" "my_read_write"; do
      N=$i
      if [[ "$k" == "my_prod_cons" || "$k" == "my_read_write" ]]; then
        N=$((i/2))
      fi

      start_time=$(date +%s.%N)
      ./"$k" $N $N
      end_time=$(date +%s.%N)
      elapsed_time=$(echo "$end_time - $start_time" | bc)

      case "$k" in
        "my_phils")
          phil_time="$elapsed_time"
          ;;
        "my_prod_cons")
          prod_cons_time="$elapsed_time"
          ;;
        "my_read_write")
          read_write_time="$elapsed_time"
          ;;
      esac
    done

    echo "$i,$phil_time,$prod_cons_time,$read_write_time" >> compilation_times_attente_active.csv
  done
done

