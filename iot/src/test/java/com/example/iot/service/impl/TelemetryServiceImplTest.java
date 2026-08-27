package com.example.iot.service.impl;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.repository.DeviceRepository;
import com.example.iot.repository.TelemetryRepository;
import com.example.iot.service.AlertService;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.List;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.Mockito.*;

public class TelemetryServiceImplTest {

	/* Oggetti usati nelle chiamate di funzione - saranno mock */
    private TelemetryRepository telemetryRepository;
    private DeviceRepository deviceRepository;
    private AlertService alertService;
    // istanza reale della classe da testare
    private TelemetryServiceImpl telemetryService;

    /* @BeforeEach viene chiamata solo una volta all'inizio, prima del @Test
     * es. se vado ad inizializzare una classe qua dentro, e la utilizzo nei test successivi, il
     * valore dell'oggetto verrà condiviso tra tutti i metodi di test che usano l'oggetto: 
     * 
     * telemetryService viene inizializzato e verrà condiviso tra tutti i @Test*/
    @BeforeEach
    void setUp() {
    	/* I mock vanno a creare degli oggetti fittizi */
        telemetryRepository = mock(TelemetryRepository.class);
        deviceRepository = mock(DeviceRepository.class);
        alertService = mock(AlertService.class);

        /* inizializzato qua e condiviso tra i vari @Test */
        telemetryService = new TelemetryServiceImpl(
                telemetryRepository,
                deviceRepository,
                alertService
        );
    }

    /*
     * Vado semplicemente a testare le interazioni della classe TelemetryServiceImpl()
     * In questo caso testo il metodo saveTelemetry() che andrà a richiamare 3 metodi:
     * - telemetryRepository.saveTelemetry()
     * - deviceRepository.upsertDevice()
     * - alertService.evaluateAlerts()
     **/
    @Test
    void saveTelemetry_shouldSaveTelemetryUpdateDeviceAndEvaluateAlerts() {
        TelemetryRequest request = new TelemetryRequest();
        request.setDeviceId("DEV001");
        request.setTimestamp("2026-08-25T09:14:18Z");
        request.setTemperature(32.95);
        request.setHumidity(41.12);
        request.setPressure(999.82);
        request.setBattery(100);
        request.setOrientation("shaken");

        telemetryService.saveTelemetry(request);

        /*
         * il verify() va a testare le interazini tra l'oggetto reale telemetryService e gli
         * oggetti delle classi fittizie creati con mockito
         * 
         * times(1) = metodo chiamato esattamente 1 volta
         * */
        verify(telemetryRepository, times(1)).saveTelemetry(request);
        verify(deviceRepository, times(1)).upsertDevice("DEV001", 100, "2026-08-25T09:14:18Z");
        verify(alertService, times(1)).evaluateAlerts(request);
        verifyNoMoreInteractions(telemetryRepository, deviceRepository, alertService);
    }

    /*
     * In questo Test vado a vedere che, richiamando il metodo getAllTelemetry() mi venga
     * restituito esattamente lo stesso record che ottengo riciamando il metodo 
     * findAllTelemetry() della classe TelemetryRepossitoryImpl(), in modo da far vedere 
     * che l'oggetto TelemetryServiceImpl chiami correttamente il metodo findAllTelemetry()
     * */
    @Test
    void getAllTelemetry_shouldReturnRepositoryData() {
    	/* creo record di prova */
        List<Map<String, Object>> record = List.of(
                Map.of("deviceId", "DEV001", "temperature", 25.0)
        );

        /* Stubbuing = Quando qualcuno chiamerà telemetryRepository.findAllTelemetry(), 
         * allora restituisci record 
         * 
         * Oss.: telemetryRepository è un MOCK!!
         * */
        when(telemetryRepository.findAllTelemetry()).thenReturn(record);

        // chiamata vera, ma l'oggetto telemetryRepository in getAllTelemetry() è il mock
        List<Map<String, Object>> actual = telemetryService.getAllTelemetry();

        // confronto tra i due valori, devono coincidere
        assertEquals(record, actual);
        
        // verifica aggiuntiva della chiamata al metodo
        verify(telemetryRepository, times(1)).findAllTelemetry();
    }

    /*
     * Comportamento identico al precedente ma testando un metodo 
     * differente: findTelemetryByDeviceId()
     * */
    @Test
    void getTelemetryByDeviceId_shouldReturnSpecificRepositoryData() {
        List<Map<String, Object>> record = List.of(
                Map.of("deviceId", "DEV001", "temperature", 25.0)
        );

        when(telemetryRepository.findTelemetryByDeviceId("DEV001")).thenReturn(record);

        List<Map<String, Object>> actual = telemetryService.getTelemetryByDeviceId("DEV001");

        assertEquals(record, actual);
        verify(telemetryRepository, times(1)).findTelemetryByDeviceId("DEV001");
    }

    @Test
    void getLatestTelemetryByDeviceId_shouldReturnLastRepositoryData() {
        Map<String, Object> expected = Map.of(
                "deviceId", "DEV001",
                "temperature", 25.0
        );

        when(telemetryRepository.findLatestTelemetryByDeviceId("DEV001")).thenReturn(expected);

        Map<String, Object> actual = telemetryService.getLatestTelemetryByDeviceId("DEV001");

        assertEquals(expected, actual);
        verify(telemetryRepository, times(1)).findLatestTelemetryByDeviceId("DEV001");
    }
}
