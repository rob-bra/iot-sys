package com.example.iot.repository.impl;

import com.example.iot.dto.CommandAckRequest;
import com.example.iot.dto.CommandRequest;
import com.example.iot.repository.CommandRepository;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

/**
 * Implementazione concreta del repository dedicato alla gestione persistente
 * dei comandi remoti.
 * <p>
 * Utilizza JDBC e {@code JdbcTemplate} per creare comandi, recuperarne lo stato
 * e registrarne l'esito nel database SQLite.
 */
@Repository
public class CommandRepositoryImpl implements CommandRepository {

	private final JdbcTemplate jdbcTemplate;

	/**
	 * Costruisce l'implementazione del repository dei comandi remoti inizializzando
	 * il supporto JDBC per la persistenza dei dati.
	 *
	 * @param jdbcTemplate il componente Spring utilizzato per eseguire query SQL
	 */
	public CommandRepositoryImpl(JdbcTemplate jdbcTemplate) {
		this.jdbcTemplate = jdbcTemplate;
	}

	/**
	 * Salva un nuovo comando remoto nel database con stato iniziale pendente.
	 *
	 * @param deviceId  l'identificativo del dispositivo destinatario
	 * @param request   il payload del comando
	 * @param createdAt l'istante di creazione del comando
	 */
	@Override
	public void createCommand(String deviceId, CommandRequest request, String createdAt) {
		String sql = """
				INSERT INTO commands (device_id, type, payload, status, created_at)
				VALUES (?, ?, ?, 'PENDING', ?)
				""";

		jdbcTemplate.update(sql, deviceId, request.getType(), request.getPayload(), createdAt);
	}

	/**
	 * Restituisce tutti i comandi associati al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista completa dei comandi del dispositivo
	 */
	@Override
	public List<Map<String, Object>> findCommandsByDeviceId(String deviceId) {
		String sql = "SELECT * FROM commands WHERE device_id = ? ORDER BY id DESC";
		return jdbcTemplate.queryForList(sql, deviceId);
	}

	/**
	 * Restituisce tutti i comandi pendenti per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista dei comandi ancora da eseguire
	 */
	@Override
	public List<Map<String, Object>> findPendingCommandsByDeviceId(String deviceId) {
		String sql = "SELECT * FROM commands WHERE device_id = ? AND status = 'PENDING' ORDER BY id ASC";
		return jdbcTemplate.queryForList(sql, deviceId);
	}

	/**
	 * Aggiorna l'esito di un comando remoto a seguito dell'ack ricevuto dal
	 * dispositivo.
	 *
	 * @param commandId  l'identificativo del comando
	 * @param ackRequest il payload contenente esito e dettagli dell'esecuzione
	 */
	@Override
	public void ackCommand(Long commandId, CommandAckRequest ackRequest) {
		String sql = """
				UPDATE commands
				SET status = ?, ack_at = ?, result_message = ?
				WHERE id = ?
				""";

		jdbcTemplate.update(sql, ackRequest.getStatus(), ackRequest.getAckAt(), ackRequest.getResultMessage(),
				commandId);
	}
}
