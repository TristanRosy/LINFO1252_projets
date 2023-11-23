import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv('compilation_times.csv', header=None, names=['threads', 'philosophes', 'producteurs-consommateurs', 'lecteurs-écrivains'])

# On calcule la moyenne et l'écart type pour chaque nombre de threads
grouped_data = data.groupby('threads').agg({'philosophes': ['mean', 'std'],
                                            'producteurs-consommateurs': ['mean', 'std'],
                                            'lecteurs-écrivains': ['mean', 'std']}).reset_index()

threads = grouped_data['threads']
phils_mean, programme1_std = grouped_data['philosophes']['mean'], grouped_data['philosophes']['std']
prod_cons_mean, programme2_std = grouped_data['producteurs-consommateurs']['mean'], grouped_data['producteurs-consommateurs']['std']
read_write_mean, programme3_std = grouped_data['lecteurs-écrivains']['mean'], grouped_data['lecteurs-écrivains']['std']


plt.figure(figsize=(10, 6))

plt.errorbar(threads, phils_mean, yerr=programme1_std, label='philosophes', marker='o')
plt.errorbar(threads, prod_cons_mean, yerr=programme2_std, label='producteurs-consommateurs', marker='o')
plt.errorbar(threads, read_write_mean, yerr=programme3_std, label='lecteurs-écrivains', marker='o')

plt.title('Temps d\'exécution en fonction du nombre de threads')
plt.xlabel('Nombre de Threads')
plt.ylabel('Temps d\'exécution (sec)')
plt.legend()
plt.grid(True)
plt.ylim(bottom=0)  # L'axe des y commence à 0

plt.show()
