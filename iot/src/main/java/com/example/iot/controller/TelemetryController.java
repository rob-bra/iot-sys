package com.example.iot.controller;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.service.TelemetryService;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

@RestController				// dice che questa classe gestisce richieste web REST
@RequestMapping("/api/v1")	// dice che tutti gli endpoint qui dentro sono raggiungibili da questo prefisso
public class TelemetryController {

    private final TelemetryService telemetryService;

    public TelemetryController(TelemetryService telemetryService) {
        this.telemetryService = telemetryService;
    }

    /* espone endpoint /telemtry per la ricezione di file JSON */
    @PostMapping("/telemetry")		// espone l'endpoint specifico POST "/api/v1/telemetry"
    public Map<String, String> receiveTelemetry(@RequestBody TelemetryRequest request) {
        telemetryService.saveTelemetry(request);	// salva nel DB il JSON che arriva
        return Map.of("status", "ok", "message", "Telemetry saved successfully");
    }
    
    /* espone stesso endpoint ma con metodo GET per acquisire informazioni */
    @GetMapping("/telemetry")
    public List<Map<String, Object>> getAllTelemetry() {
        return telemetryService.getAllTelemetry();
    }
}