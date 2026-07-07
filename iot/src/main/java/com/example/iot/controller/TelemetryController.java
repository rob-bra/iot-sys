package com.example.iot.controller;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.service.TelemetryService;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

/**
 * Controller REST responsabile della gestione delle API di telemetria.
 * <p>
 * Espone endpoint per la ricezione delle misure inviate dai dispositivi e per
 * la consultazione delle telemetrie memorizzate, sia globalmente sia per
 * singolo dispositivo.
 */
@RestController // dice che questa classe gestisce richieste web REST
@RequestMapping("/api/v1") // dice che tutti gli endpoint qui dentro sono raggiungibili da questo prefisso
public class TelemetryController {

	private final TelemetryService telemetryService;

	/**
	 * Costruisce il controller della telemetria inizializzando il servizio
	 * applicativo necessario alla gestione delle richieste REST.
	 *
	 * @param telemetryService il servizio dedicato alla gestione della telemetria
	 */
	public TelemetryController(TelemetryService telemetryService) {
		this.telemetryService = telemetryService;
	}

	/**
	 * Riceve una telemetria dal dispositivo e la inoltra al service layer per il
	 * salvataggio persistente e l'eventuale generazione di allarmi.
	 * 
	 * Espone l'endpoint specifico POST "/api/v1/telemetry" per la ricezione di file
	 * JSON.
	 *
	 * @param request il payload contenente i dati di telemetria inviati dal device
	 * @return una mappa con esito dell'operazione di salvataggio
	 */
	@PostMapping("/telemetry")
	public Map<String, String> receiveTelemetry(@RequestBody TelemetryRequest request) {
		telemetryService.saveTelemetry(request); // salva nel DB il JSON che arriva
		return Map.of("status", "ok", "message", "Telemetry saved successfully");
	}

	/**
	 * Restituisce tutte le telemetrie salvate nel sistema, ordinate per
	 * identificativo decrescente.
	 * 
	 * Espone stesso endpoint ma con metodo GET per acquisire informazioni
	 *
	 * @return la lista completa delle telemetrie memorizzate
	 */
	@GetMapping("/telemetry")
	public List<Map<String, Object>> getAllTelemetry() {
		return telemetryService.getAllTelemetry();
	}

	/**
	 * Restituisce l'ultima telemetria disponibile per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la telemetria più recente del dispositivo, oppure null se non
	 *         presente
	 */
	@GetMapping("/devices/{deviceId}/latest")
	public Map<String, Object> getLatestTelemetry(@PathVariable String deviceId) {
		return telemetryService.getLatestTelemetryByDeviceId(deviceId);
	}

	/**
	 * Restituisce lo storico delle telemetrie associate al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista delle misure memorizzate per il dispositivo
	 */
	@GetMapping("/devices/{deviceId}/measurements")
	public List<Map<String, Object>> getTelemetryByDeviceId(@PathVariable String deviceId) {
		return telemetryService.getTelemetryByDeviceId(deviceId);
	}
}