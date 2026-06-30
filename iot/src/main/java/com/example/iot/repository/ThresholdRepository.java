package com.example.iot.repository;

import com.example.iot.dto.ThresholdRequest;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

@Repository
public class ThresholdRepository {

    private final JdbcTemplate jdbcTemplate;

    public ThresholdRepository(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    public void upsertThresholds(String deviceId, ThresholdRequest request) {
        String sql = """
                INSERT INTO thresholds (
                    device_id,
                    temperature_min,
                    temperature_max,
                    humidity_min,
                    humidity_max,
                    pressure_min,
                    pressure_max
                )
                VALUES (?, ?, ?, ?, ?, ?, ?)
                ON CONFLICT(device_id) DO UPDATE SET
                    temperature_min = excluded.temperature_min,
                    temperature_max = excluded.temperature_max,
                    humidity_min = excluded.humidity_min,
                    humidity_max = excluded.humidity_max,
                    pressure_min = excluded.pressure_min,
                    pressure_max = excluded.pressure_max
                """;

        jdbcTemplate.update(
                sql,
                deviceId,
                request.getTemperatureMin(),
                request.getTemperatureMax(),
                request.getHumidityMin(),
                request.getHumidityMax(),
                request.getPressureMin(),
                request.getPressureMax()
        );
    }

    public Map<String, Object> findThresholdsByDeviceId(String deviceId) {
        String sql = "SELECT * FROM thresholds WHERE device_id = ?";
        List<Map<String, Object>> result = jdbcTemplate.queryForList(sql, deviceId);
        return result.isEmpty() ? null : result.get(0);
    }
}
