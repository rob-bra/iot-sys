package com.example.iot.service;

import com.example.iot.dto.TelemetryRequest;
import java.util.List;
import java.util.Map;

/**
 * Interfaccia del service layer dedicata alla gestione della telemetria.
 * <p>
 * Definisce le operazioni applicative per il salvataggio e la consultazione
 * delle misure inviate dai dispositivi.
 */
public interface TelemetryService {

	/**
	 * Salva una nuova telemetria e attiva la logica applicativa associata, inclusi
	 * aggiornamento del device e valutazione degli allarmi.
	 *
	 * @param telemetryRequest i dati di telemetria ricevuti
	 */
	void saveTelemetry(TelemetryRequest telemetryRequest);

	/**
	 * Restituisce tutte le telemetrie presenti nel sistema.
	 *
	 * @return la lista completa delle telemetrie memorizzate
	 */
	List<Map<String, Object>> getAllTelemetry();

	/**
	 * Restituisce tutte le telemetrie associate al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista delle telemetrie del dispositivo
	 */
	List<Map<String, Object>> getTelemetryByDeviceId(String deviceId);

	/**
	 * Restituisce l'ultima telemetria disponibile per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la telemetria più recente del dispositivo
	 */
	Map<String, Object> getLatestTelemetryByDeviceId(String deviceId);
}
