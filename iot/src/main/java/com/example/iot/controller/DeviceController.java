package com.example.iot.controller;

import com.example.iot.service.DeviceService;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

@RestController		 // @RestController = restituzione in formato REST --> JSON
@RequestMapping("/api/v1")
public class DeviceController {

    private final DeviceService deviceService;

    public DeviceController(DeviceService deviceService) {
        this.deviceService = deviceService;
    }

    @GetMapping("/devices")
    public List<Map<String, Object>> getAllDevices() {
        return deviceService.getAllDevices();
    }
}
