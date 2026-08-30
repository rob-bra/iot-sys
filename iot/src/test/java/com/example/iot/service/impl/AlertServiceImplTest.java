package com.example.iot.service.impl;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.repository.AlertRepository;
import com.example.iot.repository.ThresholdRepository;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.List;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.Mockito.*;

class AlertServiceImplTest {

    private AlertRepository alertRepository;
    private ThresholdRepository thresholdRepository;
    private AlertServiceImpl alertService;

    @BeforeEach
    void setUp() {
        alertRepository = mock(AlertRepository.class);
        thresholdRepository = mock(ThresholdRepository.class);
        alertService = new AlertServiceImpl(alertRepository, thresholdRepository);
    }

    @Test
    void evaluateAlerts_shouldDoNothingWhenThresholdsAreNull() {
        TelemetryRequest telemetry = new TelemetryRequest();
        telemetry.setDeviceId("DEV001");
        telemetry.setTimestamp("2026-07-13T09:14:18Z");
        telemetry.setTemperature(25.0);
        telemetry.setHumidity(50.0);
        telemetry.setPressure(1000.0);

        when(thresholdRepository.findThresholdsByDeviceId("DEV001")).thenReturn(null);

        alertService.evaluateAlerts(telemetry);

        verify(thresholdRepository, times(1)).findThresholdsByDeviceId("DEV001");
        verifyNoInteractions(alertRepository);
    }

    @Test
    void evaluateAlerts_shouldSaveLowTemperatureAlert() {
        TelemetryRequest telemetry = new TelemetryRequest();
        telemetry.setDeviceId("DEV001");
        telemetry.setTimestamp("2026-07-13T09:14:18Z");
        telemetry.setTemperature(5.0);
        telemetry.setHumidity(50.0);
        telemetry.setPressure(1000.0);

        Map<String, Object> thresholds = Map.of(
                "temperature_min", 10.0
        );

        when(thresholdRepository.findThresholdsByDeviceId("DEV001")).thenReturn(thresholds);

        alertService.evaluateAlerts(telemetry);

        verify(alertRepository, times(1)).saveAlert(
                "DEV001",
                "2026-07-13T09:14:18Z",
                "LOW_TEMPERATURE",
                5.0,
                10.0,
                "Temperature below minimum threshold"
        );
    }

    @Test
    void evaluateAlerts_shouldSaveHighTemperatureAlert() {
        TelemetryRequest telemetry = new TelemetryRequest();
        telemetry.setDeviceId("DEV001");
        telemetry.setTimestamp("2026-07-13T09:14:18Z");
        telemetry.setTemperature(35.0);
        telemetry.setHumidity(50.0);
        telemetry.setPressure(1000.0);

        Map<String, Object> thresholds = Map.of(
                "temperature_max", 30.0
        );

        when(thresholdRepository.findThresholdsByDeviceId("DEV001")).thenReturn(thresholds);

        alertService.evaluateAlerts(telemetry);

        verify(alertRepository, times(1)).saveAlert(
                "DEV001",
                "2026-07-13T09:14:18Z",
                "HIGH_TEMPERATURE",
                35.0,
                30.0,
                "Temperature above maximum threshold"
        );
    }

    @Test
    void evaluateAlerts_shouldSaveLowHumidityAlert() {
        TelemetryRequest telemetry = new TelemetryRequest();
        telemetry.setDeviceId("DEV001");
        telemetry.setTimestamp("2026-07-13T09:14:18Z");
        telemetry.setTemperature(25.0);
        telemetry.setHumidity(10.0);
        telemetry.setPressure(1000.0);

        Map<String, Object> thresholds = Map.of(
                "humidity_min", 20.0
        );

        when(thresholdRepository.findThresholdsByDeviceId("DEV001")).thenReturn(thresholds);

        alertService.evaluateAlerts(telemetry);

        verify(alertRepository, times(1)).saveAlert(
                "DEV001",
                "2026-07-13T09:14:18Z",
                "LOW_HUMIDITY",
                10.0,
                20.0,
                "Humidity below minimum threshold"
        );
    }

