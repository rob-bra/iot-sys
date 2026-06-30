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
    private final AlertService alertService;

    public TelemetryService(TelemetryRepository telemetryRepository, DeviceRepository deviceRepository, AlertService alertService) {
        this.telemetryRepository = telemetryRepository;
        this.deviceRepository = deviceRepository;
        this.alertService = alertService;
    }

    /* Salvataggio telemetria nel DB */
    public void saveTelemetry(TelemetryRequest telemetryRequest) {
        telemetryRepository.saveTelemetry(telemetryRequest);
        deviceRepository.upsertDevice(
                telemetryRequest.getDeviceId(),
                telemetryRequest.getBattery(),
                telemetryRequest.getTimestamp()
        );
        alertService.evaluateAlerts(telemetryRequest);
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
