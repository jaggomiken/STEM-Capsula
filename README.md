# STEM CAPSULA

<p align="center">
<img src="images/stemcapsula-header-00.jpg" width="100%">
</p>

<p align="center">
<b>Costruzione di Idee e di Complessi di Senso.</b>
</p>

---

# Cosa è STEM Capsula

<p align="justify">
Un luogo di lavoro condiviso per tutti coloro che vogliano cimentarsi nella costruzione di idee e di complessi di senso, nel perimetro appena visibile di Scienza, Tecnologia, Engineering e Matematica, con il prezioso aiuto dell'informatica e la ricchezza e libertà espressiva dell'Arte. Non c'è cosa più difficile che comunicare con gli umani. Non si è mai certi di aver costruito il discorso secondo una struttura di senso che sia inequivocabile, comprensibile, criticabile. Pur tuttavia, questo è lo sforzo che siamo chiamati a fare. I ragazzi possono, vogliono, devono imparare a organizzare le parole e le frasi, organizzare i numeri, organizzare i simboli più complessi, organizzare le azioni del corpo, organizzare la voce e il canto, organizzare il pensiero. Tutto nel linguaggio e nel pensiero è una continua cascata di analogie... 
</p>

| MEZZO          | URL 																		 |
|----------------|-----------------------------------------|
| SITO UFFICIALE | <https://www.gammasix.eu/STEM-Capsula/> |
|----------------|-----------------------------------------|
| CANALE YOUTUBE | <https://youtube.com/@stemcapsula>      |
|----------------|-----------------------------------------|

---

# I PROGETTI ATTIVI

---

<p align="justify">
	NB: Nessuno dei progetti software è stato realizzato mediante generatori di codice AI, dunque nessun vibe-coding applicato. Sono unicamente il frutto di un'attività di progettazione, design, concettualizzazione e scrittura di codice da parte dell'autore. Una sola funzione C è stata generata mediante ChatGPT, piuttosto che scaricata in forma di libreria da Internet: la trasformata di Fourier per l'analisi in frequenza del segnale audio, integrata in STEMCAPSULAX. Il progetto STEM CAPSULA promuove questo tipo di utilizzo della AI, cioè **richieste atomiche di funzioni elementari per svolgere compiti specifici perfettamente inquadrabili nelle conoscenze e competenze del designer, ma onerose in termini di tempo di sviluppo**.
</p>

---

# DILLO XL (pensiero computazionale applicato)

<p align="center">
<img src="images/dilloxl-00.png" width="100%">
</p>

## Abstract

<p align="justify">
**DILLO XL** è prima di tutto un runtime per il linguaggio didattico DILLO, il cui obiettivo è mettere gli studenti nelle condizioni di familiarizzare con il paradigma COMANDANTE/ESECUTORE tipico dei linguaggi di programmazione imperativi, in cui un comandante (l'umano) istruisce una macchina (l'esecutore), con il fine di fargli raggiungere un determinato obiettivo (o insieme di obiettivi) sulla base di una sequenza di istruzioni (un programma). Per ottenere questo obiettivo, DILLO XL sfrutta un meccanismo elementare: fornisce un insieme di primitive per pilotare un drone volante **TELLO EDU** della *Ryze Robotics* (ora DJI). Nello specifico, **DILLO XL** è alla fine un ambiente di sviluppo di software nel linguaggio *DILLO*, per controllare un drone volante e, indirettamente, imparare sequenze, ripetizioni e selezioni, variabili e costanti, primitive matematiche e tecniche di controllo (seppur elementari) di sistemi di sensori/attuatori. Esso fornisce un doppio livello di indirezione: comandante -> esecutore (computer personale) che comanda -> drone (computer embedded).
</p>

