<TeXmacs|2.1.5>

<style|<tuple|generic|italian>>

<\body>
  Si descriva lo scheduling di CPU round-robin virtuale. Si consideri uno
  scenario in cui lo scheduler di CPU round-robin virtuale sia basato su un
  time-slice pari a K \<gtr\> 10 millisecondi, esista un unico processo
  CPU-bound A di durata infinita ed esista un unico processo I/O-bound B di
  durata infinita. Il processo B ha CPU burst di lunghezza pari a 10
  millisecondi. Si indichi il massimo valore di K che possa permettere al
  processo B di avere un tempo di attesa per l'esecuzione del CPU burst non
  più ampio del 250% rispetto alla durata del time-slice.

  \;

  L'algoritmo di scheduling del virtual round robin risolve i problemi di
  round robin , ovvero che sfavoreggiava i processi IO bound e quindi c'era
  un sotto utilizzo dell'IO . virtual round robin introduce il due livelli di
  coda di priorità , una coda di priorità alta per tutti i processi rientrano
  da IO e una coda normale. La coda normale ha un quanto di tempo \<tau\>
  mentre la coda ad alta priorita avra un tempo \<tau\><rsub|p> = \<tau\>-x
  dove x è la quantità di tempo utlizzata nella precedente esecuzione.\ 

  Quindi in vrr i processi che rientrano in stato ready dopo un I/O vengono
  inseriti nella coda ad alta priorità e resteranno in cpu un tempo minore
  del quanto generale.

  vrr ha più riguardo per i processi IO bound ed è più reattivo per
  applicazzioni interattive.

  \;

  Fornire una descrizione degli approcci e dei supporti per la protezione dei
  file utilizzati nei file system Unix.

  <label|p-rc_e0ab2f28cdc7ba0b-188>Nei sistemi UNIX la protezione dei file è
  una componente fondamentale del File System<rsup|>. Il supporto principale
  è basato sui metadati conservati nell'<with|font-series|bold|i-node> del
  file<rsup|>. All'interno dell'i-node viene memorizzata una
  <with|font-series|bold|tripletta di permessi> (lettura <code*|r>, scrittura
  <code*|w>, esecuzione <code*|x>) espressa in codifica ottale, declinata per
  tre categorie di utenti: il proprietario (<with|font-shape|italic|owner>),
  il gruppo (<with|font-shape|italic|group>) e gli altri
  (<with|font-shape|italic|others>)<rsup|>. \ 

  <label|p-rc_e0ab2f28cdc7ba0b-189>Sempre nei metadati dell'i-node troviamo
  gli identificatori numerici del proprietario (<with|font-series|bold|UID>)
  e del gruppo (<with|font-series|bold|GID>)<rsup|>. È importante
  sottolineare che l'associazione tra questi ID numerici e i rispettivi
  username testuali non risiede nell'i-node, ma viene gestita a livello di
  sovrastruttura tramite file di configurazione del sistema, come
  <code*|/etc/passwd> per gli utenti e <code*|/etc/group> per i
  gruppi<rsup|>. \ 

  <label|p-rc_e0ab2f28cdc7ba0b-190>Oltre ai permessi standard, l'i-node
  supporta dei bit speciali di protezione: * <with|font-series|bold|SUID (Set
  User ID) e SGID (Set Group ID):> permettono a un processo che esegue il
  file di acquisire temporaneamente i privilegi del proprietario o del gruppo
  del file stesso<rsup|,,>. * <with|font-series|bold|Sticky bit:> applicato
  alle directory, garantisce che un file possa essere rimosso o rinominato
  esclusivamente dal suo effettivo proprietario (o da root)<rsup|>. \ 

  <label|p-rc_e0ab2f28cdc7ba0b-191>Per una protezione più granulare rispetto
  alla terna tradizionale, i moderni file system UNIX supportano le
  <with|font-series|bold|ACL (Access Control List)>, implementate tramite
  <with|font-shape|italic|i-node shadow>, che permettono di definire permessi
  specifici per singoli utenti o gruppi aggiuntivi<rsup|>. Infine, la
  consistenza e la protezione dei dati durante l'accesso concorrente viene
  supportata a livello di canale di I/O tramite meccanismi di
  <with|font-series|bold|File Locking> (gestiti con la system call
  <code*|fcntl()>)<rsup|>."

  \;

  <new-page>

  Si descriva il meccanismo di gestione della memoria basato sulla
  paginazione. Si consideri uno schema di gestione della memoria basato su
  paginazione, in cui gli indirizzi logici sono formati da 30 bit e
  l'indirizzamento della memoria è al singolo byte. Si consideri inoltre che
  il mecchanismo di paginazione sia basato su due livelli, e che il primo
  livello sia basato su una tabella di 1024 elementi. Si calcoli il numero di
  possibili pagine costituenti l'address space dell'applicazione e il numero
  di elementi di una tabella di secondo livello nel caso in cui ciascuna
  delle pagine abbia taglia 4 KB oppure abbia taglia 8 KB.

  Il meccanismo di paginazione si basa sulla divisione dello spazio di
  indirizzamento logico di un processo in blocchi di dimensione fissa
  chiamati <with|font-series|bold|pagine>, e la memoria fisica in blocchi
  della stessa taglia detti <with|font-series|bold|frame> (tipicamente di 4
  KB).

  Questa tecnica elimina il vincolo di contiguità fisica per l'allocazione
  dell'address space in RAM. Di conseguenza, si azzera il problema della
  <with|font-series|bold|frammentazione esterna> e non sono più necessari
  algoritmi di allocazione dinamica come <with|font-shape|italic|first-fit>,
  <with|font-shape|italic|best-fit> o <with|font-shape|italic|worst-fit>.
  Permane tuttavia il problema della <with|font-series|bold|frammentazione
  interna>, poiché l'ultima pagina di un processo potrebbe non essere
  utilizzata interamente. La scelta della dimensione della pagina è un
  trade-off: pagine troppo piccole portano a tabelle delle pagine
  eccessivamente grandi, mentre pagine troppo grandi aumentano la
  frammentazione interna.

  La traduzione degli indirizzi avviene tramite la
  <with|font-series|bold|tabella delle pagine> (una per ogni processo), che
  mappa il numero di pagina logica nel rispettivo frame fisico. Poiché la
  tabella risiede in memoria, ogni operazione richiederebbe due accessi alla
  RAM (uno per consultare la tabella e uno per i dati), dimezzando le
  prestazioni. Per ovviare a questo, si introduce la
  <with|font-series|bold|TLB (Translation Lookaside Buffer)>, una cache
  hardware molto veloce che memorizza le traduzioni più recenti. In caso di
  <with|font-shape|italic|TLB hit>, la traduzione è quasi istantanea; in caso
  di <with|font-shape|italic|TLB miss>, il sistema deve invece accedere alla
  memoria principale per consultare la tabella delle pagine e aggiornare la
  cache.

  Si consideri un insieme di N processi (P1 , \<ldots\>, PN) ed un altro
  insieme di M processi (L1 , \<ldots\>, LM). Ogni processo Pi scrive
  periodicamente un nuovo messaggio in una memoria condivisa M, che deve
  essere letto una sola volta da tutti i processi Lj. Il processo Pi può
  scrivere un nuovo messaggio solo dopo che l'ultimo messaggio scritto sia
  stato letto da tutti i processi Li. Altrimenti dovrà rimanere in attesa.
  Allo stesso tempo, un processo Pi che ha scritto un messaggo in M ne può
  scrivere un successivo solo dopo che anche tutti gli altri processi Pj (con
  j diverso da i) abbiano scritto il loro messaggio in M. Altrimenti dovrà
  rimanere in attesa. Allo stesso tempo, ogni processo Lj che intende leggere
  dovrà rimanere in attesa che un messaggio non ancora letto d Lj sia reso
  disponibile. Si schematizzi la soluzione del suddetto problema di
  sincronizzazione, usando solo semafori, fornendo lo pseudo-codice delle
  procedure SCRIVI e LEGGI usate rispettivamente dai generici processi Pi e
  Lj.

  <\render-code>
    sem_t writer[N]<space|1em>//init {1,0,<text-dots>,0}

    sem_t reader_Ready[M] //init{0,<text-dots>,0}

    sem_t readerFinish<space|1em>//init 0

    \;

    \ Leggi(i,msg):

    wait(reader_Ready[i],1);

    msg = M;

    signal(readerFinisch,1);

    \;

    Scrivi(i, msg){

    wait(writer[i],1);

    M= msg

    for i in (1, M):

    signal(reader_Ready[i],1);

    \;

    wait(ReaderFinish,N);

    signal(writer((i+1)mod N,1);

    \;

    }

    \;
  </render-code>
</body>

<\initial>
  <\collection>
    <associate|page-medium|paper>
  </collection>
</initial>

<\references>
  <\collection>
    <associate|p-rc_e0ab2f28cdc7ba0b-188|<tuple|?|?>>
    <associate|p-rc_e0ab2f28cdc7ba0b-189|<tuple|?|?>>
    <associate|p-rc_e0ab2f28cdc7ba0b-190|<tuple|?|?>>
    <associate|p-rc_e0ab2f28cdc7ba0b-191|<tuple|?|?>>
  </collection>
</references>