package com.example.iot.service.impl;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.dto.PendingCommandResponse;
import com.example.iot.repository.CommandRepository;
import com.example.iot.service.CommandService;

import org.springframework.stereotype.Service;

import java.time.Instant;
import java.util.List;
import java.util.Map;

/**
 * Implementazione concreta del service dedicato ai comandi remoti.
 * <p>
 * Gestisce la creazione, la consultazione e l'aggiornamento dello stato dei
 * comandi inviati ai dispositivi.
 */
@Service
public class CommandServiceImpl implements CommandService {

	private final CommandRepository commandRepository;

	/**
	 * Costruisce l'implementazione del servizio dedicato ai comandi remoti.
	 *
	 * @param commandRepository il repository dei comandi remoti
	 */
	public CommandServiceImpl(CommandRepository commandRepository) {
		this.commandRepository = commandRepository;
	}

	/**
	 * Crea un nuovo comando remoto per il dispositivo specificato, valorizzando
	 * automaticamente l'istante di creazione.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param request  il payload contenente il comando da registrare
	 */
	@Override
	public void createCommand(String deviceId, CommandRequest request) {
		String createdAt = Instant.now().toString();
		commandRepository.createCommand(deviceId, request, createdAt);
	}

	/**
	 * Restituisce tutti i comandi associati al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi del dispositivo
	 */
	@Override
	public List<Map<String, Object>> getCommandsByDeviceId(String deviceId) {
		return commandRepository.findCommandsByDeviceId(deviceId);
	}

	/**
	 * Restituisce i comandi con stato pendente per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi pendenti
	 */
	@Override
	public List<Map<String, Object>> getPendingCommandsByDeviceId(String deviceId) {
		return commandRepository.findPendingCommandsByDeviceId(deviceId);
	}

	/**
	 * Registra l'esito dell'esecuzione di un comando remoto.
	 *
	 * @param commandId  l'identificativo del comando
	 * @param ackRequest il payload contenente stato, timestamp di ack e messaggio
	 *                   di risultato
	 */
	@Override
	public void ackCommand(Long commandId, CommandAckRequest ackRequest) {
		commandRepository.ackCommand(commandId, ackRequest);
	}
	
	/**
	 * Restituisce i comandi pendenti associati al dispositivo specificato,
	 * convertendo i dati recuperati dal repository in DTO orientati all'API REST.
	 * <p>
	 * Il repository restituisce attualmente una lista di mappe contenenti i nomi
	 * delle colonne del database. Questo metodo si occupa di trasformare tali
	 * risultati in oggetti {@link com.example.iot.dto.PendingCommandResponse},
	 * esponendo una struttura più pulita e indipendente dal layer di persistenza.
	 * </p>
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi pendenti del dispositivo in formato DTO
	 */
	@Override
	public List<PendingCommandResponse> getPendingCommandDtosByDeviceId(String deviceId) {
	    List<Map<String, Object>> rows = commandRepository.findPendingCommandsByDeviceId(deviceId);

	    return rows.stream()
	            .map(row -> new PendingCommandResponse(
	                    row.get("id") != null ? ((Number) row.get("id")).longValue() : null,
	                    (String) row.get("device_id"),
	                    (String) row.get("type"),
	                    (String) row.get("payload"),
	                    (String) row.get("status"),
	                    (String) row.get("created_at"),
	                    (String) row.get("ack_at"),
	                    (String) row.get("result_message")
	            ))
	            .toList();
	}
}
