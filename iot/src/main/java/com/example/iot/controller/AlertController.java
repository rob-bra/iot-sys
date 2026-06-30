package com.example.iot.controller;

import com.example.iot.service.AlertService;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/v1")
public class AlertController {

    private final AlertService alertService;

    public AlertController(AlertService alertService) {
        this.alertService = alertService;
    }

    @GetMapping("/alerts")
    public List<Map<String, Object>> getAllAlerts() {
        return alertService.getAllAlerts();
    }

    @GetMapping("/devices/{deviceId}/alerts")
    public List<Map<String, Object>> getAlertsByDeviceId(@PathVariable String deviceId) {
        return alertService.getAlertsByDeviceId(deviceId);
    }
}
