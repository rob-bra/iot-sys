package com.example.iot.controller;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Test;
import com.example.iot.dto.TelemetryRequest;
import com.example.iot.service.TelemetryService;
import com.fasterxml.jackson.*;

import tools.jackson.databind.ObjectMapper;

import org.springframework.beans.factory.annotation.Autowired;
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
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.put;

import java.util.List;
import java.util.Map;

@WebMvcTest(TelemetryController.class)	// carica solo il contesto Spring MVC necessario per testare questo controller
class TelemetryControllerTest {

	/* simula chiamate HTTP */
	@Autowired
    private MockMvc mockMvc;
	
	/* mock da usare */
	@MockitoBean
    private TelemetryService telemetryService;
	
	/* converte oggetti Java in JSON */
	@Autowired
    private ObjectMapper objectMapper;
	
	@Test
    void receiveTelemetry_shouldReturnOkResponse() throws Exception {
        // Arrange: creo un DTO che rappresenta il JSON in input
        TelemetryRequest request = new TelemetryRequest();
        request.setDeviceId("DEV001");
        request.setTimestamp("2026-07-13T09:14:18Z");
        request.setTemperature(32.95);
        request.setHumidity(41.12);
        request.setPressure(999.82);
        request.setBattery(100);
        request.setOrientation("shaken");

        // Act + Assert: simulo una POST HTTP verso l'endpoint REST
        mockMvc.perform(post("/api/v1/telemetry")
                        .contentType(MediaType.APPLICATION_JSON)
                        .content(objectMapper.writeValueAsString(request)))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$.status").value("ok"))
                .andExpect(jsonPath("$.message").value("Telemetry saved successfully"));

        // Verify: verifico che il controller abbia delegato al service
        verify(telemetryService).saveTelemetry(any(TelemetryRequest.class));
    }

    
    @Test
    void getAllTelemetry_shouldReturnList() throws Exception {
        List<Map<String, Object>> telemetryList = List.of(
                Map.of(
                        "deviceId", "DEV001",
                        "temperature", 25.0
                )
        );

        when(telemetryService.getAllTelemetry()).thenReturn(telemetryList);
        
        /* GET method */
        mockMvc.perform(get("/api/v1/telemetry"))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$[0].deviceId").value("DEV001"))
                .andExpect(jsonPath("$[0].temperature").value(25.0));

        verify(telemetryService).getAllTelemetry();
    }

    @Test
    void getLatestTelemetry_shouldReturnLatestMeasurement() throws Exception {
        Map<String, Object> latest = Map.of(
                "deviceId", "DEV001",
                "temperature", 26.3,
                "timestamp", "2026-07-13T09:14:18Z"
        );

        when(telemetryService.getLatestTelemetryByDeviceId("DEV001")).thenReturn(latest);

        mockMvc.perform(get("/api/v1/devices/DEV001/latest"))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$.deviceId").value("DEV001"))
                .andExpect(jsonPath("$.temperature").value(26.3));

        verify(telemetryService).getLatestTelemetryByDeviceId("DEV001");
    }

    @Test
    void getTelemetryByDeviceId_shouldReturnMeasurementsList() throws Exception {
        List<Map<String, Object>> measurements = List.of(
                Map.of(
                        "deviceId", "DEV001",
                        "temperature", 24.8,
                        "timestamp", "2026-07-13T09:14:18Z"
                )
        );

        when(telemetryService.getTelemetryByDeviceId("DEV001")).thenReturn(measurements);

        mockMvc.perform(get("/api/v1/devices/DEV001/measurements"))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$[0].deviceId").value("DEV001"))
                .andExpect(jsonPath("$[0].temperature").value(24.8));

        verify(telemetryService).getTelemetryByDeviceId("DEV001");
    }
    
    @Test
    void receiveTelemetry_shouldReturnBadRequestWhenJsonIsInvalid() throws Exception {
        // Act + Assert: JSON malformato -> atteso HTTP 400
        mockMvc.perform(post("/api/v1/telemetry")
                        .contentType(MediaType.APPLICATION_JSON)
                        .content("{ invalid-json }"))
                .andExpect(status().isBadRequest());
    }

}
















