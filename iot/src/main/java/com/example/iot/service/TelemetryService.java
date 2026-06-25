package com.example.iot.service;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.repository.TelemetryRepository;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Service;

@Service
public class TelemetryService {

    private final TelemetryRepository telemetryRepository;	// obj per interagire col DB --> TelemetryRepository

    public TelemetryService(TelemetryRepository telemetryRepository) {
        this.telemetryRepository = telemetryRepository;
    }

    /* Salvataggio nel DB */
    public void saveTelemetry(TelemetryRequest telemetryRequest) {
        telemetryRepository.saveTelemetry(telemetryRequest);
    }
    
    /* Lettura dal DB */
    public List<Map<String, Object>> getAllTelemetry() {
        return telemetryRepository.findAllTelemetry();
    }
}
