# agextspot-v2


AGEXTSPOT-V2

	Agextspot è un software di rilevazione automatica di Gamma Ray Burst in un file FITS che 
	contiene la lista di fotoni ad alta energia di una certa regione di cielo.

	La versione 2 è stata migliorata ed adattata ad un contesto di short-term analysis in cui
	si analizzano mappe con tempo di esposizione da 2 a 100 secondi. 

	La rilevazione viene effettuata attraverso il seguente algoritmo:
		- conversione del file FITS in immagine opencv
		- stretching non lineare, smoothing gaussiano, estrazione dei blobs
		- calcolo delle features dei blobs estratti (numero di fotoni,indice di vicinanza fotoni)
		- utilizzo di un Naive Bayes Classifier per classificare i blob come SOURCE
		  o BACKGROUND.

	Documentazione: https://docs.google.com/document/d/1jDVoPD5emhgM4K3pjwY5XNF0hRnnHiAWVdoTvMgOEwk/edit#heading=h.568bs0t6yg27

	Repo: https://github.com/Leofaber/agextspot-v2



UTILIZZO

	./nome_eseguibile     path_al_file_fits     nome_file_di_log     soglia_di_classificazione

	Esempio di utilizzo:

	./Agextspot ../080514B.cts ../risultati.txt 94.86

OUTPUT

	classificazione,[L,B],probabilità che il blob sia un GRB

	Esempio di output:

	------------------------------------------------------
	GRB DETECTION OF../../080514B.cts - 04-09-2017 16:09:51

	BG,[36.953536,-19.454948],0.734106%
        BG,[40.026007,-32.367002],0.734106%
	BG,[68.344477,-34.964095],16.274435%
	SOURCE,[55.503383,-36.232905],100.000000%
	BG,[77.751018,-48.750292],0.734106%
	BG,[64.093617,-57.440214],1.441523%

	

SOGLIE DI CLASSIFICAZIONE

	Sono state individuate due tipi di soglia: una che minimizza la FPR, una che massimizza l’accuratezza.

	1) SOGLIA DI MINIMIZZAZIONE DELLA FALSE POSITIVE RATE: ** 99,99 **

		FPR	TPR	TNR	FNR	ACCURACY  K-Cohen
		0,863   50,27   99,13   49,72   86,72	  58	

	2) SOGLIA DI MASSIMIZZAZIONE DELL’ACCURATEZZA: **  94,86  **

		FPR	 TPR	 TNR	FNR	ACCURACY  K-Cohen  
		3,3976	 69,56	 96,88	30,43	89,94	  71	   

LEGENDA

	FPR: False Positive Rate (False Alarms)
	TPR: True Positive Rate
	TNR: True Negative Rate
	FNR: False Negative Rate (Miss rate)

	Per ulteriori informazioni riguardo ad i valori di performance (formule, training e test set,
	significato etcetc) si rimanda alla documentazione.
