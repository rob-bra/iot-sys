package com.example.iot.service;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.repository.CommandRepository;
import org.springframework.stereotype.Service;

import java.time.Instant;
import java.util.List;
import java.util.Map;

@Service
public class CommandService {

    private final CommandRepository commandRepository;

    public CommandService(CommandRepository commandRepository) {
        this.commandRepository = commandRepository;
    }

    public void createCommand(String deviceId, CommandRequest request) {
        String createdAt = Instant.now().toString();
        commandRepository.createCommand(deviceId, request, createdAt);
    }

    public List<Map<String, Object>> getCommandsByDeviceId(String deviceId) {
        return commandRepository.findCommandsByDeviceId(deviceId);
    }

    public List<Map<String, Object>> getPendingCommandsByDeviceId(String deviceId) {
        return commandRepository.findPendingCommandsByDeviceId(deviceId);
    }

    public void ackCommand(Long commandId, CommandAckRequest ackRequest) {
        commandRepository.ackCommand(commandId, ackRequest);
    }
}
