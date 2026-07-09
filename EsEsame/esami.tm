<TeXmacs|2.1.5>

<style|<tuple|generic|italian>>

<\body>
  <section|Compito<nbsp>2022-09-08>

  <\question>
    Si descriva lo scheduler di CPU multi-level feedback-queue.
  </question>

  lo scheduler MLFQ introduce il concetto di code di priorità, vengono
  istituite n code di priorità.\ 

  Al tempo <math|t=0> i processi partono tutti dalla coda ad alta priorità,
  se un processo P non rilascia spontaneamente la cpu ma viene portato fuori
  a seguito di un inteerrupt da timer esso viene declassato di una coda , se
  il processo P' invece prerilascia la coda per andare in I/O , quando
  tornerà ready rimarra nella coda che occupava prima.

  In questo algoritmo vi è preemtion e vi si esegue per un quanto di tempo.

  Questo algoritmo favorisce le applicazzioni I/O bound ma sfavorisce le
  applicazioni cpu bound in quanto mano mano che eseguiranno scenderanno di
  priorità e vi è la possibilta che rimangano li in attesa del turno, in
  sostanza vi è il rischio di starvation.

  Inoltre, si consideri uno scenario in cui all'istamte T0 esistano 2
  processi P1 e P2. Il processo P1 é CPU-bound di durata infinita, mentre il
  processo P2 é I/O-bound ed interagisce infinite volte con un dispositivo D
  prima di completare la sua esecuzione. Il CPU-burst di P2 sia di 5
  millisecondi. Nel caso lo scheduler multi-level feedback-queue abbia 2
  livelli di prioritá, si determini motivando la risposta quale sia il
  massimo valore di time-slice che garantisca un tempo di attesa di P2 non
  superiore a 10 millisecondi. Si assuma che la latenza di esecuzione dello
  scheduler di CPU sia nulla.

  il tempo di attesa di P2 è in funzione del time slice di P1 , essendo P1
  cpu bound necessariamente finira nella 2 coda ed avra time slice
  <math|2<rsup|i> > dove i è la priorità

  quindi avremo che p1 eseguira per un tempo <math|2<rsup|1>\<cdot\>k> e il
  tutto deve essere minore di 10ms

  <\eqnarray*>
    <tformat|<table|<row|<cell|2*k>|<cell|\<leqslant\>>|<cell|10>>|<row|<cell|k>|<cell|=>|<cell|5>>>>
  </eqnarray*>

  <\question>
    Si descriva il metodo di allocazione dei file indicizzato, descrivendone
    vantaggi e svantaggi.
  </question>

  Il metodo di allocazzione indicizzato porta sostanziali modifiche rispetto
  al metodo sequenziale e a catena , il metodo di allocazione indicizzato ,
  maniente in un blocco dati gli indici e gli indirizzi dei blocchi associati
  al nostro file , cosi se il sistema ha bisogno dell'indirizzo di un
  determinato blocco entra nell'indice e ne ricava l'indirizzo , rispetto al
  metodo di allocazione a catena risparmiamo molto tempo per l'accesso al
  singolo blocco non dovendo saltare da un blocco ad un altro fino ad
  arrivare a quello interssato e non vi sono problemi di perdita di dati se
  un blocco di corrompe (nel caso dell'allocazione a catena se un blocco di
  mezzo di corrompe, la catena è interrotta e i dati successivi non
  raggiungibili), i vantaggi dell'allocazione indicizzata sono quindi la
  veloicità con cui possiamo recuperare gli indirizzi dei blcocchi di memoria
  e la possibilità di allocare dinamicamente nuovi blocchi cosa che nel
  metodo sequenziale non potevamo fare , di contro pero dobbiamo tenere conto
  che stiamo andando ad utilizzare un blocco di memoria per mantenere queste
  informazioni nel caso il file divienti di dimensioni importanti l'overhead
  di questa soluzione potrebbe non essere ottimale, si introduce quindi una
  miglioria attravero l'uso del metodo di allocazzione a indicizazzione
  multipla, dove si usa il contetto di i node in unix ovvero dato un file si
  salvano in un inode dove oltre i metadati (uid guid sticky suid sguid e le
  acl) vi sono dei blocchi alcuno con indirizzamento diretto verso un blocchi
  e altri con indirizzamento verso un blocco di indici o vero un indice di
  indici (indizizazzione di secondo livello ).

  \;

  \;

  \;

  \;

  <\question>
    Si descrivano il partizionamento dinamico della memoria e le relative
    tecniche di gestione.
  </question>

  Il partizionamento dinamico rispetto al partizionamento statico non ha una
  taglia fissa e cio ci evita la frammentazzione interna, poiche nel
  partizionamento statico un processo può non occupare tutta la memoria che
  abbiamo scelto, cio crea buchi inutilizzati di memoria.

  con l'uso del partizionamento dinamico è si vero che non soffriamo di
  frammentazione interma ma si soffre comunque di frammentazzione esterna
  poiche al tempo t=0 fino ad un tempo x avremo i nostri dati dei processi
  tutti attaccati , con il passare del tempo alcuni potranno termianre o
  essere swappati dalla memoria lasciando un buco dietro di loro , cio si
  dice frammentazione esterna poiche potremmo trovarci con tanti buchi di
  memoria in cui non riusciamo a inserire nuove informazioni.\ 

  I sistenmi operativi utilizzano un sistema di ricompattazione della memoria
  , però questa operazione è molto dispendiosa sia a livello computazionale
  sia a livello di risorse, si utilizzano pero tecniche per posizionare nel
  miglior modo un nuovo blocco di dati:\ 

  \;

  <with|font-series|bold|First-fit:> Scorre la lista e alloca nel
  <with|font-shape|italic|primo> buco abbastanza grande. È il più veloce.

  <with|font-series|bold|Best-fit:> Scorre l'intera lista e sceglie il buco
  la cui dimensione è <with|font-shape|italic|più vicina> a quella richiesta
  (lascia il frammento residuo più piccolo). È lento e genera micro-buchi
  inutilizzabili.

  <with|font-series|bold|Worst-fit:> Sceglie il buco
  <with|font-shape|italic|più grande> a disposizione, con l'obiettivo di
  lasciare un frammento residuo abbastanza grande da poter essere
  riutilizzato da altri processi."

  \ 

  <\question>
    Si consideri un insieme di N processi {P1, P2, P3, ..., PN} ed un
    ulteriore processo PROD. Il processo PROD periodicamente attende che
    almeno N/2 processi Pi generici siano pronti a leggere un nuovo
    messaggio, e deposita tale nuovo messaggio su una memoria condivisa M,
    abilitando la lettura esattamente per N/2 processi Pi. Ogni processo Pi
    periodicamente vorrá acquisire un nuovo messaggio di PROD, e rimarrá in
    attesa fino a che non sia riuscito a leggere tale nuovo messaggio. Si
    schematizzi la soluzione del suddetto problema di sincronizzazione,
    usando solo semafori, fornendo lo pseudo-codice delle procedure LEGGI
    usata dai processi Pi e SCRIVI usata dal processo PROD.
  </question>

  <\code>
    sem_t messaggioPronto //init = 0;

    sem_t lettoriPronti //init = 0 deve arrivare a N/2//

    sem_t lettoreLetto //init = 0

    \;

    proce Leggi:

    signal(lettorePronti,1)

    wait(messaggioPronto,1);

    msg=M;

    signal(lettoreLetto,1);

    return(msg)

    \;

    procedura Scrivi(msg):

    wait(lettoriPronto,N/2);

    M=msg;

    signal(MessaggioPornto,N/2);

    wait(lettoreLetto,N/2);

    \;
  </code>

  <new-page>

  <section*|Esame 26/01/2023>

  <\question>
    Si descriva lo scheduler di CPU dei sistemi operativi Windows, fornendo
    anche indicazioni sulle system-call che possono essere utilizzate per la
    gestione delle priorità di processi e thread.
  </question>

  Lo scheduler windows è nativamente multi thread , è basato sull'aglritmo
  delle multi level feedback queque , ha 32 livelli di priorità divisi in
  16-32 Real time e 0-15 Variable.

  Esiste il concetto di cambiare coda, ma solo nella sezione variable, ovvero
  un processo che si torva nella classe variable , se rilascia sporanemtene
  la CPU riparte dal livello massimo , se viene rilasciato dalla cpu a
  seguito di un interrupt da timer viene declassato di coda.

  Nella classe real time al contario non posso cambiare coda. Ogni processo
  esegue per un quanto di tempo specifico. Per ogni coda di priorità i
  processi eseguono seguendo l'algoritmo round robin.

  \;

  le syscall per modificare la priorita sono setPriorityClass e
  setThreadPriority.

  <\question>
    Si descriva il metodo di allocazione dei file indicizzato, descrivendone
    vantaggi e svantaggi. Fornire anche una descrizione del classico metodo
    di indicizzazione utilizzato sui file system Unix.
  </question>

  Il metodo di allocazzione<space|1em>porta sostanziali modifiche rispetto ai
  metodi di allocazzione sequenziale o a catena, il metodo indicizzato ,
  tiene in un blocco dati un indice dove come un indice di un libro si tiene
  il nome del capitolo e il suo indirizzo di memoria, le migliorie rispetto
  all'allocazione sequenziale sono: allocazzione dinamica dei blocchi senza
  doverli preallocare andando in contro ad una frammentazione interna (in
  quanto preallocando i bloccjo , vi è la possibilta che non usi tutto lo
  spazio a me riservato) e rispetto ad un allocazione a catena le migliorie
  stanno nella velocità di accesso alle informazioni in quanto , nel metodo a
  catena devo saltare da un blocco all'altro fino a trovare il blocco che mi
  serve e vi sono problemi nel caso in cui un blocco venga corrotto, la
  catena si spezza e parte delle informazioni vengono perse.\ 

  Da un lato il metodo di allocazzione indicizzato , ci fa risparmiare tempo
  rieptto alla catena e rispetto al mentodo di allocazzione contigua posso
  aggiungere blocchi dinamicamente senza rischiare la frammentazione interna
  , dall'altro lato pero questo metodo ha un overhead importante anche per
  file piccoli , poiche anche per un piccolo file dobbiamo riservare un
  blocco per l'indice e per file grandi dobbiamo mantenere un indice
  importante.\ 

  La soluzione che porta Unix è un sistema basato sugli i-node , un inode
  contiene in una parte i metadati dell'file (uid guid suid sguid sticky bit
  e acl ) tutte informazioni volte alla sicurezza d'accesso dell'applicazione
  , poi vi cono 10 blocchi di inridizzamento diretto ad un blocco dati e i
  restanti tre sono blocchi verso un indicie di primo secondo e terzo
  livello, questa soluzione arriva a contenere file di circa 1GB un
  evoluzione è il sistema ext4 che utilizza delle extent ovvero puntatori a
  dei blocchi contigui in memoria e puntatori a indici di alte exent

  <\question>
    Si descriva l'algoritmo dell'orologio per la selezione della vittima in
    sistemi di memoria virtuale basati su paginazione.
  </question>

  L'algoritmo dell'orologio per la selezione della vittima è basato du due
  lancette che scorrono a distanza costante. Per ogni pagina si aggiunge un
  bit , detto referece bit , che senza se la pagina è referenziata da un
  processo in lettura o scrittura , la prima lancetta quella del reset ,
  ciclicamente passa su ogni casella e resetta il bit , nel momento in cui
  viene rischiesta una pagina la lancetta di selezione seglie come vittima la
  prima pagina ad avere il reference bit impostato a 0. Nei sistemi mono
  processore è sempre possibile trovare una vittima , nei sistemi multi
  processore potrebbe ssere necessario fare più giri dell'olrologgio prima di
  trovarne una.

  Una miglioria apportata a questo algoritmo è stata fatta aggiungegdo un
  altro bit , un dirty bit , che ci sa l'informazione se la pagina è stata
  modificata o meno , questo particolare ci fa risparmiare molto tempo poichè
  è preferibile prendere come vittima una pagina non referenziata al momento
  e che non è stata modificata , cosi da doverla sovrascriverla e basta senza
  doverla salvare in memoria il suo aggiornamento, il funzionamento
  dell'algoritmo è il medesimo solo che vi sono pagine con caratteristiche
  preferite per essere scelte come vittima, come detto precedentemente.

  L'algoritmo dell'orologio soffre dell'anomalia di belady , ovvero che
  all'aumentare del numero di frame disponibili in memoria la frequenza di
  major page fault non diminuisce anche aumenta , gli algoritmi che non
  soffrono di questa anomalia sono detti algoritmi a stack , come l'algoritmo
  ottimo o l'agoritmo lru. \ 

  <\question>
    Si consideri un insieme di N processi {P1, P2, \<ldots\> , PN} e due
    ulteriori processi PROD1 e PROD2. Ogni processo Pi periodicamente vorrá
    acquisire un nuovo messaggio di PROD1 oppure di PROD2 leggendolo da una
    memoria condivisa R, e rimarrá in attesa fino a che non sia riuscito a
    leggere tale nuovo messaggio. I processi PROD1 e PROD2 periodicamente
    attendono che tutti gli N processi Pi generici siano pronti a leggere un
    nuovo messaggio. Questo abilita la consegna del nuovo messaggio sulla
    memoria condivisa R solo da parte di uno dei due processi PROD1 e PROD2.
    In ogni caso, per ogni coppia di nuovi messaggi consegnati ai processi
    Pi, la consegna di uno (non necessariamente il primo) dei messaggi deve
    essere effettuata da PROD1 e dell'altro dei messagi da PROD2. Si
    schematizzi la soluzione del suddetto problema di sincronizzazione,
    usando solo semafori, fornendo lo pseudo-codice delle procedure LEGGI
    usata dai processi Pi e SCRIVI usata dai processi PROD1 e PROD2.
  </question>

  <\code>
    sem_t readers //init = 0

    sem_t turno[2] //init {1,0}

    sem_t message // init = 0;

    sem_t readerRead\ 

    \;

    proc SCRIVI(i,msg):

    wait(turno[i],1)

    wait(readers,N);

    M=msg

    signal(message,N);

    wait(readerRead,N)

    signal(turno(i+1)mod2,1)

    \;

    proc Leggi(msg):

    signal(reader,1)

    wait(message,1)

    risposta = M

    signal(readerRead ,1)

    return risposta;
  </code>

  <subsubsection*|Esame 21 06 2021>

  <\question>
    Si descriva lo scheduler di CPU Round-Robin Virtuale, evidenziandone i
    vantaggi e gli svantaggi.
  </question>

  Il Virtual Round-Robin (VRR) introduce una variante al Round-Robin (RR)
  classico per evitare la penalizzazione dei processi I/O-bound. Nel RR
  classico, se un processo si blocca per I/O dopo aver usato solo una
  frazione del suo quanto \<tau\>, quando si sblocca torna in fondo alla
  coda, perdendo il tempo non utilizzato.

  Il VRR risolve questo problema introducendo una <with|font-series|bold|coda
  ausiliaria ad alta priorità>. Quando un processo si blocca per I/O, il
  sistema memorizza il tempo CPU già utilizzato t. Al termine dell'I/O, il
  processo entra nella coda ausiliaria. Questa coda ha priorità assoluta
  sulla coda standard. Quando viene eseguito, il processo riceve un quanto
  limitato, pari al suo <with|font-series|bold|quanto residuo>
  <math|\<tau\>-t>. Se consuma tutto il residuo, viene rimesso nella coda
  standard con quanto pieno <math|\<tau\>>.

  <with|font-series|bold|Vantaggi:> Garantisce un trattamento equo (fairness)
  per i processi interattivi/IO-bound, migliorando i tempi di risposta del
  sistema e l'efficienza nell'uso delle periferiche di I/O. Riduce il rischio
  di starvation dei processi CPU-bound rispetto ai sistemi a priorità pura,
  poiché i processi nella coda ausiliaria possono consumare solo frazioni di
  quanto.

  <with|font-series|bold|Svantaggi:> Introduce un overhead maggiore per il
  Kernel, che deve cronometrare accuratamente il tempo di esecuzione di ogni
  processo e calcolare dinamicamente i quanti residui, oltre a dover gestire
  una struttura a code multiple.

  <\question>
    Descrivere le caratteristiche salienti del virtual-file-system Unix.
  </question>

  Il virtrual file system fornisce un astrazione dello strato sottostante e
  ci permette attraverso delle syscall generiche di poter interfacciarci con
  dispositivi I/O diversi, come con file system , socket e pipe.

  in unix i dispositivi hardware e le periferiche vengono gestite come se
  fossero dei file regolari. ogni risorsa è rappresentata attravero un i-node
  virtuale.\ 

  In unix possiamo interfacciarci con un dispositivo attraverso delle
  operazione di riferimento che operano sendono due metodi:\ 

  <\itemize>
    <item>stream I/O si comunica attraverso uno stream di byte di lunghuzza
    arbitraria

    <item>Block I/O si comunica attravero blocchi di taglia fissa\ 
  </itemize>

  Quando si ha un canale come un file o una pipe e si opera su di essi
  attravero syscall generiche come read, al momento dell'esecuzione della
  syscall succede quanto segue: dato il descrittore passato nella syscall il
  sistema operativo va nel nostro pcb e recupera l'indirizzo della notra
  tabella dei descrittori , dentro alla tabella dei descrittori con il
  descrittore recupera l'indirizzo di memoria della tabella inetemedia , che
  a sua volta ha due entry una verso la cache i-node e una verso \ il driver
  di quello specifico oggetto in questo caso una pipe.

  Il buffer cache è una zona di memoria dove temporaeamente salviamo parti
  del file richiesto , essendo una zona molto piccola , vi sono tecniche per
  pulirla come lru o come il metodo di buffer cache a più sezioni. \ 

  Il \ file system unix è diviso in due porzioni una riservata ai blocchi
  dati e una per manterene i record di distema dei file , che in unix sono
  chiamati i-node.

  un i-node è una struttura dati contenente i metadati del file e gli
  indirizzi ai blocchi di memoria.

  l'i-node non associa tiene pero il nome del file , il nome del file viene
  associato nelle directory con l'i-node, una directory è una file speciale ,
  contenente il nome del file e il puntatore all'i-node, un hard link.

  Dentro un i-node appunto vengono memorizzati i metadati del file come:

  <\itemize>
    <item>Tipo di i-node : specifica se si tratta di un file diretory o di un
    file o di una namedPipe\ 

    <item>uid e guid: ovvero il codice del proprietario e il gruppo del
    proprietario, non sono username , l'associazione tra uid e guid ad un
    username viene fatta in /etc/passwd

    <item>suid e sgid : per prendere temporanamente i privileggi del
    propeitario o del gruppo del prorietario.

    <item>permessi per propeitario gruppo e altri , esiste una tripletta per
    ogniuno di questi ovvero specifica quale operazioni possono fare tra read
    write execute.

    <item>sticky bit: ci da l'informazione nel caso fosse una directory se
    solo il prorpietario può rinominare o cancellare quella directory
  </itemize>

  oltre ai metadati in un i-node sono presenti i puntatori ai blocchi di dati
  , si hanno 13 indici, 10 ad indirizzamento diretto ovvero che puntanto
  direttamente ad un blocco , l'11 esimo punta ad un blocco di indici
  (indicizzazzione di primo livello) e il 12 esimo e 13 esimo formano
  l'indicizzazzione di sencondo e terzo livello. Questa modalita di
  indirizzamento ci offre la possibilità di immagazzinare al massimo un file
  da 1 GB , che nell'era moderna è poco , quindi è stata fatta una miglioria
  con l'uso delle extent e di ext 4 , l'extent utilizza una coppia di valori
  numero di blocchi: indirizzo di partenza. in un i-node ora conterra indici
  diretti ad un extent o un indice diretto ad un albero di extent.

  Per controllare meglio li accessi ad un i-node si usano le acl , con il
  loro uso ci peremette di specificare a grana fine i permessi di accesso e
  di gestione dei file . un acl è realizzata grazie ad un file shadow
  associato al file originale , quindi anche ad un i-node shadow , con le acl
  posso specificare per ogni file i permessi di accesso per ogni singolo
  utente o gruppo del sistema.

  <\question>
    Descrivere l'algoritmo LRU (Least-Recently-Used) per la sostituzione
    delle pagine in ambiente di memoria virtuale. Si consideri inoltre una
    memoria di lavoro di 4 frame e la seguente sequenza di accessi a pagine
    logiche: 1 2 3 4 5 7 8 5 5 2 1 4 6 3; si determini il numero di
    page-fault nel caso di utilizzo dell'algoritmo LRU. Si indichi infine se
    tale algoritmo soffra o meno dell'anomalia di Belady, motivando la
    risposta.
  </question>

  L'agoritmo di selezione della vittima LRU va visto un po come una pila dove
  noi richiediamo delle pagine e le impileremo in questa pila , avremo un
  blocco iniziale in alto che abbiamo appena messo e un blocco finale che è
  quello prossimo ad essere scelto come vittima , per esempio abbiamo 4 frame
  di cui tutte occupate , al momento t viene richiesta l'ultima pagina della
  pila , quella che era l'ultima della pila ora si troverà in cima alla pila,
  la vittima sarà la pagina che si troverà in fondo alla pila , poiche sarà
  la pagina che in quell'arco di tempo non è stata richiesta e si presuppone
  che non essendo richiesta in quel tempo non ci servirà per la località
  temporale.

  \;

  esercizio fatto sull'ipad 12 page fault in 14 operazioni\ 

  L'algoritmo lru insieme all'algoritmo ottimo ( che pero non è
  implementabile in quanto guarda nel futuro per scegliere come vittima la
  pagina che gli servirà più avanti nel futuro) non soffre dell'anomalia di
  belady , si verifica l'anomalia di belady nel caso in cui data una
  frequenza di page fault con determinate pagine , all'aumentare del numero
  di frames disponibili la frequenza non scende ma al contrario aumenta , gli
  algoritmi che non ne soffrono sono detti algoritmi a stack.\ 

  Un algoritmo che soffre dell'anomalia di belady è per esempio fifo ,
  all'aumentare del numero dei frame il numero di page fault aumenta.

  <subsubsection*|stesso giorno ma da 9 cfu>

  <\question>
    Si descriva lo scheduler di CPU Windows, evidenziando anche le
    system-call utili per la gestione della sua configurazione.
  </question>

  Lo scheduler di Windows è uno scheduler <strong|preemptive a priorità> che
  schedula i <strong|thread> e non direttamente i processi. Le priorità sono
  suddivise in <strong|32 livelli (0-31)>: i livelli <strong|16-31>
  appartengono alla classe <strong|Real-Time>, mentre i livelli <strong|0-15>
  sono destinati ai thread a priorità variabile.

  Lo scheduler seleziona sempre un thread appartenente alla coda ready con
  priorità più alta. Tra thread aventi la stessa priorità viene utilizzato un
  algoritmo <strong|Round Robin> con un quanto di tempo (time quantum). Se
  durante l'esecuzione scade il quanto, il thread viene prerotato e un altro
  thread della stessa priorità viene eseguito.

  Le principali system call utilizzate per configurare le priorità sono
  <strong|<code*|SetPriorityClass()>> e <strong|<code*|GetPriorityClass()>>
  per modificare o leggere la classe di priorità di un processo, e
  <strong|<code*|SetThreadPriority()>> e <strong|<code*|GetThreadPriority()>>
  per modificare o leggere la priorità di un thread.

  \ 

  <\question>
    Descrivere il metodo di accesso sequenziale. Inoltre, dato un file system
    con metodo di accesso sequenziale e metodo di allocazione contigua, il
    cui dispositivo di memoria di massa abbia blocchi di taglia pari a 1 M
    record e tempo di accesso al blocco pari a 10 millisecondi, si determini
    il tempo massimo di accesso ad un qualsiasi record di un file sequenziale
    F costituito da 3,5 M record. Si assuma che il record di sistema che
    tiene traccia dell'esistenza del file F sia a sua volta caricabile in
    memoria tramite il caricamento di un unico blocco di dispositivo, che il
    costo per le operazioni a livello del software per la gestione
    dell'accesso al file F sia trascurabile, così come sia trascurabile il
    costo per la gestione degli interrupt.
  </question>

  i record di un file vengono acceduti sequenzaialmente, l'indice di
  scrittura o lettura avanza di un unita per ogni record acceduto , l'indice
  può essere riposizionato solo all'inizio del file. Quando si accede con
  metodo sequenziale e si deve prendre un informazione a meta del file ,
  dobbiamo scandire tutto il file , questo problema viene risolto con il
  metodo di accesso indicizzato e diretto.ù

  dato il file da 3.5 M di record e ogni blocco ha 1M record\ 

  <\eqnarray*>
    <tformat|<table|<row|<cell|numero blocchi N
    >|<cell|=>|<cell|<frac|3.5M|1M> \<simeq\>4 Blocchi>>>>
  </eqnarray*>

  <math|tempo di accesso al blocco di sistema per il file>

  <\eqnarray*>
    <tformat|<table|<row|<cell|t<rsub|file>>|<cell|=>|<cell|10ms>>>>
  </eqnarray*>

  tempo di accesso ai blocchi

  <\eqnarray*>
    <tformat|<table|<row|<cell|t<rsub|blocchi>>|<cell|=>|<cell|4\<cdot\>10ms>>|<row|<cell|>|<cell|=>|<cell|40ms>>|<row|<cell|t<rsub|tot>>|<cell|=>|<cell|t<rsub|file>+t<rsub|blocchi>>>|<row|<cell|>|<cell|=>|<cell|50
    ms>>>>
  </eqnarray*>

  <\question>
    Descrivere la tecnica della segmentazione.
  </question>

  dato un address space esso è fatto da oggetti diversi ovvero da segmenti
  distinti (text data bss heap e stack), gli indirizzi logici sono dati
  dall'indirizo di segmento e da un offset per lo spiazzamento interno al
  secmento.

  Possono essere caricati in memoria in posizioni non contigue, basta avere
  in ausiolio una tabella dei descrittori dei segmenti che ci dice
  l'indirizzo base e un limite , il limite ci serve dato un indirizzo a
  controllare se l'indirizzo richiesto ricade nel segmento richiesto. Quando
  con un indirizzo logico andiamo nella tabella dei descrittori dei segmenti
  , andiamo a verificare che l'offset dato sia minore o uguale del limite
  scritto nella tabella , se vero si fa la somma della base all'offset , se
  falso si torna un trap al so.

  <\question>
    Si descriva lo scheduler di CPU multi-level-feedback-queue. Inoltre,
    supponendo che il quanto di tempo base di tale scheduler sia pari a 1
    millisecondo e che al tempo T = 0 siano creati in ordine 2 processi P1 e
    P2 tali che P1 abbia un solo CPU-burst di durata pari a 10 millisecondi,
    mentre P2 abbia due differenti CPU-burst di durata pari a 2 e 4
    millisecondi, si determini il tempo di completatmento di P2 considerando
    il caso in cui lo scheduler abbia 3 differenti livelli di priorità e tra
    i suoi due CPU-burst P2 rimanga in stato \Pwait\Q per 0.5 millisecondi.
    Si assuma che la latenza per eseguire attività di sistema operativo sia
    trascurabile.
  </question>

  Lo scheduler multi level feedback queue dal nome è fatta da n code a
  priorità diversa, i processi vengono ammesso al tempo iniziale al livello
  massimo , possono cambiare coda , dato un processo che non rilascia
  spotaneamente la cpu ma viene interrotto da un interrupt da timer , alla
  prossima esecuzione si troverà nella coda un livello sottostante , al
  contrario un processo che rilascia spotaneamente la cpu rimane nella stessa
  coda. i processi eseguono per un quanto di tempo , per evitare la
  starvation si utilizza che il quanto di tempo e pari a
  <math|2<rsup|i>*\<cdot\>\<tau\>> dove i è il livello di priorità. quindi
  più è alta la priorità meno tempo resterà in esecuzione.

  <\question>
    Descrivere i metodi di protezione dei file sui file system Unix,
    considerando anche la variante basata su ACL (Access Control List).
  </question>

  Nei sistemi operativi Unix, la sicurezza e la protezione delle risorse sono
  gestite direttamente dal kernel, il quale controlla rigorosamente l'accesso
  a ciascun file. Ogni processo attivo sulla macchina porta con sé un insieme
  di identificatori numerici di utenza e di gruppo, sia reali che effettivi
  (UID ed eUID, GID ed eGID). Quando un processo tenta di compiere
  un'operazione di lettura, scrittura o esecuzione su un file, il kernel
  incrocia gli identificatori del processo con le regole di protezione
  associate al file stesso per determinare se consentire o negare
  l'operazione.

  I metodi di protezione supportati si dividono principalmente nel modello
  tradizionale basato sui bit di permesso POSIX e nell'estensione più
  granulare fornita dalle Access Control List (ACL).

  <hrule>

  1. Il Metodo Tradizionale: Bit di Permesso POSIX

  Nel file system Unix, ogni file e directory è rappresentato internamente da
  una struttura dati chiamata <strong|i-node>. Gli attributi di protezione
  fondamentali sono registrati direttamente all'interno dei metadati di
  questo i-node:

  <\itemize>
    <item><strong|Identificatori di Proprietà (UID e GID)>: indicano l'utente
    proprietario del file e il gruppo di appartenenza. L'associazione tra
    l'ID numerico memorizzato sul file system e il nome testuale dell'utente
    (username) non è memorizzata nell'i-node, ma è gestita in user-space
    tramite file di mappatura del sistema come <code*|/etc/passwd> e
    <code*|/etc/group>.

    <item><strong|La tripletta rwx (rwxrwxrwx)>: nove bit di permesso che
    definiscono le tre operazioni base per tre distinte classi di utenti:

    <\itemize>
      <item><strong|Lettura (r)>: consente di visualizzare il contenuto del
      file.

      <item><strong|Scrittura (w)>: consente di modificare o troncare il
      contenuto del file.

      <item><strong|Esecuzione (x)>: consente di avviare il file se si tratta
      di un programma o di un eseguibile binario.

      <item>Queste tre operazioni sono ripartite rispettivamente per il
      <strong|Proprietario> (owner), il <strong|Gruppo> (group) e gli
      <strong|Altri> utenti del sistema (others). In fase di programmazione o
      creazione del file, tali permessi vengono solitamente espressi mediante
      codifica ottale (ad esempio <code*|0666> per concedere lettura e
      scrittura a tutti).
    </itemize>
  </itemize>

  Permessi Speciali e Bit di Stato

  Unix prevede inoltre tre bit speciali associati all'i-node per contesti
  d'uso specifici:

  <\enumerate>
    <item><strong|SUID (Set User ID)>: applicato ai file eseguibili, fa sì
    che chiunque esegua il programma lo faccia ereditando temporaneamente i
    privilegi del proprietario del file e non del chiamante. Un classico
    esempio è il comando <code*|passwd> (di proprietà di <em|root>), che
    necessita di scrivere temporaneamente nel file protetto
    <code*|/etc/shadow> per cambiare la password dell'utente.

    <item><strong|SGID (Set Group ID)>: funziona in modo analogo al SUID, ma
    applica temporaneamente i privilegi del gruppo proprietario del file. Se
    applicato a una directory, fa sì che ogni nuovo file creato al suo
    interno erediti automaticamente il gruppo della cartella stessa,
    facilitando il lavoro collaborativo.

    <item><strong|Sticky Bit>: applicato storicamente alle directory
    condivise (come <code*|/tmp>), impedisce la cancellazione o la
    ridenominazione di un file a chiunque non sia l'effettivo proprietario
    (owner) del file stesso, anche se la cartella concede permessi di
    scrittura globali.
  </enumerate>

  Questi permessi possono essere modificati dai programmi utente tramite le
  chiamate di sistema <code*|chmod()> (o <code*|fchmod()> se si opera tramite
  descrittore di file aperto), mentre la proprietà è gestita tramite la
  famiglia di system call <code*|chown()>.

  <hrule>

  2. La Variante Basata su ACL (Access Control List)

  La protezione tradizionale basata solo su proprietario, gruppo e altri
  mostra evidenti limiti di granularità (ad esempio, non è possibile dare
  permessi di lettura solo a un secondo utente specifico senza includere
  l'intero gruppo o l'intera platea di utenti). Per superare questa
  limitazione, i moderni file system Unix supportano le <strong|Access
  Control List (ACL)>.

  <subsubsection*|Realizzazione Architetturale>

  L'implementazione delle ACL avviene in modo trasparente dal punto di vista
  logico, ma richiede strutture aggiuntive per evitare di appesantire il
  vettore standard degli i-node:

  <\itemize>
    <item>Una ACL viene salvata e gestita all'interno di un <strong|file
    shadow> (e del rispettivo <strong|i-node shadow>) associato al file
    originale.

    <item>L'i-node del file originario memorizza semplicemente un indice di
    riferimento che rimanda al rispettivo i-node shadow.

    <item>Questo permette di definire in modo dinamico e per singolo file un
    elenco dettagliato di permessi d'accesso dedicati a <strong|utenti
    nominativi (named users)> o <strong|gruppi nominativi (named groups)> del
    sistema.
  </itemize>

  <subsubsection*|Il Meccanismo della Maschera (Mask)>

  Nelle ACL viene introdotto il concetto di <strong|maschera (mask)>.

  <\itemize>
    <item>Il campo <em|mask> rappresenta il limite massimo dei permessi che
    possono essere effettivamente concessi agli utenti nominativi e ai gruppi
    (non ha effetto sul proprietario o sulla classe <em|other>).

    <item>Il kernel applica un'operazione di <strong|AND logico> tra i
    permessi specificati per il singolo utente/gruppo nell'ACL e il valore
    impostato nella <em|mask>.

    <item>Se un amministratore deve disattivare rapidamente e temporaneamente
    un permesso sensibile (come la scrittura) per tutti gli utenti e gruppi
    speciali configurati, può farlo modificando unicamente la maschera e
    ponendo a 0 il bit corrispondente, senza dover modificare singolarmente
    ogni record dell'ACL. Per ripristinare i permessi originari, basterà
    rimettere a 1 il bit della maschera.
  </itemize>

  <subsubsection*|Interazione da Shell>

  A livello utente, la visualizzazione e la configurazione di queste liste
  non avvengono tramite i comandi tradizionali, ma fanno affidamento su due
  utility specifiche:

  <\itemize>
    <item><strong|<code*|getfacl>>: interroga il sistema per estrarre e
    mostrare l'ACL completa associata al file.

    <item><strong|<code*|setfacl>>: permette di aggiungere, modificare o
    rimuovere le regole di accesso per utenti e gruppi specifici all'interno
    della lista.
  </itemize>

  <\question>
    Descrivere la tecnica di paginazione a livelli multipli spiegando come
    questa porti vantaggi rispetto alla tecnica di paginazione a livello
    singolo in particolare in sistemi che offrono la memoria virtuale

    \;
  </question>

  La <strong|paginazione a livelli multipli> è una tecnica di gestione della
  memoria virtuale progettata per risolvere il problema dell'enorme quantità
  di memoria fisica (RAM) richiesta per memorizzare le tabelle delle pagine
  (page tables) nei sistemi moderni.

  Man mano che lo spazio di indirizzamento virtuale cresce (passando dai
  sistemi a 32 bit a quelli a 64 bit), una struttura a livello singolo
  diventa rapidamente insostenibile. La paginazione a livelli multipli
  affronta questo problema suddividendo la tabella delle pagine in una
  struttura gerarchica ad albero.

  <hrule>

  <subsection*|1. Il problema della paginazione a livello singolo>

  In un sistema di paginazione a livello singolo, il sistema operativo deve
  mantenere un'unica grande tabella per ogni processo, la quale mappa in modo
  lineare l'intero spazio di indirizzamento virtuale.

  <\itemize>
    <item><strong|Spreco di memoria contigua>: La tabella delle pagine a
    livello singolo deve essere memorizzata in modo contiguo in memoria. Ad
    esempio, in un sistema a 32 bit con pagine da 4 KB, lo spazio di
    indirizzamento contiene $2^{20}$ pagine (circa 1 milione). Se ogni voce
    della tabella (PTE) occupa 4 byte, la tabella di un singolo processo
    richiede <strong|4 MB di RAM contigua>, anche se il processo utilizza
    solo pochissime pagine (ad esempio, solo pochi byte per il codice e lo
    stack).

    <item><strong|Inapplicabilità nei sistemi a 64 bit>: Nei moderni sistemi
    a 64 bit, lo spazio di indirizzamento virtuale è astronomico (fino a 256
    TB indirizzabili con i tipici 48 bit canonici). Una tabella a livello
    singolo richiederebbe miliardi di gigabyte solo per memorizzare la
    struttura di traduzione dei puntatori, rendendo il sistema
    inutilizzabile.
  </itemize>

  <hrule>

  <subsection*|2. Come funziona la paginazione a livelli multipli>

  Invece di mappare direttamente l'indirizzo virtuale in un frame fisico
  tramite un'unica tabella, <strong|la tabella delle pagine viene essa stessa
  divisa in pagine>.

  Un indirizzo lineare (virtuale) viene decomposto dal processore in più
  parti, ognuna delle quali funge da indice per accedere a un determinato
  livello della gerarchia:

  <\enumerate>
    <item><strong|Primo Livello (Directory o Top Level)>: È l'unica tabella
    che deve essere sempre presente in RAM per il processo attivo
    (l'indirizzo della sua base fisica è registrato in un registro speciale
    della CPU, come il registro <code*|CR3> nei processori x86). Le sue voci
    (entry) non puntano ai frame di memoria dei dati, ma ad altre tabelle di
    livello inferiore.

    <item><strong|Livelli Intermedi / Secondo Livello>: Tabelle intermedie
    che puntano a livelli ancora più bassi o direttamente alla tabella
    finale.

    <item><strong|Ultimo Livello (Page Table)>: Contiene l'effettivo
    indirizzo fisico del frame di RAM in cui risiede la pagina dei dati.
  </enumerate>

  <strong|Esempio a 32 bit (2 livelli)>: L'indirizzo virtuale viene diviso in
  tre parti: <code*|P1> (indice per la Directory di primo livello),
  <code*|P2> (indice per la Tabella delle Pagine di secondo livello) e
  <code*|Offset> (lo spiazzamento all'interno della pagina fisica di 4 KB).

  <strong|Esempio x86-64 (4 livelli)>: Nelle moderne architetture x86-64 a 48
  bit canonici, il processore esegue un ``page-walk'' attraverso 4 livelli di
  tabelle, ciascuna composta da 512 entry (ognuna grande 4 KB, occupando
  esattamente una pagina fisica):

  <itemize|<item><strong|PML4> (Page-Map Level-4) $\\rightarrow$
  <strong|PDPT> (Page-Directory Pointer Table) $\\rightarrow$ <strong|PD>
  (Page Directory) $\\rightarrow$ <strong|PT> (Page Table) $\\rightarrow$
  <strong|Offset>.>

  <hrule>

  <subsection*|3. I vantaggi nei sistemi con memoria virtuale>

  L'introduzione della gerarchia a più livelli porta enormi vantaggi,
  specialmente nei sistemi che supportano la memoria virtuale e lo swapping:

  <subsubsection*|A. Allocazione Sparsa (Sparse Allocation) e risparmio
  drastico di RAM>

  La maggior parte dei programmi utilizza solo una frazione minima del
  proprio spazio di indirizzamento virtuale (fenomeno della memoria sparsa,
  con codice in basso, stack in alto e ampi ``vuoti'' in mezzo).

  <\itemize>
    <item>Con i livelli multipli, se un'intera regione di memoria virtuale
    non è mappata o utilizzata dal processo, <strong|la corrispondente entry
    nella tabella di livello superiore viene impostata a <code*|NULL>>.

    <item>Questo significa che <strong|le tabelle delle pagine di livello
    inferiore per quella zona non vengono create né allocate in RAM>. Solo le
    tabelle associate alle porzioni di memoria effettivamente materializzate
    occupano spazio fisico.
  </itemize>

  <subsubsection*|B. Paginabilità delle tabelle delle pagine stesse>

  Poiché le tabelle delle pagine di livello inferiore hanno la stessa
  dimensione di una pagina standard (es. 4 KB), il sistema operativo può
  trattarle come normali pagine di dati.

  <itemize|<item><strong|Swapping delle tabelle>: Se la memoria fisica è
  scarsa, le tabelle di secondo, terzo o quarto livello che non vengono usate
  da molto tempo possono essere <strong|portate fuori RAM (swapped-out)>
  sull'area di swap, riducendo l'overhead di memoria del kernel. Solo la
  tabella di livello più alto (<code*|PML4> o directory principale) deve
  rimanere obbligatoriamente bloccata in RAM.>

  <subsubsection*|C. Semplificazione dell'hardware tramite tabelle a
  dimensione fissa>

  Si potrebbe pensare di risparmiare spazio usando tabelle delle pagine a
  dimensione variabile (che crescono dinamicamente). Tuttavia, gestire
  strutture a dimensione variabile a livello hardware (microcodice della CPU)
  sarebbe estremamente complesso e inefficiente. La paginazione a livelli
  multipli consente di mantenere <strong|tutte le tabelle a dimensione fissa>
  (es. 4 KB, ovvero esattamente una pagina fisica), facilitando la
  progettazione dell'hardware e l'allineamento in memoria.

  <hrule>

  <subsection*|4. Lo svantaggio correlato e la soluzione hardware>

  L'unico vero svantaggio di questa tecnica è il costo computazionale della
  traduzione (overhead): per accedere a un singolo dato in memoria, la CPU
  deve effettuare molteplici accessi intermedi alla RAM per consultare i vari
  livelli di tabelle.

  Questo problema viene risolto in modo estremamente efficiente grazie al
  <strong|TLB (Translation Lookaside Buffer)>, una cache hardware velocissima
  integrata nella CPU che memorizza le associazioni recenti tra pagina
  virtuale e frame fisico, evitando di dover ripetere il ``page-walk'' sui
  diversi livelli per gli accessi successivi.

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
    <associate|auto-1|<tuple|1|1>>
    <associate|auto-10|<tuple|<with|mode|<quote|math>|\<bullet\>>|?>>
    <associate|auto-11|<tuple|<with|mode|<quote|math>|\<bullet\>>|?>>
    <associate|auto-12|<tuple|<with|mode|<quote|math>|\<bullet\>>|?>>
    <associate|auto-13|<tuple|<with|mode|<quote|math>|\<bullet\>>|?>>
    <associate|auto-14|<tuple|<with|mode|<quote|math>|\<bullet\>>|?>>
    <associate|auto-2|<tuple|4|3>>
    <associate|auto-3|<tuple|8|4>>
    <associate|auto-4|<tuple|11|?>>
    <associate|auto-5|<tuple|3|?>>
    <associate|auto-6|<tuple|<with|mode|<quote|math>|\<bullet\>>|?>>
    <associate|auto-7|<tuple|<with|mode|<quote|math>|\<bullet\>>|?>>
    <associate|auto-8|<tuple|17|?>>
    <associate|auto-9|<tuple|<with|mode|<quote|math>|\<bullet\>>|?>>
  </collection>
</references>

<\auxiliary>
  <\collection>
    <\associate|toc>
      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|1<space|2spc>Compito
      <no-break><specific|screen|<resize|<move|<with|color|<quote|#A0A0FF>|->|-0.3em|>|0em||0em|>>2022-09-08>
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-1><vspace|0.5fn>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|Esame
      26/01/2023> <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-2><vspace|0.5fn>

      <with|par-left|<quote|2tab>|Esame 21 06 2021
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-3>>
    </associate>
  </collection>
</auxiliary>