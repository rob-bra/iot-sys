package com.example.iot.repository;

import com.example.iot.dto.TelemetryRequest;
import java.util.List;
import java.util.Map;

public interface TelemetryRepository {
    void saveTelemetry(TelemetryRequest telemetry);
    List<Map<String, Object>> findAllTelemetry();
    List<Map<String, Object>> findTelemetryByDeviceId(String deviceId);
    Map<String, Object> findLatestTelemetryByDeviceId(String deviceId);
}