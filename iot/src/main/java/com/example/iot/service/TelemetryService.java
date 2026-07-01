package com.example.iot.service;

import com.example.iot.dto.TelemetryRequest;
import java.util.List;
import java.util.Map;

public interface TelemetryService {
    void saveTelemetry(TelemetryRequest telemetryRequest);
    List<Map<String, Object>> getAllTelemetry();
    List<Map<String, Object>> getTelemetryByDeviceId(String deviceId);
    Map<String, Object> getLatestTelemetryByDeviceId(String deviceId);
}
