# agextspot-v2

La versione 2 è stata adattata ad immagini con tempo di esposizione 2s -> 100s.



Utilizzo: ./nome_eseguibile path_al_file_fits nome_file_di_log soglia_di_classificazione

Output: CLASSIFICAZIONE,[L,B],PROBABILITA' CHE IL BLOB SIA UN GRB

Esempio di output:
------------------------------------------------------
GRB DETECTION OF../../es1.cts - 02-09-2017 07:23:51

BG,[83,77],0.075253%
BG,[74,52],0.075253%
BG,[27,47],88.068834%
SOURCE,[48,46],100.000000%
BG,[19,17],0.075253%
BG,[39,3],0.058119%



TODO: 

	- Decidere la combinazione di attributi discriminatori più adatta. Attualmente il bayesiano utilizza A-P-PM (Area del blob in termini di numero di pixels, numero di fotoni ad alta energia all'interno del blob, media dei valori di grigio del blob).

	- Decidere la soglia di classificazione più adatta

