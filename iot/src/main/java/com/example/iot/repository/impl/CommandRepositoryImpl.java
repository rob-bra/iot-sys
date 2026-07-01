package com.example.iot.repository.impl;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.repository.CommandRepository;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

@Repository
public class CommandRepositoryImpl implements CommandRepository {

    private final JdbcTemplate jdbcTemplate;

    public CommandRepositoryImpl(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    @Override
    public void createCommand(String deviceId, CommandRequest request, String createdAt) {
        String sql = """
                INSERT INTO commands (device_id, type, payload, status, created_at)
                VALUES (?, ?, ?, 'PENDING', ?)
                """;

        jdbcTemplate.update(sql, deviceId, request.getType(), request.getPayload(), createdAt);
    }

    @Override
    public List<Map<String, Object>> findCommandsByDeviceId(String deviceId) {
        String sql = "SELECT * FROM commands WHERE device_id = ? ORDER BY id DESC";
        return jdbcTemplate.queryForList(sql, deviceId);
    }

    @Override
    public List<Map<String, Object>> findPendingCommandsByDeviceId(String deviceId) {
        String sql = "SELECT * FROM commands WHERE device_id = ? AND status = 'PENDING' ORDER BY id ASC";
        return jdbcTemplate.queryForList(sql, deviceId);
    }

    @Override
    public void ackCommand(Long commandId, CommandAckRequest ackRequest) {
        String sql = """
                UPDATE commands
                SET status = ?, ack_at = ?, result_message = ?
                WHERE id = ?
                """;

        jdbcTemplate.update(
                sql,
                ackRequest.getStatus(),
                ackRequest.getAckAt(),
                ackRequest.getResultMessage(),
                commandId
        );
    }
}
