package com.example.iot.controller;

import com.example.iot.service.AlertService;
import com.example.iot.service.CommandService;
import com.example.iot.service.DeviceService;
import com.example.iot.service.TelemetryService;
import com.example.iot.service.ThresholdService;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;

/**
 * Controller MVC responsabile della navigazione web della dashboard.
 * <p>
 * Prepara i dati necessari alle viste Thymeleaf e gestisce la navigazione tra
 * home page, elenco dispositivi e pagina di dettaglio del singolo dispositivo.
 */
@Controller
public class DashboardController {

	/* Controller a Dashboard per ogni componente */
	private final DeviceService deviceService;
	private final TelemetryService telemetryService;
	private final AlertService alertService;
	private final ThresholdService thresholdService;
	private final CommandService commandService;

	/**
	 * Costruisce il controller MVC della dashboard inizializzando tutti i servizi
	 * necessari alla composizione delle viste web.
	 *
	 * @param deviceService    il servizio dedicato ai dispositivi
	 * @param telemetryService il servizio dedicato alla telemetria
	 * @param alertService     il servizio dedicato agli allarmi
	 * @param thresholdService il servizio dedicato alle soglie
	 * @param commandService   il servizio dedicato ai comandi remoti
	 */
	public DashboardController(DeviceService deviceService, TelemetryService telemetryService,
			AlertService alertService, ThresholdService thresholdService, CommandService commandService) {
		this.deviceService = deviceService;
		this.telemetryService = telemetryService;
		this.alertService = alertService;
		this.thresholdService = thresholdService;
		this.commandService = commandService;
	}

	/**
	 * Restituisce la home page della dashboard web.
	 *
	 * @return il nome del template Thymeleaf della home
	 */
	@GetMapping("/")
	public String home() {
		return "index";
	}

	/**
	 * Restituisce la pagina contenente l'elenco dei dispositivi registrati.
	 *
	 * @param model il model utilizzato per passare i dati alla vista
	 * @return il nome del template Thymeleaf della pagina dispositivi
	 */
	@GetMapping("/devices-page")
	public String devicesPage(Model model) {
		model.addAttribute("devices", deviceService.getAllDevices());
		return "devices";
	}

	/**
	 * Restituisce la pagina di dettaglio di un dispositivo, comprensiva di ultima
	 * telemetria, storico misure, allarmi, soglie e comandi.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param model    il model utilizzato per passare i dati alla vista
	 * @return il nome del template Thymeleaf della pagina di dettaglio dispositivo
	 */
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
