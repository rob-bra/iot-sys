package com.example.iot.repository;

import java.util.List;
import java.util.Map;

/* REPOSITORY per la tabella devices */
/**
 * Interfaccia del repository dedicato alla persistenza delle informazioni
 * relative ai dispositivi.
 * <p>
 * Definisce le operazioni necessarie per inserire, aggiornare e consultare lo
 * stato dei device.
 */
public interface DeviceRepository {

	/**
	 * Inserisce o aggiorna il record relativo a un dispositivo.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param battery  il livello di batteria rilevato
	 * @param lastSeen il timestamp dell'ultimo contatto ricevuto
	 */
	void upsertDevice(String deviceId, Integer battery, String lastSeen);

	/**
	 * Restituisce tutti i dispositivi registrati nel database.
	 *
	 * @return la lista dei dispositivi
	 */
	List<Map<String, Object>> findAllDevices();
}