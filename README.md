# AG_extspot-v2

AG_extspot-v2 è un software di estrazione automatica di possibili blob candidati ad ulteriori analisi al fine di identificare automaticamente eventi astrofisici ad alta energia.

La versione 2 è stata migliorata ed adattata ad un contesto di short-term analysis in cui si analizzano mappe con tempo di esposizione da 2 a 100 secondi.

La rilevazione viene effettuata attraverso il seguente algoritmo:
* conversione del file FITS in immagine opencv
* smoothing gaussiano, thresholding (soglia=valore medio dei livelli di grigio), estrazione dei blobs con numero di fotoni >= 2
* calcolo delle features dei blobs estratti (numero di fotoni,indice di vicinanza fotoni)
* utilizzo di un Naive Bayes Classifier per classificare i blobs come SOURCE o BACKGROUND.

Può essere anche calcolata la valutazione exp-ratio per ogni blob trovato. La routine exp-ratio permette di valutare quando una “detection” o “spot” è troppo vicina ai bordi dell’exposure del telescopio. Tutte le valutazioni vengono fatte su di una mappa exp NORMALIZZATA. (se la mappa in input NON è normalizzata, il software provvederà a normalizzarla) Restituisce un numero compreso tra 0 e 100 (0 bad).

**Documentazione completa del software:** https://docs.google.com/document/d/12GUmqOhHCWN3L290vGGeJIBehVWHIcdA6kM9zK-BgcQ/

## Installazione ed utilizzo
```bash
git clone https://github.com/Leofaber/agextspot-v2
cd agextspot-v2
export AGILE=.. CFITSIO=.. OPENCV=..
make
export PFILES=.. (must point where AG_extspot-v2.par is located e.g. conf/ )
./bin/AG_extspot outputLogName, psf, imageCtsPath, classificationThreshold, visualizationMode, imageExpPath, isExpMapNormalized, createExpNormalizedMap, createExpRatioMap, minTreshold, maxTreshold, squareSize
```

### Input
* outputLogPrefixName [string, required]: il prefisso del file di output (è possibile specificare anche il percorso ma questo DEVE esistere). Il nome del file di output verrà generato	in questo modo: [percorso]/prefisso_nomemappa.txt

* psf [double, required]: la point spread function del telescopio.

* imageCtsPath [string, required]: il percorso alla mappa dei conteggi (.cts o .cts.gz).

* classificationThreshold [dobule, required]: la soglia di classificazione, se la percentuale bayesiana di classificazione supera la soglia, il blob i-esimo viene etichettato come GRB

* visualizationMode [yes/no, required]: se il valore è 'yes', saranno mostrati per via grafica i vari passi di estrazione dei blob (immagine originale, smoothing, thresholding, istrogramma dei livelli di grigio).

* imageExpPath [string, required]: il percorso alla mappa di esposizone (.exp o .exp.gz). Se il valore è "None" la valutazione exp ratio non verrà effettuata: nel file di output verrà riportato il valore -1.

* isExpMapNormalized [yes/no, optional]: se il valore è 'no', si afferma che la mappa exp in input (imageExpPath) NON è normalizzata. Il software provvederà a normalizzarla.

* createExpNormalizedMap [yes/no, optional]: se il valore è 'yes' verrà scritta su file la mappa normalizzata. ( yes/no )

* createExpRatioMap [yes/no, optional]: se il valore è 'yes', viene creata una mappa (FITS) nella quale il valore di ogni pixel rappresenta il valore dell'ExpRatioEvaluator calcolato sul medesimo pixel. Se il valore è 'no' non viene creata alcuna mappa.

* minThreshold [double, optional]: la soglia minima sotto la quale il pixel è considerato "bad" per il calcolo dell'exp-ratio

* maxThreshold [double, optional]: la soglia minima sopra la quale il pixel è considerato "bad" per il calcolo dell'exp-ratio

* squareSize [int, optional]: la dimensione in pixel del lato della regione (quadrato) su cui si calcola l'exp-ratio


### Output

* L'outuput di ExpRatioEvaluator (se abilitato) -> https://github.com/Leofaber/ExpRatioEvaluator

