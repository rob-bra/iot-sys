package com.example.iot.service;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.repository.AlertRepository;
import com.example.iot.repository.ThresholdRepository;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Map;

@Service
public class AlertService {

    private final AlertRepository alertRepository;
    private final ThresholdRepository thresholdRepository;

    public AlertService(AlertRepository alertRepository, ThresholdRepository thresholdRepository) {
        this.alertRepository = alertRepository;
        this.thresholdRepository = thresholdRepository;
    }

    public void evaluateAlerts(TelemetryRequest telemetry) {
        Map<String, Object> thresholds = thresholdRepository.findThresholdsByDeviceId(telemetry.getDeviceId());

        if (thresholds == null) {
            return;
        }

        checkTemperature(telemetry, thresholds);
        checkHumidity(telemetry, thresholds);
        checkPressure(telemetry, thresholds);
    }

    private void checkTemperature(TelemetryRequest telemetry, Map<String, Object> thresholds) {
        Double min = getDouble(thresholds.get("temperature_min"));
        Double max = getDouble(thresholds.get("temperature_max"));

        if (min != null && telemetry.getTemperature() < min) {
            alertRepository.saveAlert(
                    telemetry.getDeviceId(),
                    telemetry.getTimestamp(),
                    "LOW_TEMPERATURE",
                    telemetry.getTemperature(),
                    min,
                    "Temperature below minimum threshold"
            );
        }

        if (max != null && telemetry.getTemperature() > max) {
            alertRepository.saveAlert(
                    telemetry.getDeviceId(),
                    telemetry.getTimestamp(),
                    "HIGH_TEMPERATURE",
                    telemetry.getTemperature(),
                    max,
                    "Temperature above maximum threshold"
            );
        }
    }

    private void checkHumidity(TelemetryRequest telemetry, Map<String, Object> thresholds) {
        Double min = getDouble(thresholds.get("humidity_min"));
        Double max = getDouble(thresholds.get("humidity_max"));

        if (min != null && telemetry.getHumidity() < min) {
            alertRepository.saveAlert(
                    telemetry.getDeviceId(),
                    telemetry.getTimestamp(),
                    "LOW_HUMIDITY",
                    telemetry.getHumidity(),
                    min,
                    "Humidity below minimum threshold"
            );
        }

        if (max != null && telemetry.getHumidity() > max) {
            alertRepository.saveAlert(
                    telemetry.getDeviceId(),
                    telemetry.getTimestamp(),
                    "HIGH_HUMIDITY",
                    telemetry.getHumidity(),
                    max,
                    "Humidity above maximum threshold"
            );
        }
    }

    private void checkPressure(TelemetryRequest telemetry, Map<String, Object> thresholds) {
        Double min = getDouble(thresholds.get("pressure_min"));
        Double max = getDouble(thresholds.get("pressure_max"));

        if (min != null && telemetry.getPressure() < min) {
            alertRepository.saveAlert(
                    telemetry.getDeviceId(),
                    telemetry.getTimestamp(),
                    "LOW_PRESSURE",
                    telemetry.getPressure(),
                    min,
                    "Pressure below minimum threshold"
            );
        }

        if (max != null && telemetry.getPressure() > max) {
            alertRepository.saveAlert(
                    telemetry.getDeviceId(),
                    telemetry.getTimestamp(),
                    "HIGH_PRESSURE",
                    telemetry.getPressure(),
                    max,
                    "Pressure above maximum threshold"
            );
        }
    }

    private Double getDouble(Object value) {
        if (value == null) {
            return null;
        }
        return ((Number) value).doubleValue();
    }

    public List<Map<String, Object>> getAllAlerts() {
        return alertRepository.findAllAlerts();
    }

    public List<Map<String, Object>> getAlertsByDeviceId(String deviceId) {
        return alertRepository.findAlertsByDeviceId(deviceId);
    }
}
