package com.example.iot.controller;

import com.example.iot.service.AlertService;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

/**
 * Controller REST dedicato alla consultazione degli allarmi generati dal
 * sistema.
 * <p>
 * Espone endpoint per ottenere l'elenco completo degli allarmi oppure i soli
 * allarmi associati a uno specifico dispositivo.
 */
@RestController
@RequestMapping("/api/v1")
public class AlertController {

	/* sfrutta il service Alert */
	private final AlertService alertService;

	/**
	 * Costruisce il controller degli allarmi inizializzando il servizio applicativo
	 * dedicato alla loro gestione.
	 *
	 * @param alertService il servizio dedicato agli allarmi
	 */
	public AlertController(AlertService alertService) {
		this.alertService = alertService;
	}

	/**
	 * Restituisce tutti gli allarmi generati dal sistema.
	 *
	 * @return la lista completa degli allarmi memorizzati
	 */
	@GetMapping("/alerts")
	public List<Map<String, Object>> getAllAlerts() {
		return alertService.getAllAlerts();
	}

	/**
	 * Restituisce gli allarmi associati al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista degli allarmi del dispositivo
	 */
	@GetMapping("/devices/{deviceId}/alerts")
	public List<Map<String, Object>> getAlertsByDeviceId(@PathVariable String deviceId) {
		return alertService.getAlertsByDeviceId(deviceId);
	}
}
