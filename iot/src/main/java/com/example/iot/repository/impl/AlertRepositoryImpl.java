package com.example.iot.repository.impl;

import com.example.iot.repository.AlertRepository;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

/**
 * Implementazione concreta del repository dedicato alla gestione persistente
 * degli allarmi.
 * <p>
 * Utilizza JDBC e {@code JdbcTemplate} per salvare gli eventi di allarme e
 * recuperarli dal database SQLite.
 */
@Repository
public class AlertRepositoryImpl implements AlertRepository {

	private final JdbcTemplate jdbcTemplate;

	/**
	 * Costruisce l'implementazione del repository degli allarmi inizializzando il
	 * supporto JDBC per la persistenza nel database.
	 *
	 * @param jdbcTemplate il componente Spring utilizzato per eseguire query SQL
	 */
	public AlertRepositoryImpl(JdbcTemplate jdbcTemplate) {
		this.jdbcTemplate = jdbcTemplate;
	}

	/**
	 * Salva un nuovo allarme nel database.
	 *
	 * @param deviceId       l'identificativo del dispositivo
	 * @param timestamp      il timestamp dell'evento
	 * @param type           il tipo di allarme
	 * @param measuredValue  il valore misurato
	 * @param thresholdValue la soglia superata
	 * @param message        il messaggio descrittivo dell'allarme
	 */
	@Override
	public void saveAlert(String deviceId, String timestamp, String type, Double measuredValue, Double thresholdValue,
			String message) {
		String sql = """
				INSERT INTO alerts (device_id, timestamp, type, measured_value, threshold_value, message)
				VALUES (?, ?, ?, ?, ?, ?)
				""";

		jdbcTemplate.update(sql, deviceId, timestamp, type, measuredValue, thresholdValue, message);
	}

	/**
	 * Restituisce tutti gli allarmi memorizzati nel database.
	 *
	 * @return la lista completa degli allarmi
	 */
	@Override
	public List<Map<String, Object>> findAllAlerts() {
		String sql = "SELECT * FROM alerts ORDER BY id DESC";
		return jdbcTemplate.queryForList(sql);
	}

	/**
	 * Restituisce tutti gli allarmi associati a un dispositivo.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista degli allarmi del dispositivo
	 */
	@Override
	public List<Map<String, Object>> findAlertsByDeviceId(String deviceId) {
		String sql = "SELECT * FROM alerts WHERE device_id = ? ORDER BY id DESC";
		return jdbcTemplate.queryForList(sql, deviceId);
	}

}
