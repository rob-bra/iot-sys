package com.example.iot.repository;

import com.example.iot.dto.ThresholdRequest;
import java.util.Map;

public interface ThresholdRepository {
    void upsertThresholds(String deviceId, ThresholdRequest request);
    Map<String, Object> findThresholdsByDeviceId(String deviceId);
}