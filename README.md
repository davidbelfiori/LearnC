
# 💻 LearnC — Playground per Sistemi Operativi

Benvenuti nel repository **LearnC**! Questa repository nasce come **playground di esercitazione e supporto per l'esame di Sistemi Operativi** [cite: 1]. 

L'obiettivo principale è fornire un punto di partenza pratico per comprendere sia le basi della programmazione di sistema in C, sia le dinamiche di risoluzione delle tracce d'esame [cite: 1].

---

## 📌 Struttura del Repository

Il repository è organizzato in diverse sezioni e cartelle tematiche per facilitare lo studio e la navigazione:

### 1. 📚 Esercizi Tematici Basici
In questa sezione sono presenti esercizi introduttivi volti ad apprendere e consolidare i macroargomenti fondamentali del corso [cite: 1]:
- **`EsProcessi/`**: Gestione dei processi, chiamate di sistema (`fork`, `exec`, `wait`), gerarchie e alberi dei processi [cite: 1].
- **`EsThreads/`**: Programmazione multithread tramite libreria **POSIX Threads** (`pthread_create`, `pthread_join`) [cite: 1].
- **`EsPipe/`**: Comunicazione tra processi tramite pipe anonime, FIFO (named pipe) e Message Queues [cite: 1].
- **`EsSincronizzzzione/`**: Problemi classici di sincronizzazione (Produttore/Consumatore, Sezioni Critiche, Mutex, Semafori) [cite: 1].
- **`mmap/`**: Gestione e mappatura della memoria tramite `mmap` e manipolazione dei file [cite: 1].
- **`Tutoraggio/` & Concetti di base**: Puntatori, gestione delle stringhe (`strtok`), struct, `scanf` e basi del linguaggio C applicato ai sistemi operativi [cite: 1].

### 2. 📝 Esercizi d'Esame Risolti (`EsEsame/`)
La cartella `EsEsame/` (e le relative sottocartelle come `rifatti/`) contiene una raccolta di soluzioni a varie prove d'esame e appelli passati [cite: 1].
- Troverai implementazioni complete di tracce svolte per allenarti nella scrittura del codice sotto simulazione d'esame [cite: 1].

---

## ⚠️ Disclaimer Importante

> **ATTENZIONE:** Il materiale contenuto in questo repository è frutto di esercitazioni personali. **Non è assicurata la correttezza richiesta dal professore per la risoluzione degli esercizi [cite: 1].** 
> 
> Alcune soluzioni potrebbero contenere imprecisioni, approcci alternativi o errori di sincronizzazione/gestione delle risorse. Si consiglia vivamente di utilizzare questo codice come **spunto di confronto** e non come soluzione ufficiale assoluta [cite: 1].

---

## 🛠️ Requisiti e Compilazione

Per compilare ed eseguire i programmi in un ambiente Linux/POSIX:

```bash
# Compilazione standard di un file C
gcc -Wall -o programma nome_file.c

# Per programmi che utilizzano i thread (pthread)
gcc -Wall -o programma nome_file.c -lpthread
```

Buono studio e buono sviluppo! 🚀
README.md
Visualizzazione di README.md.
