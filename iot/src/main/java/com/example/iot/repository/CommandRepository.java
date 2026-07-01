package com.example.iot.repository;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import java.util.List;
import java.util.Map;

public interface CommandRepository {
    void createCommand(String deviceId, CommandRequest request, String createdAt);
    List<Map<String, Object>> findCommandsByDeviceId(String deviceId);
    List<Map<String, Object>> findPendingCommandsByDeviceId(String deviceId);
    void ackCommand(Long commandId, CommandAckRequest ackRequest);
}
