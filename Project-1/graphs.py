import matplotlib.pyplot as plt
import pandas as pd

# Lecture du premier fichier CSV (POSIX)
data_posix = pd.read_csv('compilation_times.csv', header=None, names=['threads', 'philosophes', 'producteurs-consommateurs', 'lecteurs-écrivains'])
grouped_data_posix = data_posix.groupby('threads').agg({'philosophes': ['mean', 'std'],
                                                        'producteurs-consommateurs': ['mean', 'std'],
                                                        'lecteurs-écrivains': ['mean', 'std']}).reset_index()

threads_posix = grouped_data_posix['threads']
phils_mean_posix, programme1_std_posix = grouped_data_posix['philosophes']['mean'], grouped_data_posix['philosophes']['std']
prod_cons_mean_posix, programme2_std_posix = grouped_data_posix['producteurs-consommateurs']['mean'], grouped_data_posix['producteurs-consommateurs']['std']
read_write_mean_posix, programme3_std_posix = grouped_data_posix['lecteurs-écrivains']['mean'], grouped_data_posix['lecteurs-écrivains']['std']

# Lecture du deuxième fichier CSV (attente active)
data_active = pd.read_csv('compilation_times_attente_active.csv', header=None, names=['threads', 'philosophes', 'producteurs-consommateurs', 'lecteurs-écrivains'])
grouped_data_active = data_active.groupby('threads').agg({'philosophes': ['mean', 'std'],
                                                          'producteurs-consommateurs': ['mean', 'std'],
                                                          'lecteurs-écrivains': ['mean', 'std']}).reset_index()

threads_active = grouped_data_active['threads']
phils_mean_active, programme1_std_active = grouped_data_active['philosophes']['mean'], grouped_data_active['philosophes']['std']
prod_cons_mean_active, programme2_std_active = grouped_data_active['producteurs-consommateurs']['mean'], grouped_data_active['producteurs-consommateurs']['std']
read_write_mean_active, programme3_std_active = grouped_data_active['lecteurs-écrivains']['mean'], grouped_data_active['lecteurs-écrivains']['std']

# Lecture du troisième fichier CSV ("primitives_attente_active.csv")
data_attente_act = pd.read_csv('primitives_attente_active.csv', header=None, names=['threads', 'philosophes', 'producteurs-consommateurs', 'lecteurs-écrivains'])
grouped_data_attente_act = data_attente_act.groupby('threads').agg({'philosophes': ['mean', 'std'],
                                                                    'producteurs-consommateurs': ['mean', 'std'],
                                                                    'lecteurs-écrivains': ['mean', 'std']}).reset_index()

threads_attente_act = grouped_data_attente_act['threads']
phils_mean_attente_act, programme1_std_attente_act = grouped_data_attente_act['philosophes']['mean'], grouped_data_attente_act['philosophes']['std']
prod_cons_mean_attente_act, programme2_std_attente_act = grouped_data_attente_act['producteurs-consommateurs']['mean'], grouped_data_attente_act['producteurs-consommateurs']['std']
read_write_mean_attente_act, programme3_std_attente_act = grouped_data_attente_act['lecteurs-écrivains']['mean'], grouped_data_attente_act['lecteurs-écrivains']['std']

# Génération des graphiques
fig, axes = plt.subplots(2, 2, figsize=(15, 10))
fig.suptitle('Temps d\'exécution en fonction du nombre de threads')

# Graphiques pour les philosophes
axes[0, 0].errorbar(threads_posix, phils_mean_posix, yerr=programme1_std_posix, label='Philosophes POSIX', marker='o')
axes[0, 0].errorbar(threads_active, phils_mean_active, yerr=programme1_std_active, label='Philosophes Attente Active', marker='o')
axes[0, 0].set_title('Philosophes')
axes[0, 0].set_xlabel('Nombre de Threads')
axes[0, 0].set_ylabel('Temps d\'exécution (sec)')
axes[0, 0].legend()
axes[0, 0].grid(True)
axes[0, 0].set_ylim(bottom=0)

# Graphiques pour les producteurs-consommateurs
axes[0, 1].errorbar(threads_posix, prod_cons_mean_posix, yerr=programme2_std_posix, label='Prod-Cons POSIX', marker='o')
axes[0, 1].errorbar(threads_active, prod_cons_mean_active, yerr=programme2_std_active, label='Prod-Cons Attente Active', marker='o')
axes[0, 1].set_title('Producteurs-Consommateurs')
axes[0, 1].set_xlabel('Nombre de Threads')
axes[0, 1].set_ylabel('Temps d\'exécution (sec)')
axes[0, 1].legend()
axes[0, 1].grid(True)
axes[0, 1].set_ylim(bottom=0)

# Graphiques pour les lecteurs-écrivains
axes[1, 0].errorbar(threads_posix, read_write_mean_posix, yerr=programme3_std_posix, label='Lecteurs-Écrivains POSIX', marker='o')
axes[1, 0].errorbar(threads_active, read_write_mean_active, yerr=programme3_std_active, label='Lecteurs-Écrivains Attente Active', marker='o')
axes[1, 0].set_title('Lecteurs-Écrivains')
axes[1, 0].set_xlabel('Nombre de Threads')
axes[1, 0].set_ylabel('Temps d\'exécution (sec)')
axes[1, 0].legend()
axes[1, 0].grid(True)
axes[1, 0].set_ylim(bottom=0)

# Graphique pour le fichier "primitives_attente_act.csv"
axes[1, 1].errorbar(threads_attente_act, phils_mean_attente_act, yerr=programme1_std_attente_act, label='Test and Set', marker='o')
axes[1, 1].errorbar(threads_attente_act, prod_cons_mean_attente_act, yerr=programme2_std_attente_act, label='Test and Test and Set', marker='o')
axes[1, 1].errorbar(threads_attente_act, read_write_mean_attente_act, yerr=programme3_std_attente_act, label='Back-off Test and Test and Set', marker='o')
axes[1, 1].set_title('Primitives Attente Active')
axes[1, 1].set_xlabel('Nombre de Threads')
axes[1, 1].set_ylabel('Temps d\'exécution (sec)')
axes[1, 1].legend()
axes[1, 1].grid(True)
axes[1, 1].set_ylim(bottom=0)

plt.tight_layout()
plt.show()
