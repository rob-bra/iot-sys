package com.example.iot.repository;

import com.example.iot.dto.TelemetryRequest;

import java.util.List;
import java.util.Map;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

@Repository
public class TelemetryRepository {

    private final JdbcTemplate jdbcTemplate;

    public TelemetryRepository(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    public void saveTelemetry(TelemetryRequest telemetry) {
        String sql = """
                INSERT INTO telemetry (device_id, timestamp, temperature, humidity, pressure)
                VALUES (?, ?, ?, ?, ?)
                """;

        jdbcTemplate.update(
                sql,
                telemetry.getDeviceId(),
                telemetry.getTimestamp(),
                telemetry.getTemperature(),
                telemetry.getHumidity(),
                telemetry.getPressure()
        );
    }
    
    // metodo per la lettura dei dati che vengono salvati nel DB (file 'iot.db')
    public List<Map<String, Object>> findAllTelemetry() {
        String sql = "SELECT * FROM telemetry ORDER BY id DESC";
        return jdbcTemplate.queryForList(sql);
    }
}