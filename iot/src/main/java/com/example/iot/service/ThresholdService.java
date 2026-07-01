package com.example.iot.service;

import com.example.iot.dto.ThresholdRequest;
import java.util.Map;

public interface ThresholdService {
    void saveThresholds(String deviceId, ThresholdRequest request);
    Map<String, Object> getThresholds(String deviceId);
}
