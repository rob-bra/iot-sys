package com.example.iot.service;

import com.example.iot.dto.ThresholdRequest;
import java.util.Map;

/**
 * Interfaccia del service layer dedicata alla gestione delle soglie.
 * <p>
 * Definisce le operazioni per salvare, aggiornare e leggere la configurazione
 * delle soglie di allarme dei dispositivi.
 */
public interface ThresholdService {

	/**
	 * Salva o aggiorna le soglie di allarme del dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param request  il payload contenente le soglie da memorizzare
	 */
	void saveThresholds(String deviceId, ThresholdRequest request);

	/**
	 * Restituisce le soglie configurate per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return le soglie configurate, oppure null se assenti
	 */
	Map<String, Object> getThresholds(String deviceId);
}
