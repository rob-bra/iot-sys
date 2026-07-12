package com.example.iot.controller;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.dto.PendingCommandResponse;
import com.example.iot.service.CommandService;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

/**
 * Controller REST dedicato alla gestione dei comandi remoti inviati ai
 * dispositivi.
 * <p>
 * Espone endpoint per la creazione dei comandi, la consultazione dello storico
 * e dei comandi pendenti, nonché per la ricezione degli acknowledgement inviati
 * dal device.
 */
@RestController
@RequestMapping("/api/v1")
public class CommandController {

	private final CommandService commandService;

	/**
	 * Costruisce il controller dei comandi remoti inizializzando il servizio
	 * applicativo dedicato alla loro gestione.
	 *
	 * @param commandService il servizio dedicato ai comandi remoti
	 */
	public CommandController(CommandService commandService) {
		this.commandService = commandService;
	}

	/**
	 * Crea un nuovo comando remoto (inviabile da dashboard) destinato al
	 * dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo destinatario del comando
	 * @param request  il payload contenente tipo e contenuto del comando
	 * @return una mappa con l'esito della creazione del comando
	 */
	@PostMapping("/devices/{deviceId}/commands")
	public Map<String, String> createCommand(@PathVariable String deviceId, @RequestBody CommandRequest request) {
		commandService.createCommand(deviceId, request);
		return Map.of("status", "ok", "message", "Command created successfully");
	}

	/**
	 * Restituisce tutti i comandi associati al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi registrati per il dispositivo
	 */
	@GetMapping("/devices/{deviceId}/commands")
	public List<Map<String, Object>> getCommandsByDeviceId(@PathVariable String deviceId) {
		return commandService.getCommandsByDeviceId(deviceId);
	}

	/**
	 * Restituisce i comandi ancora pendenti per il dispositivo specificato sotto
	 * forma di DTO dedicati alla risposta REST.
	 * <p>
	 * Questo endpoint è pensato principalmente per il polling eseguito dal firmware
	 * del dispositivo embedded, che interroga periodicamente il backend per
	 * recuperare eventuali comandi remoti ancora da eseguire.
	 * </p>
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi con stato {@code PENDING} destinati al dispositivo
	 */
	@GetMapping("/devices/{deviceId}/commands/pending")
	public List<PendingCommandResponse> getPendingCommandsByDeviceId(@PathVariable String deviceId) {
		return commandService.getPendingCommandDtosByDeviceId(deviceId);
	}

	/**
	 * Registra l'esito dell'esecuzione di un comando remoto inviato al dispositivo.
	 *
	 * @param deviceId  l'identificativo del dispositivo
	 * @param commandId l'identificativo del comando da aggiornare
	 * @param request   il payload contenente stato, istante di ack e messaggio di
	 *                  risultato
	 * @return una mappa con l'esito dell'aggiornamento del comando
	 */
	@PostMapping("/devices/{deviceId}/commands/{commandId}/ack")
	public Map<String, String> ackCommand(@PathVariable String deviceId, @PathVariable Long commandId,
			@RequestBody CommandAckRequest request) {
		commandService.ackCommand(commandId, request);
		return Map.of("status", "ok", "message", "Command acknowledged successfully");
	}
}
