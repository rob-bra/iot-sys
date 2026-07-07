package com.example.iot.service;

import java.util.List;
import java.util.Map;

/**
 * Interfaccia del service layer dedicata alla gestione dei dispositivi.
 * <p>
 * Definisce le operazioni applicative necessarie alla consultazione delle
 * informazioni di stato dei device registrati.
 */
public interface DeviceService {

	/**
	 * Restituisce l'elenco di tutti i dispositivi registrati.
	 *
	 * @return la lista dei dispositivi presenti nel sistema
	 */
	List<Map<String, Object>> getAllDevices();
}