* Un file di log che elenca tutte le classificazioni effettuate (backgrounds e sources) con il seguente formato:
indice,  etichetta_di_classificazione,  L_GAL,   B_GAL,   confidenza_(%),   dataUTC,   dataTT, soglia_di_classificazione,  nome_file_di_log,  exp-ratio     

* Se la modalità di visualizzazione è attiva, saranno mostrati per via grafica i vari passi di estrazione dei blob (immagine originale, smoothing, thresholding, istrogramma dei livelli di grigio).

#### Esempio di output

File: output_0000000010_001_GRBTESTMAP_120_140_10.txt

1 BG 66.714393 43.532257 0.734106 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP -1.000000
2 SOURCE 24.858941 47.314162 100.000000 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP -1.000000
3 BG 61.788211 28.181715 56.217029 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP 42.534206
4 SOURCE 45.313232 37.249633 100.000000 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP 77.572873
5 BG 41.636776 16.708706 1.441523 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP 89.351576
6 SOURCE 30.767518 16.509013 98.090464 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP 65.675193
7 BG 51.624101 7.100261 1.441523 2017-03-13T22:41:16 416529609 95.000000 0000000010_001_GRBTESTMAP -1.000000


Se non vengono effettuate classificazioni:

NO_BLOBS 2017-08-28T19:14:08 137843757 SCAN100_431032381.0_431032481.0_23782 95 -1.000000

## Esempio di utilizzo
```bash
./bin/AG_extspot-v2 logs/LOG 7.5 ../AG_extspot-v2-TESTING/0000000150_001_F_2s_60000_0.cts.gz 50 no ../AG_extspot-v2-TESTING/MAP00002s_45l_30b.exp.gz no yes yes 45 30 30
```
Output:
* logs/LOG_0000000150_001_F_2s_60000_0.txt
* MAP00002s_45l_30b_0_0_30.nexp.gz (from ExpRatioEvaluator)
* MAP00002s_45l_30b_0_0_30.expratio.gz (from ExpRatioEvaluator)

```bash
./bin/AG_extspot-v2 logs/LOG 7.5 ../AG_extspot-v2-TESTING/0000000150_001_F_2s_60000_0.cts.gz 50 no None
```
Output:
* logs/LOG_0000000150_001_F_2s_60000_0.txt


## Training e testing del classificatore Naive Bayes
Il classificatore viene utilizzato per assegnare ad ogni blob estratto tramite la fase di image processing una probabilità che il blob sia generato da un’effettiva sorgente di raggi gamma o semplicemente da background. Per il training ed il testing del classificatore sono presenti due tools di supporto.

### AG_extspost-v2-bayesian-model-evaluator
Il tool AG_extspot-bayesian-model-evaluator  viene utilizzato nella fase di training al fine di ottenere le features dei BLOBs e le relative distribuzioni normali, le quali saranno utilizzate all’interno della classe BayesianClassifierForBlobs di AG_extspot-v2 come valori per le probabilità a priori all’interno della formula della probabilità condizionata che esprime il teorema di Bayes. Al fine di discriminare un blob di flusso da un blob di background sono state individuate le seguenti features:
* AREA (area del blob espressa in gradi celesti );
* PHOTONS (numero di fotoni contenuti all’interno del blob );
* PHOTONS_CLOSENESS (indice di vicinanza dei fotoni nel blob );

#### Generazione del training set
La cartella del training set deve avere la seguente struttura:
* training-set
	* /bg
	* /flux

Le mappe simulate devono essere inserite all'interno delle corrispondenti cartelle a seconda se presentano una sorgente o solo background. Il tool provvederà a
creare il training-set effettivo, estraendo i BLOBs da queste mappe etichettandoli ed analizzandone le features.

#### Utilizzo
```bash
git clone https://github.com/Leofaber/agextspot-v2
cd agextspot-v2
export AGILE=.. CFITSIO=.. OPENCV=..
make training
./bin/AG_extspot-v2-bayesian-model-evaluator <path-training-set> <CDELT1> <CDELT2> <PSF>
```
#### Input
* path-training-set [string, required] →  path al training test (la cartella del training set deve contenere due sottocartelle bg e flux).
* CDELT1 [float, required] → la dimensione x in gradi dei pixels delle immagini FITS del training set.
* CDELT2 [float, required] → la dimensione y in gradi dei pixels delle immagini FITS del training set.
* PSF [float, required] → la point-spread-function, caratteristica del telescopio.


