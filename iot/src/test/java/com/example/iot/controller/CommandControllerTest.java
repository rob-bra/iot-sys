package com.example.iot.controller;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.dto.PendingCommandResponse;
import com.example.iot.service.CommandService;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import static org.junit.jupiter.api.Assertions.*;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.service.TelemetryService;
import com.fasterxml.jackson.*;

import org.springframework.boot.webmvc.test.autoconfigure.WebMvcTest;
import org.springframework.http.MediaType;
import org.springframework.test.context.bean.override.mockito.MockitoBean;
import org.springframework.test.web.servlet.MockMvc;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.put;


import java.util.List;
import java.util.Map;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;
import static org.springframework.test.web.servlet.result.MockMvcResultHandlers.print;

@WebMvcTest(CommandController.class)
class CommandControllerTest {

    @Autowired
    private MockMvc mockMvc;

    @MockitoBean
    private CommandService commandService;

//    @Autowired
//    private ObjectMapper objectMapper;
    
    private final ObjectMapper objectMapper = new ObjectMapper();
    
    @Test
    void createCommand_shouldReturnOkResponse() throws Exception {
        CommandRequest request = new CommandRequest();
        request.setType("FORCE_MEASUREMENT");
        request.setPayload("immediate");

        mockMvc.perform(post("/api/v1/devices/DEV001/commands")
                        .contentType(MediaType.APPLICATION_JSON)
                        .content(objectMapper.writeValueAsString(request)))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.status").value("ok"))
                .andExpect(jsonPath("$.message").value("Command created successfully"));

        verify(commandService).createCommand(eq("DEV001"), any(CommandRequest.class));
    }

    @Test
    void getCommandsByDeviceId_shouldReturnList() throws Exception {
        List<Map<String, Object>> commands = List.of(
                Map.of(
                        "id", 1,
                        "device_id", "DEV001",
                        "type", "FORCE_MEASUREMENT",
                        "status", "PENDING"
                )
        );

        when(commandService.getCommandsByDeviceId("DEV001")).thenReturn(commands);

        mockMvc.perform(get("/api/v1/devices/DEV001/commands"))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$[0].id").value(1))
                .andExpect(jsonPath("$[0].device_id").value("DEV001"))
                .andExpect(jsonPath("$[0].type").value("FORCE_MEASUREMENT"))
                .andExpect(jsonPath("$[0].status").value("PENDING"));

        verify(commandService).getCommandsByDeviceId("DEV001");
    }

    @Test
    void getPendingCommandsByDeviceId_shouldReturnList() throws Exception {
        PendingCommandResponse pending = new PendingCommandResponse();
        pending.setId(1L);
        pending.setType("FORCE_MEASUREMENT");
        pending.setPayload("immediate");

        when(commandService.getPendingCommandDtosByDeviceId("DEV001"))
                .thenReturn(List.of(pending));

        mockMvc.perform(get("/api/v1/devices/DEV001/commands/pending"))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$[0].id").value(1))
                .andExpect(jsonPath("$[0].type").value("FORCE_MEASUREMENT"))
                .andExpect(jsonPath("$[0].payload").value("immediate"));

        verify(commandService).getPendingCommandDtosByDeviceId("DEV001");
    }

    @Test
    void ackCommand_shouldReturnOkResponse() throws Exception {
        CommandAckRequest request = new CommandAckRequest();
        request.setStatus("EXECUTED");
        request.setAckAt("2026-07-13T09:14:18Z");
        request.setResultMessage("Measurement executed successfully");

        mockMvc.perform(post("/api/v1/devices/DEV001/commands/1/ack")
                        .contentType(MediaType.APPLICATION_JSON)
                        .content(objectMapper.writeValueAsString(request)))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$.status").value("ok"))
                .andExpect(jsonPath("$.message").value("Command acknowledged successfully"));

        verify(commandService).ackCommand(eq(1L), any(CommandAckRequest.class));
    }
}