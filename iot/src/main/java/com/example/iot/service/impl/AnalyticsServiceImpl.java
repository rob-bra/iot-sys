package com.example.iot.service.impl;

import com.example.iot.algorithm.SlidingWindowMovingAverage;
import com.example.iot.dto.MovingAveragePoint;
import com.example.iot.service.AnalyticsService;
import com.example.iot.service.TelemetryService;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

@Service
public class AnalyticsServiceImpl implements AnalyticsService {

    private final TelemetryService telemetryService;

    public AnalyticsServiceImpl(TelemetryService telemetryService) {
        this.telemetryService = telemetryService;
    }

    @Override
    public List<MovingAveragePoint> getMovingAverage(String deviceId, String metric, int windowSize) {
        List<Map<String, Object>> measurements = telemetryService.getTelemetryByDeviceId(deviceId);

        SlidingWindowMovingAverage movingAverage = new SlidingWindowMovingAverage(windowSize);
        List<MovingAveragePoint> result = new ArrayList<>();

        for (Map<String, Object> row : measurements) {
            String timestamp = row.get("timestamp") != null ? row.get("timestamp").toString() : null;
            Double value = extractMetric(row, metric);

            if (value != null) {
                double avg = movingAverage.add(value);
                result.add(new MovingAveragePoint(timestamp, value, avg));
            }
        }

        return result;
    }

    private Double extractMetric(Map<String, Object> row, String metric) {
        Object value;

        switch (metric.toLowerCase()) {
            case "temperature":
                value = row.get("temperature");
                break;
            case "humidity":
                value = row.get("humidity");
                break;
            case "pressure":
                value = row.get("pressure");
                break;
            default:
                throw new IllegalArgumentException("Unsupported metric: " + metric);
        }

        if (value == null) {
            return null;
        }

        return ((Number) value).doubleValue();
    }
}