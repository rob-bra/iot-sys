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

public class TelemetryServiceImplTestN {

    private TelemetryRepository telemetryRepository;
    private DeviceRepository deviceRepository;
    private AlertService alertService;
    private TelemetryServiceImpl telemetryService;

    /* @BeforeEach viene chiamata solo una volta all'inizio, prima del @Test
     * es. se vado ad inizializzare una classe qua dentro, e la utilizzo nei test successivi, il
     * valore dell'oggetto verrà condiviso tra tutti i metodi di test che usano l'oggetto: 
     * 
     * telemetryService viene inizializzato e verrà condiviso tra tutti i @Test*/
    @BeforeEach
    void setUp() {
    	/* I mock, a differenza degli assert, vanno a creare degli oggetti fittizi */
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

        verify(telemetryRepository, times(1)).saveTelemetry(request);
        //verify(deviceRepository, times(1)).upsertDevice("DEV001", 100, "2026-07-13T09:14:18Z");
        verify(alertService, times(1)).evaluateAlerts(request);
        //verifyNoMoreInteractions(telemetryRepository, deviceRepository, alertService);
    }

    @Test
    void getAllTelemetry_shouldReturnRepositoryData() {
        List<Map<String, Object>> expected = List.of(
                Map.of("deviceId", "DEV001", "temperature", 25.0)
        );

        when(telemetryRepository.findAllTelemetry()).thenReturn(expected);

        List<Map<String, Object>> result = telemetryService.getAllTelemetry();

        assertEquals(expected, result);
        verify(telemetryRepository, times(1)).findAllTelemetry();
    }

    @Test
    void getTelemetryByDeviceId_shouldReturnRepositoryData() {
        List<Map<String, Object>> expected = List.of(
                Map.of("deviceId", "DEV001", "temperature", 25.0)
        );

        when(telemetryRepository.findTelemetryByDeviceId("DEV001")).thenReturn(expected);

        List<Map<String, Object>> result = telemetryService.getTelemetryByDeviceId("DEV001");

        assertEquals(expected, result);
        verify(telemetryRepository, times(1)).findTelemetryByDeviceId("DEV001");
    }

    @Test
    void getLatestTelemetryByDeviceId_shouldReturnRepositoryData() {
        Map<String, Object> expected = Map.of(
                "deviceId", "DEV001",
                "temperature", 25.0
        );

        when(telemetryRepository.findLatestTelemetryByDeviceId("DEV001")).thenReturn(expected);

        Map<String, Object> result = telemetryService.getLatestTelemetryByDeviceId("DEV001");

        assertEquals(expected, result);
        verify(telemetryRepository, times(1)).findLatestTelemetryByDeviceId("DEV001");
    }
}
