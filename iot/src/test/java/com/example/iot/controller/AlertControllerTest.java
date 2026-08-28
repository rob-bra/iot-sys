package com.example.iot.controller;

import com.example.iot.service.AlertService;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.webmvc.test.autoconfigure.WebMvcTest;
import org.springframework.http.MediaType;
import org.springframework.test.context.bean.override.mockito.MockitoBean;
import org.springframework.test.web.servlet.MockMvc;

import java.util.List;
import java.util.Map;

import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

@WebMvcTest(AlertController.class)
class AlertControllerTest {

    @Autowired
    private MockMvc mockMvc;

    @MockitoBean
    private AlertService alertService;

    @Test
    void getAllAlerts_shouldReturnList() throws Exception {
        List<Map<String, Object>> alerts = List.of(
                Map.of(
                        "device_id", "DEV001",
                        "type", "TEMPERATURE_HIGH",
                        "message", "Temperature above threshold"
                )
        );

        when(alertService.getAllAlerts()).thenReturn(alerts);

        mockMvc.perform(get("/api/v1/alerts"))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$[0].device_id").value("DEV001"))
                .andExpect(jsonPath("$[0].type").value("TEMPERATURE_HIGH"));

        verify(alertService).getAllAlerts();
    }

    @Test
    void getAlertsByDeviceId_shouldReturnList() throws Exception {
        List<Map<String, Object>> alerts = List.of(
                Map.of(
                        "device_id", "DEV001",
                        "type", "TEMPERATURE_HIGH",
                        "message", "Temperature above threshold"
                )
        );

        when(alertService.getAlertsByDeviceId("DEV001")).thenReturn(alerts);

        mockMvc.perform(get("/api/v1/devices/DEV001/alerts"))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$[0].device_id").value("DEV001"))
                .andExpect(jsonPath("$[0].type").value("TEMPERATURE_HIGH"));

        verify(alertService).getAlertsByDeviceId("DEV001");
    }
}