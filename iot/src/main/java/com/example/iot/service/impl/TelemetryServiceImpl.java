package com.example.iot.service.impl;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.repository.DeviceRepository;
import com.example.iot.repository.TelemetryRepository;
import com.example.iot.service.AlertService;
import com.example.iot.service.TelemetryService;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Service;

@Service
public class TelemetryServiceImpl implements TelemetryService {

	private final TelemetryRepository telemetryRepository;	// obj per interagire col DB --> TelemetryRepository
    private final DeviceRepository deviceRepository;
    private final AlertService alertService;

    public TelemetryServiceImpl(TelemetryRepository telemetryRepository, DeviceRepository deviceRepository, AlertService alertService) {
        this.telemetryRepository = telemetryRepository;
        this.deviceRepository = deviceRepository;
        this.alertService = alertService;
    }

    /* Salvataggio telemetria nel DB */
    @Override
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
    @Override
    public List<Map<String, Object>> getAllTelemetry() {
        return telemetryRepository.findAllTelemetry();
    }
    
    @Override
    public List<Map<String, Object>> getTelemetryByDeviceId(String deviceId) {
        return telemetryRepository.findTelemetryByDeviceId(deviceId);
    }

    @Override
    public Map<String, Object> getLatestTelemetryByDeviceId(String deviceId) {
        return telemetryRepository.findLatestTelemetryByDeviceId(deviceId);
    }
}
