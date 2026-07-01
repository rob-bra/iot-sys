package com.example.iot.repository.impl;

import com.example.iot.repository.DeviceRepository;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

/**
 * REPOSITORY per la tabella devices
 * */

@Repository
public class DeviceRepositoryImpl implements DeviceRepository {

    private final JdbcTemplate jdbcTemplate;

    public DeviceRepositoryImpl(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    /*upsertDevice: - se il device non esiste, viene aggiunto
     * 				- se il device esiste, vengono aggiornati i parametri di stato/batteria/last_seen*/
    @Override
    public void upsertDevice(String deviceId, Integer battery, String lastSeen) {
        String sql = """
                INSERT INTO devices (device_id, status, battery, last_seen)
                VALUES (?, 'ONLINE', ?, ?)
                ON CONFLICT(device_id) DO UPDATE SET
                    status = 'ONLINE',
                    battery = excluded.battery,
                    last_seen = excluded.last_seen
                """;

        jdbcTemplate.update(sql, deviceId, battery, lastSeen);
    }

    /* Tabella in output */
    @Override
    public List<Map<String, Object>> findAllDevices() {
        String sql = "SELECT * FROM devices ORDER BY device_id";
        return jdbcTemplate.queryForList(sql);
    }
}
