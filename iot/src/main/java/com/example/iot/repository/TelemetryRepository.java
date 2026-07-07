package com.example.iot.repository;

import com.example.iot.dto.TelemetryRequest;
import java.util.List;
import java.util.Map;

/**
 * Interfaccia del repository dedicato alla persistenza delle telemetrie.
 * <p>
 * Definisce le operazioni di accesso ai dati necessarie per salvare e
 * recuperare le misure associate ai dispositivi.
 */
public interface TelemetryRepository {

	/**
	 * Salva una telemetria nel database.
	 *
	 * @param telemetry i dati di telemetria da persistere
	 */
	void saveTelemetry(TelemetryRequest telemetry);

	/**
	 * Restituisce tutte le telemetrie presenti nel database.
	 *
	 * @return la lista completa delle telemetrie
	 */
	List<Map<String, Object>> findAllTelemetry();

	/**
	 * Restituisce tutte le telemetrie associate a un dispositivo.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista delle telemetrie del dispositivo
	 */
	List<Map<String, Object>> findTelemetryByDeviceId(String deviceId);

	/**
	 * Restituisce l'ultima telemetria disponibile per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la telemetria più recente del dispositivo
	 */
	Map<String, Object> findLatestTelemetryByDeviceId(String deviceId);
}