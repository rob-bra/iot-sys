package com.example.iot.service.impl;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.repository.CommandRepository;
import com.example.iot.service.CommandService;

import org.springframework.stereotype.Service;

import java.time.Instant;
import java.util.List;
import java.util.Map;

@Service
public class CommandServiceImpl implements CommandService {

    private final CommandRepository commandRepository;

    public CommandServiceImpl(CommandRepository commandRepository) {
        this.commandRepository = commandRepository;
    }

    @Override
    public void createCommand(String deviceId, CommandRequest request) {
        String createdAt = Instant.now().toString();
        commandRepository.createCommand(deviceId, request, createdAt);
    }

    @Override
    public List<Map<String, Object>> getCommandsByDeviceId(String deviceId) {
        return commandRepository.findCommandsByDeviceId(deviceId);
    }

    @Override
    public List<Map<String, Object>> getPendingCommandsByDeviceId(String deviceId) {
        return commandRepository.findPendingCommandsByDeviceId(deviceId);
    }

    @Override
    public void ackCommand(Long commandId, CommandAckRequest ackRequest) {
        commandRepository.ackCommand(commandId, ackRequest);
    }
}
