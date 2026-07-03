package com.example.iot.controller;

import com.example.iot.service.AlertService;
import com.example.iot.service.CommandService;
import com.example.iot.service.DeviceService;
import com.example.iot.service.TelemetryService;
import com.example.iot.service.ThresholdService;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;

@Controller
public class DashboardController {

	/* Controller a Dashboard per ogni componente */
    private final DeviceService deviceService;
    private final TelemetryService telemetryService;
    private final AlertService alertService;
    private final ThresholdService thresholdService;
    private final CommandService commandService;

    public DashboardController(DeviceService deviceService,
                               TelemetryService telemetryService,
                               AlertService alertService,
                               ThresholdService thresholdService,
                               CommandService commandService) {
        this.deviceService = deviceService;
        this.telemetryService = telemetryService;
        this.alertService = alertService;
        this.thresholdService = thresholdService;
        this.commandService = commandService;
    }

    @GetMapping("/")
    public String home() {
        return "index";
    }

    @GetMapping("/devices-page")
    public String devicesPage(Model model) {
        model.addAttribute("devices", deviceService.getAllDevices());
        return "devices";
    }

    @GetMapping("/devices-page/{deviceId}")
    public String deviceDetailPage(@PathVariable String deviceId, Model model) {
        model.addAttribute("deviceId", deviceId);
        model.addAttribute("latestTelemetry", telemetryService.getLatestTelemetryByDeviceId(deviceId));
        model.addAttribute("measurements", telemetryService.getTelemetryByDeviceId(deviceId));
        model.addAttribute("alerts", alertService.getAlertsByDeviceId(deviceId));
        model.addAttribute("thresholds", thresholdService.getThresholds(deviceId));
        model.addAttribute("commands", commandService.getCommandsByDeviceId(deviceId));
        return "device-detail";
    }
}
