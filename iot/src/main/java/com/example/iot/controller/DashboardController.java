package com.example.iot.controller;

import com.example.iot.dto.CommandRequest;
import com.example.iot.dto.ThresholdRequest;
import com.example.iot.service.AlertService;
import com.example.iot.service.AnalyticsService;
import com.example.iot.service.CommandService;
import com.example.iot.service.DeviceService;
import com.example.iot.service.TelemetryService;
import com.example.iot.service.ThresholdService;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;
import com.example.iot.service.AnalyticsService;

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
	private final AnalyticsService analyticsService;

	/**
	 * Costruisce il controller della dashboard inizializzando i servizi
	 * necessari alla gestione delle viste e delle operazioni utente.
	 *
	 * @param deviceService servizio per la gestione dei dispositivi
	 * @param telemetryService servizio per la gestione delle telemetrie
	 * @param alertService servizio per la gestione degli allarmi
	 * @param thresholdService servizio per la gestione delle soglie
	 * @param commandService servizio per la gestione dei comandi remoti
	 */
	public DashboardController(DeviceService deviceService, TelemetryService telemetryService,
			AlertService alertService, ThresholdService thresholdService, CommandService commandService, AnalyticsService analyticsService) {
		this.deviceService = deviceService;
		this.telemetryService = telemetryService;
		this.alertService = alertService;
		this.thresholdService = thresholdService;
		this.commandService = commandService;
		this.analyticsService = analyticsService;
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
	public String deviceDetailPage(@PathVariable String deviceId,
	                               @RequestParam(defaultValue = "5") int windowSize,
	                               Model model) {
		model.addAttribute("deviceId", deviceId);
		model.addAttribute("latestTelemetry", telemetryService.getLatestTelemetryByDeviceId(deviceId));
		model.addAttribute("measurements", telemetryService.getTelemetryByDeviceId(deviceId));
		model.addAttribute("alerts", alertService.getAlertsByDeviceId(deviceId));
		model.addAttribute("thresholds", thresholdService.getThresholds(deviceId));
		model.addAttribute("commands", commandService.getCommandsByDeviceId(deviceId));

		model.addAttribute("temperatureMovingAverage",
				analyticsService.getMovingAverage(deviceId, "temperature", windowSize));
		model.addAttribute("humidityMovingAverage",
				analyticsService.getMovingAverage(deviceId, "humidity", windowSize));
		model.addAttribute("pressureMovingAverage",
				analyticsService.getMovingAverage(deviceId, "pressure", windowSize));
		model.addAttribute("movingAverageWindowSize", windowSize);

		return "device-detail";
	}

	/**
	 * Gestisce la visualizzazione della pagina contenente tutti gli allarmi
	 * generati dal sistema.
	 *
	 * <p>
	 * Il metodo recupera la lista completa degli allarmi registrati e la inserisce
	 * nel modello, rendendola disponibile alla vista che li mostra in forma
	 * tabellare.
	 * </p>
	 *
	 * @param model il modello utilizzato per passare i dati alla vista
	 * @return il nome della vista che mostra l'elenco globale degli allarmi
	 */
	@GetMapping("/alerts-page")
	public String alertsPage(Model model) {
		model.addAttribute("alerts", alertService.getAllAlerts());
		return "alerts";
	}

	/**
	 * Salva o aggiorna la configurazione delle soglie di un device specifico.
	 *
	 * <p>
	 * Il metodo costruisce un oggetto {@code ThresholdRequest} a partire dai
	 * parametri ricevuti dal form HTML e lo inoltra al relativo servizio
	 * applicativo per il salvataggio. Al termine effettua un redirect verso la
	 * pagina di dettaglio del dispositivo.
	 * </p>
	 *
	 * @param deviceId       identificativo del dispositivo a cui associare le
	 *                       soglie
	 * @param temperatureMin soglia minima di temperatura
	 * @param temperatureMax soglia massima di temperatura
	 * @param humidityMin    soglia minima di umidità
	 * @param humidityMax    soglia massima di umidità
	 * @param pressureMin    soglia minima di pressione
	 * @param pressureMax    soglia massima di pressione
	 * @return una redirect verso la pagina di dettaglio del dispositivo
	 */
	@PostMapping("/devices-page/{deviceId}/thresholds")
	public String saveThresholds(@PathVariable String deviceId, @RequestParam(required = false) Double temperatureMin,
			@RequestParam(required = false) Double temperatureMax, @RequestParam(required = false) Double humidityMin,
			@RequestParam(required = false) Double humidityMax, @RequestParam(required = false) Double pressureMin,
			@RequestParam(required = false) Double pressureMax) {

		ThresholdRequest request = new ThresholdRequest();
		request.setTemperatureMin(temperatureMin);
		request.setTemperatureMax(temperatureMax);
		request.setHumidityMin(humidityMin);
		request.setHumidityMax(humidityMax);
		request.setPressureMin(pressureMin);
		request.setPressureMax(pressureMax);

		thresholdService.saveThresholds(deviceId, request);
		return "redirect:/devices-page/" + deviceId;
	}

	/**
	 * Gestisce la creazione di un comando remoto da inviare a un dispositivo.
	 *
	 * <p>
	 * Il metodo riceve dal form HTML il tipo di comando e un eventuale payload
	 * associato, costruisce un oggetto {@code CommandRequest} e lo inoltra al
	 * servizio che si occupa della gestione dei comandi remoti. Al termine effettua
	 * un redirect verso la pagina di dettaglio del dispositivo.
	 * </p>
	 *
	 * @param deviceId identificativo del dispositivo destinatario del comando
	 * @param type     tipo del comando remoto da inviare
	 * @param payload  eventuale payload associato al comando
	 * @return una redirect verso la pagina di dettaglio del dispositivo
	 */
	@PostMapping("/devices-page/{deviceId}/commands")
	public String createCommand(@PathVariable String deviceId, @RequestParam String type,
			@RequestParam(required = false) String payload) {

		CommandRequest request = new CommandRequest();
		request.setType(type);
		request.setPayload(payload);

		commandService.createCommand(deviceId, request);
		return "redirect:/devices-page/" + deviceId;
	}

	/**
	 * Gestisce la richiesta rapida di acquisizione immediata di una misura da parte
	 * di uno specifico dispositivo.
	 *
	 * <p>
	 * Il metodo crea automaticamente un comando remoto di tipo
	 * {@code FORCE_MEASUREMENT} con payload predefinito, demandando poi al servizio
	 * applicativo la sua memorizzazione come comando pendente. Al termine effettua
	 * un redirect verso la pagina di dettaglio del dispositivo.
	 * </p>
	 *
	 * @param deviceId identificativo del dispositivo destinatario del comando
	 * @return una redirect verso la pagina di dettaglio del dispositivo
	 */
	@PostMapping("/devices-page/{deviceId}/force-measurement")
	public String forceMeasurement(@PathVariable String deviceId) {
		CommandRequest request = new CommandRequest();
		request.setType("FORCE_MEASUREMENT");
		request.setPayload("immediate");

		commandService.createCommand(deviceId, request);
		return "redirect:/devices-page/" + deviceId;
	}

}
