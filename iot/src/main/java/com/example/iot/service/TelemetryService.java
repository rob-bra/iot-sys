package com.example.iot.service;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.repository.DeviceRepository;
import com.example.iot.repository.TelemetryRepository;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Service;

@Service
public class TelemetryService {

	private final TelemetryRepository telemetryRepository;	// obj per interagire col DB --> TelemetryRepository
    private final DeviceRepository deviceRepository;

    public TelemetryService(TelemetryRepository telemetryRepository, DeviceRepository deviceRepository) {
        this.telemetryRepository = telemetryRepository;
        this.deviceRepository = deviceRepository;
    }

    /* Salvataggio telemetria nel DB */
    public void saveTelemetry(TelemetryRequest telemetryRequest) {
        telemetryRepository.saveTelemetry(telemetryRequest);
        deviceRepository.upsertDevice(
                telemetryRequest.getDeviceId(),
                telemetryRequest.getBattery(),
                telemetryRequest.getTimestamp()
        );
    }
    
    /* Letture dal DB */
    public List<Map<String, Object>> getAllTelemetry() {
        return telemetryRepository.findAllTelemetry();
    }
    
    public List<Map<String, Object>> getTelemetryByDeviceId(String deviceId) {
        return telemetryRepository.findTelemetryByDeviceId(deviceId);
    }

    public Map<String, Object> getLatestTelemetryByDeviceId(String deviceId) {
        return telemetryRepository.findLatestTelemetryByDeviceId(deviceId);
    }
}
