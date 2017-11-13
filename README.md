# AG_extspot-v2 1.1.1

Agextspot è un software di estrazione automatica di possibili blob candidati ad ulteriori analisi al fine di identificare automaticamente eventi astrofisici ad alta energia.

La versione 2 è stata migliorata ed adattata ad un contesto di short-term analysis in cui si analizzano mappe con tempo di esposizione da 2 a 100 secondi. 

La rilevazione viene effettuata attraverso il seguente algoritmo:
* conversione del file FITS in immagine opencv
* stretching non lineare, smoothing gaussiano, estrazione dei blobs
* calcolo delle features dei blobs estratti (numero di fotoni,indice di vicinanza fotoni)
* utilizzo di un Naive Bayes Classifier per classificare i blobs come SOURCE o BACKGROUND.
		  
Può essere anche calcolata la valutazione exp-ratio per ogni blob trovato. Per ulteriori informazioni sulla valutazione exp-ratio si rimanda ad https://github.com/Leofaber/ExpRatioEvaluator

### Documentazione: 
	
	https://docs.google.com/document/d/1jDVoPD5emhgM4K3pjwY5XNF0hRnnHiAWVdoTvMgOEwk/edit#heading=h.568bs0t6yg27

### Utilizzo

	./bin/AG_extspot outputLogName, imageCtsPath, classificationThreshold, imageExpPath, isExpMapNormalized, createExpNormalizedMap, createExpRatioMap, minTreshold, maxTreshold, squareSize
	

### Input

	outputLogName: il file di output (è possibile specificare anche il percorso) 
	
	imageCtsPath: il percorso alla mappa dei conteggi (.cts o .cts.gz) 

	classificationThreshold: la soglia di classificazione, se la percentuale bayesiana di classificazione supera la soglia, il blob i-esimo viene etichettato come GRB

	imageExpPath: il percorso alla mappa di esposizone (.exp o .exp.gz). Se il valore è "None" la valutazione exp ratio non verrà effettuata: nel file di output verrà riportato il valore -1.
	
	isExpMapNormalized : se il valore è "false", si afferma che la mappa exp in input (imageExpPath) NON è normalizzata. Il software provvederà a normalizzarla.
	
	createExpNormalizedMap : se il valore è true verrà scritta su file la mappa normalizzata. ( yes/no ) 

	createExpRatioMap: se il valore è "true", viene creata una mappa (FITS) nella quale il valore di ogni pixel rappresenta il valore dell'ExpRatioEvaluator calcolato sul medesimo pixel. Se il valore è "false" non viene creata alcuna mappa.

	minThreshold: la soglia minima sotto la quale il pixel è considerato "bad" per il calcolo dell'exp-ratio 
	
	maxThreshold: la soglia minima sopra la quale il pixel è considerato "bad" per il calcolo dell'exp-ratio
	
	squareSize: la dimensione in pixel del lato della regione (quadrato) su cui si calcola l'exp-ratio
	
### Esempio di utilizzo

	./bin/AG_extspot log.txt MAPPE_PER_TEST/0000000010_001_GRBTESTMAP.cts 95 MAPPE_PER_TEST/MAP1000s_45l_30b.exp yes no no 110 150 10
 	

### Output

	1) L'outuput di ExpRatioEvaluator -> https://github.com/Leofaber/ExpRatioEvaluator	

	2) Il software scrive un file di log che elenca tutte le classificazioni effettuate (backgrounds e sources) con il seguente formato:	

	indice,  etichetta_di_classificazione,  L_GAL,   B_GAL,   confidenza_(%),   dataUTC,   dataTT, soglia_di_classificazione,  nome_file_di_log,  exp-ratio     

	
### Esempio di output

	File: output_0000000010_001_GRBTESTMAP_120_140_10.txt

	1 BG 66.714393 43.532257 0.734106 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP -1.000000
	2 SOURCE 24.858941 47.314162 100.000000 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP -1.000000
	3 BG 61.788211 28.181715 56.217029 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP 42.534206
	4 SOURCE 45.313232 37.249633 100.000000 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP 77.572873
	5 BG 41.636776 16.708706 1.441523 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP 89.351576
	6 SOURCE 30.767518 16.509013 98.090464 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP 65.675193
	7 BG 51.624101 7.100261 1.441523 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP -1.000000


	Se non vengono effettuate classificazioni:
	
	NO_BLOBS, 2017-08-28T19:14:08, 137843757, SCAN100_431032381.0_431032481.0_23782, 95



### Soglie di classificazione

Sono state individuate due tipi di soglia: una che minimizza la FPR, una che massimizza l’accuratezza.

	1) SOGLIA DI MINIMIZZAZIONE DELLA FALSE POSITIVE RATE: ** 99,99 **

		FPR	TPR	TNR	FNR	ACCURACY  K-Cohen
		0,863   50,27   99,13   49,72   86,72	  58	

	2) SOGLIA DI MASSIMIZZAZIONE DELL’ACCURATEZZA: **  94,86  **

		FPR	 TPR	 TNR	FNR	ACCURACY  K-Cohen  
		3,3976	 69,56	 96,88	30,43	89,94	  71	   

### Legenda

	FPR: False Positive Rate (False Alarms)
	TPR: True Positive Rate
	TNR: True Negative Rate
	FNR: False Negative Rate (Miss rate)

	Per ulteriori informazioni riguardo ad i valori di performance (formule, training e test set,
	significato etcetc) si rimanda alla documentazione.
	
### Dipendenze:

	ExpRatioEvaluator https://github.com/Leofaber/ExpRatioEvaluator
