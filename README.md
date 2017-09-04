# agextspot-v2


AGEXTSPOT-V2

	La versione 2 è stata adattata ad immagini con tempo di esposizione 2s -> 100s.



UTILIZZO

	./nome_eseguibile     path_al_file_fits     nome_file_di_log     soglia_di_classificazione



OUTPUT

	classificazione,[L,B],probabilità che il blob si un GRB

	Esempio di output:

	------------------------------------------------------
	GRB DETECTION OF../../es1.cts - 02-09-2017 07:23:51

	BG,[83,77],0.075253%
	BG,[74,52],0.075253%
	BG,[27,47],88.068834%
	SOURCE,[48,46],100.000000%
	BG,[19,17],0.075253%
	BG,[39,3],0.058119%
	


SOGLIE DI CLASSIFICAZIONE

	Sono state individuate due tipi di soglia: una che minimizza la FPR, una che massimizza l’accuratezza.

	1) SOGLIA DI MINIMIZZAZIONE DELLA FALSE POSITIVE RATE (99,99)

		FPR	TPR	  THRESHOLD
		1,0407  50,8804   99,99

	2) SOGLIA DI MASSIMIZZAZIONE DELL’ACCURATEZZA (94,86)

		FPR	ACCURACY   THRESHOLD
		3,3976  90,06	   94,86



