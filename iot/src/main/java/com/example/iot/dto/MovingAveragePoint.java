package com.example.iot.dto;

public class MovingAveragePoint {

    private String timestamp;
    private Double originalValue;
    private Double movingAverage;

    public MovingAveragePoint() {
    }

    public MovingAveragePoint(String timestamp, Double originalValue, Double movingAverage) {
        this.timestamp = timestamp;
        this.originalValue = originalValue;
        this.movingAverage = movingAverage;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }

    public Double getOriginalValue() {
        return originalValue;
    }

    public void setOriginalValue(Double originalValue) {
        this.originalValue = originalValue;
    }

    public Double getMovingAverage() {
        return movingAverage;
    }

    public void setMovingAverage(Double movingAverage) {
        this.movingAverage = movingAverage;
    }
}