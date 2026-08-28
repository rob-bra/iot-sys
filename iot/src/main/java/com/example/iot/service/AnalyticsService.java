package com.example.iot.service;

import com.example.iot.dto.MovingAveragePoint;
import java.util.List;

public interface AnalyticsService {
    List<MovingAveragePoint> getMovingAverage(String deviceId, String metric, int windowSize);
}