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

	./nome_eseguibile     path_al_file_cts     path_al_file_exp     nome_file_di_log     soglia_di_classificazione     (Opzionale)     minTreshold (default value 0)     maxTreshold (default value 100)

	Esempio di utilizzo:

	./Agextspot ../080514B.cts  ../080514B.exp ../LOG.txt 94.86 (Opzionale) 5 75

OUTPUT
	
	Il software scrive un file di log che elenca tutte le classificazioni effettuate (backgrounds e sources) con il seguente formato:	

	indice,  etichetta_di_classificazione,  L_GAL,   B_GAL,   confidenza_(%),   dataUTC,   dataTT, soglia_di_classificazione,  nome_file_di_log,  exp-ratio,   nBad,   totCount,   greyLevel/totCount     

	
	Esempio di output:

	File: LOG_SCAN100_431032381.0_431032481.0_23782.txt
	------------------------------------------------------
	1, BG, 36.953536, -19.454948, 0.734106, 2017-08-28T19:14:08, 137843757,95,SCAN100_431032381.0_431032481.0_23782,100, 0, 1681, 9 
	2, BG, 40.026007, -32.367002, 0.734106, 2017-08-28T19:14:08, 137843757, 95 SCAN100_431032381.0_431032481.0_23782,100, 0, 1681, 10
	3, BG, 68.344477, -34.964095, 16.274435, 2017-08-28T19:14:08, 137843757, 95 SCAN100_431032381.0_431032481.0_23782,-1, -1, -1, -1
	4, SOURCE, 55.503383, -36.232905, 100.000000, 2017-08-28T19:14:08, 137843757, 95 SCAN100_431032381.0_431032481.0_23782, 100, 0, 1681, 10
	5, BG, 77.751018, -48.750292, 0.734106, 2017-08-28T19:14:08, 137843757, 95 SCAN100_431032381.0_431032481.0_23782,-1, -1, -1, -1
	6, BG, 64.093617, -57.440214, 1.441523, 2017-08-28T19:14:08, 137843757, 95 SCAN100_431032381.0_431032481.0_23782,100, 0, 1681, 9 

	Se non vengono effettuate classificazioni:
	File: LOG_SCAN100_431032381.0_431032481.0_23782.txt
	-----------------------------------------------------
	NO_BLOBS, 2017-08-28T19:14:08, 137843757, SCAN100_431032381.0_431032481.0_23782, 95



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
