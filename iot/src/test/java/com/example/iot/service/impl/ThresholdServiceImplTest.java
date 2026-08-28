package com.example.iot.service.impl;

import com.example.iot.dto.ThresholdRequest;
import com.example.iot.repository.ThresholdRepository;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.Map;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.Mockito.*;

class ThresholdServiceImplTest {

    private ThresholdRepository thresholdRepository;
    private ThresholdServiceImpl thresholdService;

    @BeforeEach
    void setUp() {
        thresholdRepository = mock(ThresholdRepository.class);
        thresholdService = new ThresholdServiceImpl(thresholdRepository);
    }

    @Test
    void saveThresholds_shouldDelegateToRepository() {
        ThresholdRequest request = new ThresholdRequest();
        request.setTemperatureMin(10.0);
        request.setTemperatureMax(30.0);
        request.setHumidityMin(20.0);
        request.setHumidityMax(70.0);
        request.setPressureMin(990.0);
        request.setPressureMax(1030.0);

        thresholdService.saveThresholds("DEV001", request);

        verify(thresholdRepository, times(1)).upsertThresholds("DEV001", request);
    }

    @Test
    void getThresholds_shouldReturnRepositoryData() {
        Map<String, Object> expected = Map.of(
                "temperature_min", 10.0,
                "temperature_max", 30.0,
                "humidity_min", 20.0,
                "humidity_max", 70.0,
                "pressure_min", 990.0,
                "pressure_max", 1030.0
        );

        when(thresholdRepository.findThresholdsByDeviceId("DEV001")).thenReturn(expected);

        Map<String, Object> result = thresholdService.getThresholds("DEV001");

        assertEquals(expected, result);
        verify(thresholdRepository, times(1)).findThresholdsByDeviceId("DEV001");
    }
}