| Problema                                | Soluzione           | Tecnologie         |
|-----------------------------------------|---------------------|--------------------|
| Acquisire l'Attenzione degli Studenti   | Uscire dall'aula    | Droni e Linguaggi  |
|-----------------------------------------|---------------------|--------------------|
| Trattenere l'Attenzione degli Studenti  | Programmare Droni   | Droni e Linguaggi  |
|-----------------------------------------|---------------------|--------------------|
| Imparare a programmare                  | Linguaggio Semplice | DILLO              |
|-----------------------------------------|---------------------|--------------------|
| Curva di apprendimento non ripidia      | IDE in DILLO XL     | DILLO XL           |
|-----------------------------------------|---------------------|--------------------|
| Evitare la noia                         | Fare Missioni       | DILLO XL           |
|-----------------------------------------|---------------------|--------------------|

### Approfondimento

<p align="justify">
  **DILLO XL** è rilasciato secondo una licenza opensource GPL ed è disponibile nel branch devel di questo repository, all'interno della cartella Progetti. È scritto in C++17 ed è multi piattaforma, cioè il suo sorgente compila e viene eseguito su Windows, Linux e macOS, dunque è un sorgente scritto per essere portabile. Il progetto si basa sul sistema di meta-build CMake e può essere compilato su Windows all'interno di MSYS-UCRT64, su Linux (mediante GCC) e su macOS (mediante Clang). Qui sotto un tipico sorgente *DILLO*:
</p>

```
#include "dilloxl"

//
// Il mio primo programma che fa volare il drone Tello.
// Il tuo nome e cognome: <scrivi qui>
// La data di oggi: AAAA-MM-GG
//
programma_inizio
	crea_oggetto_da_astrazione(Drone) dando_nome(plinio)
	crea_oggetto_da_astrazione(numero_intero_positivo) dando_nome(passi)
	passi = 0;
	messaggia_oggetto( plinio, decolla, (0) )
	fai_questo_finche_e_vero_che(passi < 3)
		messaggia_oggetto( plinio, vai_avanti_cm, (50))
		passi = passi + 1;
	fine_questo
	passi = 0;
	fai_questo_finche_e_vero_che(passi < 3)
		messaggia_oggetto( plinio, vai_indietro_cm, (50))
		passi = passi + 1;
	fine_questo
	messaggia_oggetto( plinio, ruota_senso_orario_gradi, (90) )
	messaggia_oggetto( plinio, ruota_senso_antiorario_gradi, (90) )
	messaggia_oggetto( plinio, vai_a_sinistra_cm, (50) )
	messaggia_oggetto( plinio, vai_a_destra_cm, (50) )
	messaggia_oggetto( plinio, vai_su_cm, (50) )
	messaggia_oggetto( plinio, vai_giu_cm, (50) )
	messaggia_oggetto( plinio, atterra, (0) )
programma_fine
```
Qui di seguito l'albero del progetto, che è standardizzato per tutti gli altri progetti.

```
.
├── docs
├── images
└── progetti
    ├── DilloXL
    │   ├── ext
    │   ├── fonts
    │   ├── images
    │   ├── inc
    │   └── src
```

---

# LCG (Lezioni di Computer Grafica)

<p align="center">
<img src="images/lcg-00.png" width="60%">
</p>

## Abstract

<p align="justify">
**LCG (Lezioni di Computer Grafica)** è un software molto semplice scritto in C++17, il cui scopo è avvicinare gli studenti appassionati e con pregressa conoscenza del linguaggio C/C++ ai concetti della grafica 2D e 3D. Il software è tuttavia strutturato per funzionare non solo su un sistema desktop (Windows, Linux e macOS), ma anche all'interno di una macchina virtuale nel browser mediante la tecnologia Web Assembly. Nello specifico, è scritto per compilare e linkare con la toolchain fornita da *EMSCRIPTEN* e dunque è facilmente distribuibile come terna {HTML, WASM, JavaScript} e quindi disponibile per la distribuzione via HTTP e l'esecuzione in un qualunque browser moderno. Per apprezzare una dimostrazione funzionante, benché non con tutte le funzionalità pianificate, è possibile cliccare al seguente link <a href="https://www.gammasix.eu/STEM-Capsula/lcg.html" target="_blank" rel="noopener noreferrer">LCG</a>.
</p>

