package com.example.iot.repository;

import com.example.iot.dto.ThresholdRequest;
import java.util.Map;

/**
 * Interfaccia del repository dedicato alla persistenza delle soglie.
 * <p>
 * Definisce le operazioni necessarie per salvare e recuperare le configurazioni
 * di soglia associate ai dispositivi.
 */
public interface ThresholdRepository {

	/**
	 * Inserisce o aggiorna le soglie di allarme del dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param request  il payload contenente le soglie da memorizzare
	 */
	void upsertThresholds(String deviceId, ThresholdRequest request);

	/**
	 * Restituisce le soglie configurate per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la configurazione delle soglie oppure null se assente
	 */
	Map<String, Object> findThresholdsByDeviceId(String deviceId);
}