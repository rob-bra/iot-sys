package com.example.iot.service;

import com.example.iot.dto.TelemetryRequest;
import java.util.List;
import java.util.Map;

/**
 * Interfaccia del service layer dedicata alla gestione degli allarmi.
 * <p>
 * Definisce le operazioni per valutare una telemetria rispetto alle soglie
 * configurate e per consultare gli allarmi generati dal sistema.
 */
public interface AlertService {

	/**
	 * Valuta una telemetria rispetto alle soglie configurate ed eventualmente
	 * genera uno o più allarmi.
	 *
	 * @param telemetry la telemetria da analizzare
	 */
	void evaluateAlerts(TelemetryRequest telemetry);

	/**
	 * Restituisce tutti gli allarmi presenti nel sistema.
	 *
	 * @return la lista completa degli allarmi
	 */
	List<Map<String, Object>> getAllAlerts();

	/**
	 * Restituisce gli allarmi associati a un determinato dispositivo.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista degli allarmi del dispositivo
	 */
	List<Map<String, Object>> getAlertsByDeviceId(String deviceId);
}