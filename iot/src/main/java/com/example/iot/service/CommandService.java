package com.example.iot.service;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import java.util.List;
import java.util.Map;

public interface CommandService {
    void createCommand(String deviceId, CommandRequest request);
    List<Map<String, Object>> getCommandsByDeviceId(String deviceId);
    List<Map<String, Object>> getPendingCommandsByDeviceId(String deviceId);
    void ackCommand(Long commandId, CommandAckRequest ackRequest);
}
