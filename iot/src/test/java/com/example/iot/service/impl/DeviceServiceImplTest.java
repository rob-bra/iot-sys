package com.example.iot.service.impl;

import com.example.iot.repository.DeviceRepository;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.List;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.Mockito.*;

class DeviceServiceImplTest {

    private DeviceRepository deviceRepository;
    private DeviceServiceImpl deviceService;

    @BeforeEach
    void setUp() {
        deviceRepository = mock(DeviceRepository.class);
        deviceService = new DeviceServiceImpl(deviceRepository);
    }

    @Test
    void getAllDevices_shouldReturnRepositoryData() {
        List<Map<String, Object>> expected = List.of(
                Map.of(
                        "device_id", "DEV001",
                        "status", "ONLINE",
                        "battery", 100,
                        "last_seen", "2026-07-13T09:14:18Z"
                )
        );

        when(deviceRepository.findAllDevices()).thenReturn(expected);

        List<Map<String, Object>> result = deviceService.getAllDevices();

        assertEquals(expected, result);
        verify(deviceRepository, times(1)).findAllDevices();
    }
}