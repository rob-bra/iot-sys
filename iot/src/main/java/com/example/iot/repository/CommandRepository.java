package com.example.iot.repository;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import java.util.List;
import java.util.Map;

/**
 * Interfaccia del repository dedicato alla persistenza dei comandi remoti.
 * <p>
 * Definisce le operazioni necessarie per creare, consultare e aggiornare lo
 * stato dei comandi inviati ai dispositivi.
 */
public interface CommandRepository {

	/**
	 * Salva un nuovo comando remoto nel database con stato iniziale pendente.
	 *
	 * @param deviceId  l'identificativo del dispositivo destinatario
	 * @param request   il payload del comando
	 * @param createdAt il timestamp di creazione del comando
	 */
	void createCommand(String deviceId, CommandRequest request, String createdAt);

	/**
	 * Restituisce tutti i comandi associati al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista completa dei comandi del dispositivo
	 */
	List<Map<String, Object>> findCommandsByDeviceId(String deviceId);

	/**
	 * Restituisce i comandi pendenti del dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi con stato PENDING
	 */
	List<Map<String, Object>> findPendingCommandsByDeviceId(String deviceId);

	/**
	 * Aggiorna nel database lo stato di un comando remoto a seguito
	 * dell'acknowledgement ricevuto dal dispositivo.
	 *
	 * @param commandId  l'identificativo del comando
	 * @param ackRequest il payload contenente lo stato finale del comando
	 */
	void ackCommand(Long commandId, CommandAckRequest ackRequest);
}
