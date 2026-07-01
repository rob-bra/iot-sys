package com.example.iot.service;

import com.example.iot.dto.TelemetryRequest;
import java.util.List;
import java.util.Map;

public interface AlertService {
    void evaluateAlerts(TelemetryRequest telemetry);
    List<Map<String, Object>> getAllAlerts();
    List<Map<String, Object>> getAlertsByDeviceId(String deviceId);
}