package com.example.iot.service.impl;

import com.example.iot.repository.DeviceRepository;
import com.example.iot.service.DeviceService;

import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Map;

@Service
public class DeviceServiceImpl implements DeviceService {

    private final DeviceRepository deviceRepository;

    public DeviceServiceImpl(DeviceRepository deviceRepository) {
        this.deviceRepository = deviceRepository;
    }

    @Override
    public List<Map<String, Object>> getAllDevices() {
        return deviceRepository.findAllDevices();
    }
}