package com.example.iot.service;

import java.util.List;
import java.util.Map;

public interface DeviceService {
    List<Map<String, Object>> getAllDevices();
}