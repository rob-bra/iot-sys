package com.example.iot.dto;

/**
 * Struttura del tipo di dato dell'oggetto di tipo TresholdRequest
 * */
public class ThresholdRequest {

    private Double temperatureMin;
    private Double temperatureMax;
    private Double humidityMin;
    private Double humidityMax;
    private Double pressureMin;
    private Double pressureMax;

    public ThresholdRequest() {
    }

    public Double getTemperatureMin() {
        return temperatureMin;
    }

    public void setTemperatureMin(Double temperatureMin) {
        this.temperatureMin = temperatureMin;
    }

    public Double getTemperatureMax() {
        return temperatureMax;
    }

    public void setTemperatureMax(Double temperatureMax) {
        this.temperatureMax = temperatureMax;
    }

    public Double getHumidityMin() {
        return humidityMin;
    }

    public void setHumidityMin(Double humidityMin) {
        this.humidityMin = humidityMin;
    }

    public Double getHumidityMax() {
        return humidityMax;
    }

    public void setHumidityMax(Double humidityMax) {
        this.humidityMax = humidityMax;
    }

    public Double getPressureMin() {
        return pressureMin;
    }

    public void setPressureMin(Double pressureMin) {
        this.pressureMin = pressureMin;
    }

    public Double getPressureMax() {
        return pressureMax;
    }

    public void setPressureMax(Double pressureMax) {
        this.pressureMax = pressureMax;
    }
}
