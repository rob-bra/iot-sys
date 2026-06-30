package com.example.iot.service;

import com.example.iot.repository.DeviceRepository;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Map;

@Service
public class DeviceService {

    private final DeviceRepository deviceRepository;

    public DeviceService(DeviceRepository deviceRepository) {
        this.deviceRepository = deviceRepository;
    }

    public List<Map<String, Object>> getAllDevices() {
        return deviceRepository.findAllDevices();
    }
}