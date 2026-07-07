package com.example.iot.controller;

import com.example.iot.dto.ThresholdRequest;
import com.example.iot.service.ThresholdService;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

/**
 * Controller REST responsabile della configurazione e consultazione delle
 * soglie di allarme associate ai dispositivi.
 * <p>
 * Espone endpoint per salvare o aggiornare le soglie e per leggerne la
 * configurazione corrente.
 */
@RestController
@RequestMapping("/api/v1")
public class ThresholdController {

	private final ThresholdService thresholdService;

	/**
	 * Costruisce il controller delle soglie inizializzando il servizio applicativo
	 * che ne gestisce configurazione e consultazione.
	 *
	 * @param thresholdService il servizio dedicato alle soglie di allarme
	 */
	public ThresholdController(ThresholdService thresholdService) {
		this.thresholdService = thresholdService;
	}

	/**
	 * Salva o aggiorna le soglie di allarme associate al dispositivo specificato
	 * mediante metodo PUT (setta i valori soglia)
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param request  il payload contenente le soglie da configurare
	 * @return una mappa con l'esito dell'operazione
	 */
	@PutMapping("/devices/{deviceId}/thresholds")
	public Map<String, String> saveThresholds(@PathVariable String deviceId, @RequestBody ThresholdRequest request) {
		thresholdService.saveThresholds(deviceId, request);
		return Map.of("status", "ok", "message", "Thresholds saved successfully");
	}

	/**
	 * Restituisce le soglie configurate per il dispositivo specificato (metodo
	 * GET).
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return una mappa contenente le soglie configurate, oppure null se assenti
	 */
	@GetMapping("/devices/{deviceId}/thresholds")
	public Map<String, Object> getThresholds(@PathVariable String deviceId) {
		return thresholdService.getThresholds(deviceId);
	}
}