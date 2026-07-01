package com.example.iot.repository;

import java.util.List;
import java.util.Map;

/**
 * REPOSITORY per la tabella devices
 * */
public interface DeviceRepository {
    void upsertDevice(String deviceId, Integer battery, String lastSeen);
    List<Map<String, Object>> findAllDevices();
}