package com.example.iot.service.impl;

import com.example.iot.dto.ThresholdRequest;
import com.example.iot.repository.ThresholdRepository;
import com.example.iot.service.ThresholdService;

import org.springframework.stereotype.Service;

import java.util.Map;

@Service
public class ThresholdServiceImpl implements ThresholdService {

    private final ThresholdRepository thresholdRepository;

    public ThresholdServiceImpl(ThresholdRepository thresholdRepository) {
        this.thresholdRepository = thresholdRepository;
    }

    @Override
    public void saveThresholds(String deviceId, ThresholdRequest request) {
        thresholdRepository.upsertThresholds(deviceId, request);
    }

    @Override
    public Map<String, Object> getThresholds(String deviceId) {
        return thresholdRepository.findThresholdsByDeviceId(deviceId);
    }
}

