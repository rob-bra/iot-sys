package com.example.iot.service;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.dto.PendingCommandResponse;

import java.util.List;
import java.util.Map;

/**
 * Interfaccia del service layer dedicata alla gestione dei comandi remoti.
 * <p>
 * Definisce le operazioni per creare comandi, consultarne lo stato e registrare
 * gli acknowledgement inviati dai dispositivi.
 */
public interface CommandService {

	/**
	 * Crea un nuovo comando remoto destinato al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param request  il payload contenente il comando da inviare
	 */
	void createCommand(String deviceId, CommandRequest request);

	/**
	 * Restituisce tutti i comandi associati al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi del dispositivo
	 */
	List<Map<String, Object>> getCommandsByDeviceId(String deviceId);

	/**
	 * Restituisce i comandi ancora pendenti per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi con stato pendente
	 */
	List<Map<String, Object>> getPendingCommandsByDeviceId(String deviceId);

	/**
	 * Registra l'esito dell'esecuzione di un comando remoto.
	 *
	 * @param commandId  l'identificativo del comando
	 * @param ackRequest il payload contenente l'esito dell'esecuzione
	 */
	void ackCommand(Long commandId, CommandAckRequest ackRequest);
	
	/**
	 * Restituisce i comandi pendenti associati a uno specifico dispositivo sotto
	 * forma di DTO di risposta dedicato all'API REST.
	 * <p>
	 * Questo metodo consente di esporre verso il firmware embedded una struttura
	 * JSON più pulita e stabile rispetto alla rappresentazione interna basata su
	 * mappe e nomi di colonne del database.
	 * </p>
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi con stato {@code PENDING} destinati al dispositivo
	 */
	List<PendingCommandResponse> getPendingCommandDtosByDeviceId(String deviceId);
}
