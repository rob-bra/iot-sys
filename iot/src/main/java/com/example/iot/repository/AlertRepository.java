package com.example.iot.repository;

import java.util.List;
import java.util.Map;

public interface AlertRepository {
    void saveAlert(String deviceId, String timestamp, String type,
                   Double measuredValue, Double thresholdValue, String message);
    List<Map<String, Object>> findAllAlerts();
    List<Map<String, Object>> findAlertsByDeviceId(String deviceId);
}