    @Test
    void evaluateAlerts_shouldSaveHighHumidityAlert() {
        TelemetryRequest telemetry = new TelemetryRequest();
        telemetry.setDeviceId("DEV001");
        telemetry.setTimestamp("2026-07-13T09:14:18Z");
        telemetry.setTemperature(25.0);
        telemetry.setHumidity(80.0);
        telemetry.setPressure(1000.0);

        Map<String, Object> thresholds = Map.of(
                "humidity_max", 70.0
        );

        when(thresholdRepository.findThresholdsByDeviceId("DEV001")).thenReturn(thresholds);

        alertService.evaluateAlerts(telemetry);

        verify(alertRepository, times(1)).saveAlert(
                "DEV001",
                "2026-07-13T09:14:18Z",
                "HIGH_HUMIDITY",
                80.0,
                70.0,
                "Humidity above maximum threshold"
        );
    }

    @Test
    void evaluateAlerts_shouldSaveLowPressureAlert() {
        TelemetryRequest telemetry = new TelemetryRequest();
        telemetry.setDeviceId("DEV001");
        telemetry.setTimestamp("2026-07-13T09:14:18Z");
        telemetry.setTemperature(25.0);
        telemetry.setHumidity(50.0);
        telemetry.setPressure(980.0);

        Map<String, Object> thresholds = Map.of(
                "pressure_min", 990.0
        );

        when(thresholdRepository.findThresholdsByDeviceId("DEV001")).thenReturn(thresholds);

        alertService.evaluateAlerts(telemetry);

        verify(alertRepository, times(1)).saveAlert(
                "DEV001",
                "2026-07-13T09:14:18Z",
                "LOW_PRESSURE",
                980.0,
                990.0,
                "Pressure below minimum threshold"
        );
    }

    @Test
    void evaluateAlerts_shouldSaveHighPressureAlert() {
        TelemetryRequest telemetry = new TelemetryRequest();
        telemetry.setDeviceId("DEV001");
        telemetry.setTimestamp("2026-07-13T09:14:18Z");
        telemetry.setTemperature(25.0);
        telemetry.setHumidity(50.0);
        telemetry.setPressure(1040.0);

        Map<String, Object> thresholds = Map.of(
                "pressure_max", 1030.0
        );

        when(thresholdRepository.findThresholdsByDeviceId("DEV001")).thenReturn(thresholds);

        alertService.evaluateAlerts(telemetry);

        verify(alertRepository, times(1)).saveAlert(
                "DEV001",
                "2026-07-13T09:14:18Z",
                "HIGH_PRESSURE",
                1040.0,
                1030.0,
                "Pressure above maximum threshold"
        );
    }

    @Test
    void evaluateAlerts_shouldNotSaveAlertsWhenValuesAreWithinThresholds() {
        TelemetryRequest telemetry = new TelemetryRequest();
        telemetry.setDeviceId("DEV001");
        telemetry.setTimestamp("2026-07-13T09:14:18Z");
        telemetry.setTemperature(25.0);
        telemetry.setHumidity(50.0);
        telemetry.setPressure(1000.0);

        Map<String, Object> thresholds = Map.of(
                "temperature_min", 10.0,
                "temperature_max", 30.0,
                "humidity_min", 20.0,
                "humidity_max", 70.0,
                "pressure_min", 990.0,
                "pressure_max", 1030.0
        );

        when(thresholdRepository.findThresholdsByDeviceId("DEV001")).thenReturn(thresholds);

        alertService.evaluateAlerts(telemetry);

        verify(thresholdRepository, times(1)).findThresholdsByDeviceId("DEV001");
        verifyNoInteractions(alertRepository);
    }

    @Test
    void getAllAlerts_shouldReturnRepositoryData() {
        List<Map<String, Object>> expected = List.of(
                Map.of(
                        "device_id", "DEV001",
                        "type", "HIGH_TEMPERATURE",
                        "message", "Temperature above maximum threshold"
                )
        );

        when(alertRepository.findAllAlerts()).thenReturn(expected);

        List<Map<String, Object>> result = alertService.getAllAlerts();

        assertEquals(expected, result);
        verify(alertRepository, times(1)).findAllAlerts();
    }

    @Test
    void getAlertsByDeviceId_shouldReturnRepositoryData() {
        List<Map<String, Object>> expected = List.of(
                Map.of(
                        "device_id", "DEV001",
                        "type", "HIGH_TEMPERATURE",
                        "message", "Temperature above maximum threshold"
                )
        );

        when(alertRepository.findAlertsByDeviceId("DEV001")).thenReturn(expected);

        List<Map<String, Object>> result = alertService.getAlertsByDeviceId("DEV001");

        assertEquals(expected, result);
        verify(alertRepository, times(1)).findAlertsByDeviceId("DEV001");
    }
}
