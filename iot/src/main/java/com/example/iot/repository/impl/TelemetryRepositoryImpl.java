package com.example.iot.repository.impl;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.repository.TelemetryRepository;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

@Repository
public class TelemetryRepositoryImpl implements TelemetryRepository {

    private final JdbcTemplate jdbcTemplate;

    public TelemetryRepositoryImpl(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public void saveTelemetry(TelemetryRequest telemetry) {
        String sql = """
                INSERT INTO telemetry (device_id, timestamp, temperature, humidity, pressure, battery, orientation)
                VALUES (?, ?, ?, ?, ?, ?, ?)
                """;

        jdbcTemplate.update(
                sql,
                telemetry.getDeviceId(),
                telemetry.getTimestamp(),
                telemetry.getTemperature(),
                telemetry.getHumidity(),
                telemetry.getPressure(),
                telemetry.getBattery(),
                telemetry.getOrientation()
        );
    }
    
    /* metodo per la lettura dei dati nel DB (file 'iot.db') di TUTTI I DISPOSITIVI che vengono salvati */
    @Override
    public List<Map<String, Object>> findAllTelemetry() {
        String sql = "SELECT * FROM telemetry ORDER BY id DESC";		// recupera tutte le righe e le ordina dalla piu' recente (desc=decrescente)
        return jdbcTemplate.queryForList(sql);							// restituisce una lista di righe in cui ogi riga e' una mappa<column_name,val>
        																// quindi il JSON con tutti i valori delle colonne: "device_id": 3
        																// 													"timestamp": "2026-06-25T11:06:00Z" ...
    }
    
    /* metodo per la lettura dei dati nel DB di 1 SINGOLO DEVICE */
    @Override
    public List<Map<String, Object>> findTelemetryByDeviceId(String deviceId) {
        String sql = "SELECT * FROM telemetry WHERE device_id = ? ORDER BY id DESC";
        return jdbcTemplate.queryForList(sql, deviceId);
    }
    
    /* leggere ESCLUSIVAMENTE L'ULTIMA telemetria di 1 SINGOLO DEVICE */
    @Override
    public Map<String, Object> findLatestTelemetryByDeviceId(String deviceId) {
        String sql = "SELECT * FROM telemetry WHERE device_id = ? ORDER BY id DESC LIMIT 1";
        List<Map<String, Object>> result = jdbcTemplate.queryForList(sql, deviceId);
        return result.isEmpty() ? null : result.get(0);
    }
    
}