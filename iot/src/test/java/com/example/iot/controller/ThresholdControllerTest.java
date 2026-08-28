package com.example.iot.controller;

import com.example.iot.dto.ThresholdRequest;
import com.example.iot.service.ThresholdService;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.webmvc.test.autoconfigure.WebMvcTest;
import org.springframework.http.MediaType;
import org.springframework.test.context.bean.override.mockito.MockitoBean;
import org.springframework.test.web.servlet.MockMvc;

import java.util.Map;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.put;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

@WebMvcTest(ThresholdController.class)
class ThresholdControllerTest {

    @Autowired
    private MockMvc mockMvc;

    @MockitoBean
    private ThresholdService thresholdService;

//  @Autowired
//  private ObjectMapper objectMapper;
    
    private final ObjectMapper objectMapper = new ObjectMapper();

    @Test
    void saveThresholds_shouldReturnOkResponse() throws Exception {
        ThresholdRequest request = new ThresholdRequest();
        request.setTemperatureMin(10.0);
        request.setTemperatureMax(30.0);
        request.setHumidityMin(20.0);
        request.setHumidityMax(70.0);
        request.setPressureMin(990.0);
        request.setPressureMax(1030.0);

        mockMvc.perform(put("/api/v1/devices/DEV001/thresholds")
                        .contentType(MediaType.APPLICATION_JSON)
                        .content(objectMapper.writeValueAsString(request)))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$.status").value("ok"))
                .andExpect(jsonPath("$.message").value("Thresholds saved successfully"));

        verify(thresholdService).saveThresholds(eq("DEV001"), any(ThresholdRequest.class));
    }

    @Test
    void getThresholds_shouldReturnThresholds() throws Exception {
        Map<String, Object> thresholds = Map.of(
                "temperature_min", 10.0,
                "temperature_max", 30.0,
                "humidity_min", 20.0,
                "humidity_max", 70.0
        );

        when(thresholdService.getThresholds("DEV001")).thenReturn(thresholds);

        mockMvc.perform(get("/api/v1/devices/DEV001/thresholds"))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$.temperature_min").value(10.0))
                .andExpect(jsonPath("$.temperature_max").value(30.0));

        verify(thresholdService).getThresholds("DEV001");
    }
}