package com.example.iot.repository;

import java.util.List;
import java.util.Map;

/**
 * Interfaccia del repository dedicato alla persistenza degli allarmi.
 * <p>
 * Definisce le operazioni necessarie per salvare e consultare gli allarmi
 * generati dal sistema.
 */
public interface AlertRepository {

	/**
	 * Salva un nuovo allarme nel database.
	 *
	 * @param deviceId       l'identificativo del dispositivo
	 * @param timestamp      il timestamp dell'evento
	 * @param type           il tipo di allarme
	 * @param measuredValue  il valore misurato
	 * @param thresholdValue la soglia violata
	 * @param message        il messaggio descrittivo dell'allarme
	 */
	void saveAlert(String deviceId, String timestamp, String type, Double measuredValue, Double thresholdValue,
			String message);

	/**
	 * Restituisce tutti gli allarmi presenti nel database.
	 *
	 * @return la lista completa degli allarmi
	 */
	List<Map<String, Object>> findAllAlerts();

	/**
	 * Restituisce tutti gli allarmi associati al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista degli allarmi del dispositivo
	 */
	List<Map<String, Object>> findAlertsByDeviceId(String deviceId);
}