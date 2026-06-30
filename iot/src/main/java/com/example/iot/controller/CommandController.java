package com.example.iot.controller;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.service.CommandService;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/v1")
public class CommandController {

    private final CommandService commandService;

    public CommandController(CommandService commandService) {
        this.commandService = commandService;
    }

    @PostMapping("/devices/{deviceId}/commands")
    public Map<String, String> createCommand(@PathVariable String deviceId,
                                             @RequestBody CommandRequest request) {
        commandService.createCommand(deviceId, request);
        return Map.of("status", "ok", "message", "Command created successfully");
    }

    @GetMapping("/devices/{deviceId}/commands")
    public List<Map<String, Object>> getCommandsByDeviceId(@PathVariable String deviceId) {
        return commandService.getCommandsByDeviceId(deviceId);
    }

    @GetMapping("/devices/{deviceId}/commands/pending")
    public List<Map<String, Object>> getPendingCommandsByDeviceId(@PathVariable String deviceId) {
        return commandService.getPendingCommandsByDeviceId(deviceId);
    }

    @PostMapping("/devices/{deviceId}/commands/{commandId}/ack")
    public Map<String, String> ackCommand(@PathVariable String deviceId,
                                          @PathVariable Long commandId,
                                          @RequestBody CommandAckRequest request) {
        commandService.ackCommand(commandId, request);
        return Map.of("status", "ok", "message", "Command acknowledged successfully");
    }
}




















