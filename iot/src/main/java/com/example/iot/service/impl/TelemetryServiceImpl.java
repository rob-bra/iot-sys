package com.example.iot.service.impl;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.repository.DeviceRepository;
import com.example.iot.repository.TelemetryRepository;
import com.example.iot.service.AlertService;
import com.example.iot.service.TelemetryService;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Service;

/**
 * Implementazione concreta del service dedicato alla gestione della telemetria.
 * <p>
 * Coordina il salvataggio delle misure, l'aggiornamento dello stato del
 * dispositivo e la valutazione degli allarmi associati ai dati ricevuti.
 */
@Service
public class TelemetryServiceImpl implements TelemetryService {

	private final TelemetryRepository telemetryRepository; // obj per interagire col DB --> TelemetryRepository
	private final DeviceRepository deviceRepository;
	private final AlertService alertService;

	/**
	 * Costruisce l'implementazione del servizio di telemetria inizializzando i
	 * repository e i servizi ausiliari necessari alla gestione delle misure.
	 *
	 * @param telemetryRepository il repository della telemetria
	 * @param deviceRepository    il repository dei dispositivi
	 * @param alertService        il servizio dedicato alla valutazione degli
	 *                            allarmi
	 */
	public TelemetryServiceImpl(TelemetryRepository telemetryRepository, DeviceRepository deviceRepository,
			AlertService alertService) {
		this.telemetryRepository = telemetryRepository;
		this.deviceRepository = deviceRepository;
		this.alertService = alertService;
	}

	/**
	 * Salva la telemetria ricevuta nel DB, aggiorna lo stato del dispositivo e
	 * valuta l'eventuale generazione di allarmi.
	 *
	 * @param telemetryRequest i dati di telemetria ricevuti dal device
	 */
	@Override
	public void saveTelemetry(TelemetryRequest telemetryRequest) {
		telemetryRepository.saveTelemetry(telemetryRequest);
		deviceRepository.upsertDevice(telemetryRequest.getDeviceId(), telemetryRequest.getBattery(),
				telemetryRequest.getTimestamp());
		alertService.evaluateAlerts(telemetryRequest);
	}

	/**
	 * Restituisce tutte le telemetrie memorizzate nel sistema.
	 *
	 * @return la lista completa delle telemetrie
	 */
	@Override
	public List<Map<String, Object>> getAllTelemetry() {
		return telemetryRepository.findAllTelemetry();
	}
	
	/**
	 * Restituisce l'ultima telemetria registrata per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la telemetria più recente del dispositivo
	 */
	@Override
	public Map<String, Object> getLatestTelemetryByDeviceId(String deviceId) {
		return telemetryRepository.findLatestTelemetryByDeviceId(deviceId);
	}

	/**
	 * Restituisce lo storico delle telemetrie del dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista delle telemetrie del dispositivo
	 */
	@Override
	public List<Map<String, Object>> getTelemetryByDeviceId(String deviceId) {
		return telemetryRepository.findTelemetryByDeviceId(deviceId);
	}
}
