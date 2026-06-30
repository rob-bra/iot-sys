package com.example.iot.service;

import com.example.iot.dto.ThresholdRequest;
import com.example.iot.repository.ThresholdRepository;
import org.springframework.stereotype.Service;

import java.util.Map;

@Service
public class ThresholdService {

    private final ThresholdRepository thresholdRepository;

    public ThresholdService(ThresholdRepository thresholdRepository) {
        this.thresholdRepository = thresholdRepository;
    }

    public void saveThresholds(String deviceId, ThresholdRequest request) {
        thresholdRepository.upsertThresholds(deviceId, request);
    }

    public Map<String, Object> getThresholds(String deviceId) {
        return thresholdRepository.findThresholdsByDeviceId(deviceId);
    }
}
