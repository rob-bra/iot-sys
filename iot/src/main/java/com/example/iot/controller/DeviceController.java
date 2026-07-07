package com.example.iot.controller;

import com.example.iot.service.DeviceService;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

/**
 * Controller REST dedicato alla gestione delle informazioni relative ai
 * dispositivi registrati nel sistema.
 * <p>
 * Espone endpoint per la consultazione dello stato e dell'elenco dei
 * dispositivi noti al backend.
 */
@RestController // @RestController = restituzione in formato REST --> JSON
@RequestMapping("/api/v1")
public class DeviceController {

	private final DeviceService deviceService;

	/**
	 * Costruisce il controller dei dispositivi inizializzando il servizio
	 * applicativo dedicato alla loro gestione.
	 *
	 * @param deviceService il servizio dedicato ai dispositivi
	 */
	public DeviceController(DeviceService deviceService) {
		this.deviceService = deviceService;
	}

	/**
	 * Restituisce l'elenco di tutti i dispositivi registrati nel sistema e le
	 * relative informazioni di stato.
	 *
	 * @return la lista dei dispositivi presenti nel database
	 */
	@GetMapping("/devices")
	public List<Map<String, Object>> getAllDevices() {
		return deviceService.getAllDevices();
	}
}
