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
- espone API REST
- ospita una dashboard web per la visualizzazione e il controllo del sistema

---

## Funzionalità principali

### Firmware embedded
- acquisizione periodica dei dati ambientali
- invio telemetria al backend via HTTP
- polling dei comandi remoti pendenti
- esecuzione del comando `FORCE_MEASUREMENT`
- invio acknowledgement dei comandi eseguiti
- gestione interna tramite **task FreeRTOS**, **coda eventi**, **event buffer** e **macchina a stati HTTP**

### Backend Spring Boot
- ricezione telemetria via endpoint REST
- persistenza dati in SQLite
- gestione dispositivi
- configurazione soglie
- generazione allarmi
- gestione comandi remoti
- dashboard web con Thymeleaf
- calcolo della **media mobile** per la visualizzazione dei trend storici

### Dashboard web
- visualizzazione elenco dispositivi
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

## Struttura del progetto

```text
iot-monitoring-system/
├── firmware/                  # Firmware C per STEVAL-NBIOTV1
├── backend/                   # Backend Spring Boot
│   ├── src/main/java/
│   ├── src/main/resources/
│   └── pom.xml
├── docs/                      # Documentazione, diagrammi UML, PDF progetto
└── README.md