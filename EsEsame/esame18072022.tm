<TeXmacs|2.1.5>

<style|<tuple|generic|italian>>

<\body>
  <\question>
    \;
  </question>

  Si descriva il funzionamento dello scheduler di CPU Unix tradizionale,
  discutendo anche quali siano le system-call offerte per la gestione di
  questo scheduler.

  \;

  lo scheduler unix traadizionale utilizza un meccanismo a code multiple con
  feedback da M a -N livelli di priorità dove M è il livello di priorità
  peggiore e -N il migliore , la priotià massimima quando si rientra da swap
  out (livello -N) e quando si rientra sa I/O si viene posizionati al livelo
  (-N+1) . I processi vengono ammessi ad un livello base 0 e lo scheduler
  calcola periodicamente la loro priorità attraverso una formula:

  <\eqnarray*>
    <tformat|<table|<row|<cell|P>|<cell|=>|<cell|base+<frac|Cpu
    Usage|2>+nice>>>>
  </eqnarray*>

  Dove base è una priorità base scelta dal so, Cpu usage rappresenta lo
  storico di quanta cpu ha utilizzato, quindi più cpu andra ad usare più la
  sua priorità di alzera numericamente ma scendera nell'effettivo , nice un
  valore che può essere anche negativo gestito dall'utente attraverso la
  syscall nice(int val), con la nice possiamo alzare o abbassare la sua
  priorità e possiamo rendere più o meno amico il processo.

  La gestione di ogni singola coda è round robin , quindi si va in esecuzione
  in modo circolare.

  Le syscall per la gestione di questo scheduler sono\ 

  <\itemize>
    <item>Nice(int val): che come detto prima può essere anche negativa , va
    a modificare quanto è amichevole il processo\ 

    <item>GetPriority(int which, int who) ritorna il valore la priorità di un
    singolo processo o di un songolo gruppo\ 

    <item>setPriority imposta il valore della priorita di un processo o di un
    gruppo.
  </itemize>

  SVR4 ha 160 livelli divisi in

  <\itemize>
    <item>159-100 real time

    <item>99-60 kernel

    <item>59-0 time sharing\ 
  </itemize>

  In questo caso anche il kernel diventa preemtabile , ovvero possiamo
  interrompere il kernel all'arrivo di un processo di priorità superiore però
  il kernel deve trovarsi in una posizione sicura del kernel , il kernel è
  fatto di molte zone sicure dove può essere fermato.

  SCHEDULING POSIX , ha 100 livelli di priorita da real time a time sharing,
  la valenza del valore della niceness vale solo tra processi dello stesso
  livelli di priorita , due thread con priorità diversa e di cui quello con
  priorità minore ha niceness maggiore , essendo in livelli di priorità
  diversi la niceness diventa uan don't care.

  Si introduce il concetto di epoca di scheduling , un delta tempo in cui
  bisogna dare in base alla priorità dei processi quanti di tempo per ogni
  thread, cosi da evitare la starvation.

  \;

  \;

  \;

  \;

  <\question>
    \;

    Si descriva il metodo di allocazione dei file a catena, discutendone
    vantaggi e svantaggi. Considerando un file di 10 K record, allocato
    secondo lo schema a catena, e un dispositivo di memoria di massa avente
    blocchi di taglia pari a 1 K record, la cui latenza per il caricamento in
    memoria di lavoro è pari a 10 millisecondi, si calcoli la latenza di caso
    peggiore per il caricamento in memoria dell'ultimo record del file
    considerando che il tempo richiesto dal software di sistema per
    l'identificazione di un indice di blocco di dispositivo (quando già
    caricato in memoria) sia pari a 1 millisecondo, che il record di sistema
    (RS) di ogni file sia contenuto in un unico blocco del dispositivo, e che
    l'indicizzazione dei blocchi di dispositivo richieda l'utilizzo di indici
    di taglia pari a 64 record.
  </question>

  L'allocazione a catena (o concatenata) prevede che un file sia memorizzato
  come una lista concatenata di blocchi del disco. I blocchi possono essere
  sparsi ovunque sul supporto fisico.

  Il <with|font-series|bold|File Control Block (FCB)> contiene unicamente il
  puntatore al <with|font-series|bold|primo blocco> del file (e talvolta
  all'ultimo). Ogni blocco sul disco contiene una porzione di dati utili e un
  puntatore (indirizzo fisico) al blocco successivo della catena.

  <subsection*|1. Vantaggi>

  <\itemize>
    <item><with|font-series|bold|Assenza di Frammentazione Esterna:> Non è
    richiesto spazio contiguo sul disco. Qualsiasi blocco libero può essere
    allocato al file, eliminando la necessità di algoritmi di compattazione
    della memoria secondaria.

    <item><with|font-series|bold|Flessibilità e Crescita Dinamica:> Un file
    può crescere indefinitamente (fino all'esaurimento dei blocchi liberi del
    disco) senza dover pre-allocare lo spazio o conoscere in anticipo la
    dimensione finale del file.

    <item><with|font-series|bold|Splicing/Inserimento Semplice:> È molto
    facile inserire o rimuovere blocchi all'interno del file modificando
    semplicemente i puntatori, senza dover spostare fisicamente i dati
    adiacenti.
  </itemize>

  <subsection*|2. Svantaggi>

  <\itemize>
    <item><with|font-series|bold|Accesso Casuale Inefficiente:> Il difetto
    principale è che supporta efficacemente solo l'accesso sequenziale. Per
    accedere al blocco $N$, il sistema deve obbligatoriamente leggere i
    precedenti $N-1$ blocchi dal disco per recuperare i puntatori, rendendo
    l'accesso diretto estremamente lento.

    <item><with|font-series|bold|Spreco di Spazio nel Blocco (Internal
    Overhead):> Ogni blocco deve riservare una manciata di byte per il
    puntatore interno. Questo rompe la corrispondenza binaria standard delle
    dimensioni dei blocchi (es. le potenze di 2), complicando il calcolo dei
    record logici da parte del sistema operativo.

    <item><with|font-series|bold|Scarsa Affidabilità:> Se un singolo blocco
    si danneggia o un puntatore viene corrotto, l'intera catena a valle viene
    persa, poiché non è più possibile recuperare l'indirizzo del blocco
    successivo.
  </itemize>

  \;

  Blocchi necessari\ 

  <\eqnarray*>
    <tformat|<table|<row|<cell|blocchi nevessari>|<cell|=>|<cell|<frac|10000|1000-64>
    >>|<row|<cell|>|<cell|=>|<cell|11>>|<row|<cell|accessi al
    disco>|<cell|=>|<cell|1<around*|(|Rs|)>+11<around*|(|blocchi|)>>>|<row|<cell|tempo
    I/o>|<cell|=>|<cell|12*\<times\>10=120ms>>|<row|<cell|tempo eleborazione
    indici>|<cell|=>|<cell|11\<times\>1=11ms>>|<row|<cell|tempo
    tot>|<cell|=>|<cell|131 ms>>>>
  </eqnarray*>

  <\question>
    Descrivere l'algoritmo dell'orologio per l'identificazione della vittima
    in sistemi basati su paginazione e memoria virtuale. Inoltre si indichi
    se tale algoritmo soffre o meno dell'anomalia di Belady.
  </question>

  L'algoritmo dell'orologio (<with|font-shape|italic|Clock Algorithm>) è un
  meccanismo a basso overhead impiegato per l'identificazione della pagina
  vittima nei sistemi con memoria virtuale e paginazione. Nella sua variante
  <with|font-series|bold|a due lancette> (<with|font-shape|italic|Two-Handed
  Clock>), la struttura e il funzionamento si articolano come segue:

  <subsection*|1. Struttura e Funzionamento>

  <\itemize>
    <item><with|font-series|bold|Reference Bit:> A ogni frame di memoria
    fisica è associato un bit hardware di riferimento
    (<with|font-shape|italic|Reference Bit>). Quando una pagina viene
    referenziata (sia in lettura che in scrittura), l'hardware imposta
    automaticamente questo bit a <with|font-series|bold|1>.

    <item><with|font-series|bold|Le Due Lancette:> I frame sono organizzati
    logicamente in una lista circolare presieduta da due lancette che si
    muovono mantenendo una distanza (offset) costante l'una dall'altra:

    <\itemize>
      <item><with|font-series|bold|Lancetta Anteriore (di Reset):> Avanza
      ciclicamente e in modo indipendente con il compito di
      <with|font-series|bold|azzerare (portare a 0)> il
      <with|font-shape|italic|Reference Bit> delle pagine che incontra,
      determinandone l'invecchiamento.

      <item><with|font-series|bold|Lancetta Posteriore (della Vittima):>
      Segue la prima e viene interpellata solo quando si verifica un
      <with|font-shape|italic|page fault> e il sistema necessita di una
      vittima. Esamina il <with|font-shape|italic|Reference Bit> del frame
      corrente: se è rimasto a <with|font-series|bold|0> (segno che la pagina
      non è più stata riutilizzata dopo il passaggio della prima lancetta),
      la pagina viene scelta come <with|font-series|bold|vittima>. Se è
      tornato a <with|font-series|bold|1>, la pagina viene risparmiata e si
      passa alla successiva.
    </itemize>
  </itemize>

  <subsection*|2. Identificazione della Vittima nel Multiprocessore>

  In un sistema monoprocessore, l'algoritmo garantisce sempre
  l'individuazione di una vittima (al massimo azzerando tutte le pagine in un
  ciclo). Nei sistemi <with|font-series|bold|multiprocessore>, invece,
  l'efficienza può ridursi: l'attività contemporanea di più core sui medesimi
  frame può far sì che i <with|font-shape|italic|Reference Bit> vengano
  reimpostati a 1 dai processi parallelamente all'avanzamento delle lancette.
  Questo può costringere l'algoritmo a compiere più giri della struttura
  circolare prima di trovare un bit a 0, oltre a introdurre problemi di
  contesa (<with|font-shape|italic|locking>) sulla struttura dati globale
  dell'orologio.

  <subsection*|3. Relazione con l'Anomalia di Belady>

  L'algoritmo dell'orologio <with|font-series|bold|soffre dell'anomalia di
  Belady>. Un algoritmo di rimpiazzamento è immune a tale anomalia (definita
  come il paradossale aumento dei <with|font-shape|italic|major page fault>
  all'aumentare dei frame fisici disponibili) solo se appartiene alla classe
  degli <with|font-series|bold|algoritmi a stack>, i quali soddisfano la
  proprietà di inclusione. Poiché l'orologio si basa fondamentalmente su una
  scansione circolare di derivazione FIFO (seppur mitigata dal bit di
  riferimento), non rispetta tale proprietà matematica e rimane pertanto
  vulnerabile all'anomalia su determinate stringhe di riferimento. Gli unici
  algoritmi immuni sono l'Ottimo (non implementabile poiché richiede la
  conoscenza del futuro) e l'LRU (<with|font-shape|italic|Least Recently
  Used>).

  <\question>
    Si consideri un sistema con due processi PROC1 e PROC2 che scambiano
    periodicamente informazioni utilizzando due segmenti di memoria condivisa
    M1 e M2. Lo scambio delle informazioni avviene secondo il seguente
    schema: PROC1 scrive un nuovo messaggio in M1 mentre PROC2 scrive un
    nuovo messaggio in M2, PROC2 scrive una risposta per un messaggio di
    PROC1 in M1 mentre PROC1 scrive una risposta per un messaggio di PROC2 in
    M2. Quando qualsiasi dei due processi (PROC1 o PROC2) intende scambiare
    informazioni scrivendo il proprio messaggio nel relativo slot della
    memoria condivisa, esso deve rimanere in attesa che anche l'altro
    processo intenda scambiare informazioni scrivendo il suo messaggio. In
    particolare, dopo aver scritto un nuovo messaggio, ogni processo deve
    rimanere in attesa della risposta da parte dell'altro processo. Si
    schematizzi la soluzione del suddetto problema di sincronizzazione,
    usando solo semafori, fornendo lo pseudo-codice delle procedure per lo
    scambio di informazioni usate da PROC1 e PROC2.
  </question>

  \;

  <\code>
    sem_t readyInfoM1 init = 0

    sem_t readyInfoM2 init = 0

    sem_t ready1 init = 0;

    sem_t ready2 init =0;

    \;

    proc1(msg):

    Signal(ready1,1)

    wait(ready2,1)

    M1 = msg

    signal(readyInfoM1,1)

    wait(readyInfoM2,1)

    Ris=M2

    M2=msg

    signal(readyInfoM2)

    wait(readyInfoM1)

    msg=M1

    return msg

    \;

    proc2(msg):

    signal(ready2,1)

    wait(ready2,1)

    M2=msg

    signal(readyInfoM2,1)

    wait(readyInfoM1,1)

    risposta = M2

    M1=Replay(msg)

    signal(readyInfoM1,1)

    wair(readyInfoM2,2)

    msg=M2

    return(msg)
  </code>

  \;

  \;
</body>

<\initial>
  <\collection>
    <associate|page-medium|paper>
  </collection>
</initial>

<\references>
  <\collection>
    <associate|auto-1|<tuple|2|?>>
    <associate|auto-2|<tuple|<with|mode|<quote|math>|\<bullet\>>|?>>
    <associate|auto-3|<tuple|3|?>>
    <associate|auto-4|<tuple|<with|mode|<quote|math>|<rigid|\<circ\>>>|?>>
    <associate|auto-5|<tuple|<with|mode|<quote|math>|<rigid|\<circ\>>>|?>>
  </collection>
</references>