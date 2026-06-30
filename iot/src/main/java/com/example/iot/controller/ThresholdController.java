package com.example.iot.controller;

import com.example.iot.dto.ThresholdRequest;
import com.example.iot.service.ThresholdService;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/api/v1")
public class ThresholdController {

    private final ThresholdService thresholdService;

    public ThresholdController(ThresholdService thresholdService) {
        this.thresholdService = thresholdService;
    }

    @PutMapping("/devices/{deviceId}/thresholds")
    public Map<String, String> saveThresholds(@PathVariable String deviceId,
                                              @RequestBody ThresholdRequest request) {
        thresholdService.saveThresholds(deviceId, request);
        return Map.of("status", "ok", "message", "Thresholds saved successfully");
    }

    @GetMapping("/devices/{deviceId}/thresholds")
    public Map<String, Object> getThresholds(@PathVariable String deviceId) {
        return thresholdService.getThresholds(deviceId);
    }
}