| Problema | Soluzione | Tecnologie |
|----------|-----------|------------|
| ... | ... | ... |

### Approfondimento

<p align="justify">

...

</p>

<p align="center">

➡ **Apri il progetto**

</p>

---

# STEMCAPSULA-X (Laboratorio 2D/3D)

<p align="center">
<img src="images/stemcapsulax-00.png" width="100%">
</p>

## Abstract

<p align="justify">
	**STEMCAPSULA X** è un software, strutturato come un semplicissimo framework basilare, che consente di realizzare anche semplici giochi con grafica 2D e 3D. È nato come esperimento per provare la fantastica libreria RAYLIB, ma poi si è evoluto come banco di lavoro per costruire dimostratori e applicazioni per simulare la fisica del corpo rigido (mediante la libreria Box2D), l'analisi in frequenza per mettere insieme grafica e musica e, in futuro, la simulazione di mondi 3D mediante la vera novità di questo nostro 2026, che è Box3D, dal medesimo creatore di Box2D. Nella sua architettura, STEMCAPSULAX impiega una gerarchia di classi e alcuni design pattern tipici dei videogame, privilegiando la scrittura di codice rispetto alla manipolazione di file di dati (prettamente configuration-oriented). Il fine è, infatti, allenare ed esprimere le attitudini di codice e design degli studenti esperti. Anche questo software, come gli altri, si basa sul sistema di meta-build CMake, compila e poi gira su Windows, Linux e macOS, per cui la sua base di codice è scritta per Microsoft CL (Visual Studio), GCC e Clang. Nel tempo ha subito un'evoluzione sostanziale, per cui nei video dimostrativi qui presenti noterete alcune differenze nelle feature e nei comportamenti.
</p>

| Problema | Soluzione | Tecnologie |
|----------|-----------|------------|
| ... | ... | ... |

### Approfondimento

<p align="justify">
	Nel video qui sotto possiamo vedere all'opera l'applicazione che mostra il una serie di Layer sovrapposti, il più evidente dei quali è integrato con il simulatore fisico Box2D. Nello specifico, il framework costruisce la classe Pupazzo e ne istanzia 7 versioni con colori e dimensioni differenti. Il pupazzo è formato da un certo numero di Body (concetto di Box2D) collegati tra loro da Joint (anche questo è concetto di Box2D). L'utente può esercitare una forza impulsiva (di tipo Explosion) per sollecitare il moto dei copri e verificarne le reazioni. Esiste poi una funzione che legge una PNG e la particellizza in una serie di Body piccoli colorati come i pixel dell'immagine, soggetti alla gravità e al regime di forze del sistema, ottendendo effetti cascata e rimbalzo.<br>
	<i>Il tuo browser potrebbe drasticamente ridurre il framerate di playback per questo video.</i>
</p>

<p align="center">
<img src="images/stemcapsulax_demo_01.webp" width="900">
</p>

<p align="justify">
	Nel video qui sotto possiamo vedere all'opera una versione precedente che forniva piccole funzionalità di creazione di oggetti Body (Box2D) dalle forme più disparate, disegnati a schermo mediante la funzionalità DebugDraw di Box2D e dunque ricca di elementi che mostrano gli aspetti interni della libreria di simulazione fisica. Un semplice banco di prova, da cui poi derivare un progetto vero e proprio.<br>
	<i>Il tuo browser potrebbe drasticamente ridurre il framerate di playback per questo video.</i>
</p>

<p align="center">
<img src="images/stemcapsulax_demo_02.webp" width="900">
</p>

<p align="center">

➡ **Apri il progetto**

</p>

---

# Tecnologie

<p align="center">

| Linguaggio | Framework | Utilizzo |
|------------|-----------|----------|
| ... | ... | ... |

</p>

---

# Struttura del repository

```
/
│
├── project1/
├── project2/
├── project3/
├── images/
├── docs/
└── README.md
```

---

# Roadmap

- [ ] ...
- [ ] ...
- [ ] ...

---

# Contatti

...
