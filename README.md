# agextspot-v2


AGEXTSPOT-V2

	La versione 2 è stata adattata ad immagini con tempo di esposizione 2s -> 100s.



UTILIZZO

	./nome_eseguibile     path_al_file_fits     nome_file_di_log     soglia_di_classificazione



OUTPUT

	classificazione,[L,B],probabilità che il blob si un GRB

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

	1) SOGLIA DI MINIMIZZAZIONE DELLA FALSE POSITIVE RATE (99,99)

		FPR	TPR	  THRESHOLD
		1,0407  50,8804   99,99

	2) SOGLIA DI MASSIMIZZAZIONE DELL’ACCURATEZZA (94,86)

		FPR	ACCURACY   THRESHOLD
		3,3976  90,06	   94,86