### AG_extspot-v2-performance-evaluator
Lo scopo di questo tool è valutare le performances del classificatore Bayesiano di AG_extspot-v2. Per ogni step-soglia nell'intervallo [minTreshold, maxThreshold] si calcolano le seguenti variabili di prestazione:

	* TP (True Positive);
	* FP (False Positive);
	* TN (True Negative);
	* FN (False Negative);
	* falsePositiveRate = FP/totalBackgroundInstances;
	* falseNegativeRate = FN/totalFluxInstances;
	* truePositiveRate = precision = TP/(TP+FP);
	* sensitivity = TP/(TP+FN);
	* specificity = TN/(TN+FP);
	* accuracy = sensitivity*(totalFluxInstances/totalInstances) + specificity * (totalBackgroundInstances/totalInstances);
	* precision = TP/(TP+FP);
	* f_measure = 2 * ((precision * sensitivity) / (precision + sensitivity));
	* k_choen = ( ( (TP + TN) / totalInstances ) -pr_e) / (1-pr_e);
	* errorDistancesMean = errorDistancesTotal / errorDistances.size;
	* errorDistancesDeviation = sqrt(errorDistancesDeviation/errorDistances.size()).

Questi valori vengono salvati in 2 files, un file di log e un file utilizzato per ottenere grafici di performance grazie allo script *draw_performance_plot.py*.

#### Generazione del test-set
La cartella del test-set conterrà x mappe. Il primo carattere (F o B) del nome della mappa deve indicare se la mappa presenti una sorgente (flux) oppure no (background).
Il tool provvederà a creare il test-set effettivo, estraendo i BLOBs da queste mappe.

#### Utilizzo
```bash
git clone https://github.com/Leofaber/agextspot-v2
cd agextspot-v2
export AGILE=.. CFITSIO=.. OPENCV=..
make training
 ./bin/AG_extspot-v2-performance-evaluator <test-set-path> <minThreshold> <maxThreshold> <step> <CDELT1> <CDELT2> <PSF>
python draw_performance_plot.py <file-di-performances>
```

#### Input
* test-set-path [string, required]: il path al test-set.
* minThreshold  [double, required]: soglia di partenza da cui iniziare a valutare le performance.
* maxThreshold [double, required]: soglia massima in cui far finire la valutazione delle performance.  
* step [double, required]: passo di incrementazione della Threshold.
* CDELT1 [float, required] → la dimensione x in gradi dei pixels delle immagini FITS del training set.
* CDELT2 [float, required] → la dimensione y in gradi dei pixels delle immagini FITS del training set.
* PSF [float, required] → la point-spread-function, caratteristica del telescopio.


## Dipendenze esterne

* libagilesci
	* AgileMap.h
	* AlikeData5.h
	* ExpRatioEvaluator.h
* libagilepil
	* PilParams.h
* Opencv 2.4.13
	* opencv2/core/core.hpp
	* opencv2/highgui/highgui.hpp
	* opencv2/imgproc/imgproc.hpp
* cfitsio
* libagilewcs
* root



## Struttura del progetto

* AG_extspot-v2
	* GammaRayDetector.h
		* BayesianClassifierForBlobs.h
			* Blob.h
			* NormalDistribution.h
		* BlobsFinder.h
			* Opencv.h
			* Blob.h
			* MapConverter.h
		* FileWriter.h
					--
		*	ExpRatioEvaluator.h
			* MapConverter.h
			* FitsUtils.h
			* AgileMap.h
			* AlikeData5.h
		* AgileMap.h

* AG_performance-evaluator
	* PerformanceEvaluator.h
		* BlobsFinder.h
			* Opencv.h
			* Blob.h
			* MapConverter.h
		* BayesianClassifierForBlobs.h
			* Blob.h
		* FileWriter.h
		* FolderManager.h
		* AgileMap.h

* AG_model-evaluator
	* BayesianModelEvaluator.h
		* BlobsFinder.h
			* Opencv.h
			* Blob.h
			* MapConverter.h
		* FolderManager.h
		* MapConverter.h		
		* NormalDistribution.h
