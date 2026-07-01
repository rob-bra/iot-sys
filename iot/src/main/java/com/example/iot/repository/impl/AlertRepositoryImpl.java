package com.example.iot.repository.impl;

import com.example.iot.repository.AlertRepository;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

@Repository
public class AlertRepositoryImpl implements AlertRepository {

    private final JdbcTemplate jdbcTemplate;

    public AlertRepositoryImpl(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public void saveAlert(String deviceId, String timestamp, String type,
                          Double measuredValue, Double thresholdValue, String message) {
        String sql = """
                INSERT INTO alerts (device_id, timestamp, type, measured_value, threshold_value, message)
                VALUES (?, ?, ?, ?, ?, ?)
                """;

        jdbcTemplate.update(sql, deviceId, timestamp, type, measuredValue, thresholdValue, message);
    }

    @Override
    public List<Map<String, Object>> findAllAlerts() {
        String sql = "SELECT * FROM alerts ORDER BY id DESC";
        return jdbcTemplate.queryForList(sql);
    }

    @Override
    public List<Map<String, Object>> findAlertsByDeviceId(String deviceId) {
        String sql = "SELECT * FROM alerts WHERE device_id = ? ORDER BY id DESC";
        return jdbcTemplate.queryForList(sql, deviceId);
    }

}
