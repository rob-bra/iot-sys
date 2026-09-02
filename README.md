# IoT Monitoring System

Sistema di monitoraggio IoT basato su board **STEVAL-NBIOTV1** e backend **Spring Boot**, progettato per acquisire, trasmettere, memorizzare e visualizzare dati ambientali tramite dashboard web.

## Descrizione del progetto

Il progetto realizza un'architettura **client-server distribuita** in ambito IoT:

- **Nodo embedded**: board **STEVAL-NBIOTV1** con firmware in **C**
- **Nodo server**: **Raspberry Pi 5** con backend **Java Spring Boot**
- **Dashboard web**: interfaccia browser per consultazione dati, soglie, allarmi e comandi remoti

La board acquisisce periodicamente i valori dei sensori onboard, come:

- temperatura
- umidità
- pressione
- stato batteria
- orientamento

e li invia al backend tramite comunicazione **HTTP** su rete **NB-IoT**.

Il backend:

- riceve le telemetrie
- le salva in un database **SQLite**
- genera eventuali allarmi in caso di superamento soglie
- espone le API REST
- ospita una dashboard web per la visualizzazione e il controllo del sistema

---

## Funzionalità principali

### Firmware embedded
- acquisizione periodica dei dati ambientali
- invio telemetria al backend via HTTP
- polling dei comandi remoti pendenti
- esecuzione del comando `FORCE_MEASUREMENT`
- invio acknowledgement dei comandi eseguiti
- gestione interna tramite **task FreeRTOS**, **coda eventi** e **macchina a stati**

### Backend Spring Boot
- ricezione telemetria via endpoint REST
- persistenza dati in SQLite
- gestione dei dispositivi
- configurazione soglie
- generazione allarmi
- gestione comandi remoti
- dashboard web con Thymeleaf
- calcolo della **media mobile** per la visualizzazione dei trend storici

### Dashboard web
- visualizzazione elenco dei dispositivi
- dettaglio del singolo dispositivo
- consultazione storico misure
- visualizzazione allarmi
- configurazione soglie
- invio comandi remoti
- richiesta rapida di misura immediata
- grafico con **Chart.js** per mostrare dati originali e media mobile

---

## Architettura

Il sistema è organizzato secondo una **Layered Architecture** lato backend:

- **Controller Layer**
- **Service Layer**
- **Repository Layer**
- **DTO / View Layer**

Il firmware sulla board è invece strutturato in task concorrenti e logica a eventi.

---

## Tecnologie utilizzate

### Embedded
- **C**
- **STM32CubeIDE**
- **FreeRTOS**
- **ST87EC Lib**
- **STEVAL-NBIOTV1**

### Backend
- **Java**
- **Spring Boot**
- **Maven**
- **SQLite**
- **JDBC**

### Frontend / Dashboard
- **HTML**
- **CSS**
- **JavaScript**
- **Thymeleaf**
- **Chart.js**

### Test e supporto
- **Postman**
- **JUnit**
- **Mockito**
- **MockMvc**
- **CodeMR**
- **Git / GitHub**

---

## Installazione e uso

### Avvio backend e accesso da web browser

Il backend **Spring Boot** può essere avviato mediante l'esecuzione del file `jar` generato con **Maven**. In particolare:

- nella directory del progetto **Java** in cui è presente il file `pom.xml`, si genera il file **jar** tramite il comando:

    ```bash
    mvn clean package

- sul backend, invece, si va ad eseguire il **jar** file da terminale che avvierà l’applicazione **Spring Boot** mostrando 
  in tempo reale l’esito e le informazioni di qualsiasi interazione col sistema. Il comando da terminale per farlo:

    ```bash
    java-jar iot-sys.jar

- per accedere alla web dashboard invece è sufficiente aprire il browser e navigare al seguente indirizzo locale in ascolto 
  sulla porta **8080**:

    ```bash
    http://localhost:8080

---

## Struttura del progetto

```text
iot-sys/
├── Firmware/                   # Firmware C per STEVAL-NBIOTV1
├── Test/                       # Screen dei test effettuati con CodeMR e JUnit
├── iot/                        # Backend Spring Boot                   
│   ├── src/main/java/
│   ├── src/main/resources/
│   └── pom.xml
└── README.md

---

