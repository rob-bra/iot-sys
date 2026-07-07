package com.example.iot.repository.impl;

import com.example.iot.dto.TelemetryRequest;
import com.example.iot.repository.TelemetryRepository;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

/**
 * Implementazione concreta del repository dedicato alla gestione persistente
 * delle telemetrie.
 * <p>
 * Utilizza JDBC e {@code JdbcTemplate} per interagire con il database SQLite e
 * salvare o recuperare le misure dei dispositivi.
 */
@Repository
public class TelemetryRepositoryImpl implements TelemetryRepository {

	private final JdbcTemplate jdbcTemplate;

	/**
	 * Costruisce l'implementazione del repository della telemetria inizializzando
	 * il supporto JDBC utilizzato per l'accesso al database.
	 *
	 * @param jdbcTemplate il componente Spring utilizzato per eseguire query SQL
	 */
	public TelemetryRepositoryImpl(JdbcTemplate jdbcTemplate) {
		this.jdbcTemplate = jdbcTemplate;
	}

	/**
	 * Salva una telemetria nel database.
	 *
	 * @param telemetry i dati di telemetria da salvare
	 */
	@Override
	public void saveTelemetry(TelemetryRequest telemetry) {
		String sql = """
				INSERT INTO telemetry (device_id, timestamp, temperature, humidity, pressure, battery, orientation)
				VALUES (?, ?, ?, ?, ?, ?, ?)
				""";

		jdbcTemplate.update(sql, telemetry.getDeviceId(), telemetry.getTimestamp(), telemetry.getTemperature(),
				telemetry.getHumidity(), telemetry.getPressure(), telemetry.getBattery(), telemetry.getOrientation());
	}

	/**
	 * Restituisce tutte le telemetrie memorizzate nel database.
	 * 
	 * Lettura dei dati nel DB (file 'iot.db') di TUTTI I DISPOSITIVI che vengono
	 * salvati.
	 *
	 * @return la lista completa delle telemetrie
	 */
	@Override
	public List<Map<String, Object>> findAllTelemetry() {
		String sql = "SELECT * FROM telemetry ORDER BY id DESC"; // recupera tutte le righe e le ordina dalla piu'
																	// recente (desc=decrescente)
		return jdbcTemplate.queryForList(sql); // restituisce una lista di righe in cui ogi riga e' una
												// mappa<column_name,val>
												// quindi il JSON con tutti i valori delle colonne: "device_id": 3
												// "timestamp": "2026-06-25T11:06:00Z" ...
	}

	/**
	 * Restituisce tutte le telemetrie associate al dispositivo specificato.
	 * 
	 * Lettura dei dati nel DB di 1 SINGOLO DEVICE
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la lista delle telemetrie del dispositivo
	 */
	@Override
	public List<Map<String, Object>> findTelemetryByDeviceId(String deviceId) {
		String sql = "SELECT * FROM telemetry WHERE device_id = ? ORDER BY id DESC";
		return jdbcTemplate.queryForList(sql, deviceId);
	}

	/**
	 * Restituisce l'ultima telemetria registrata per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la telemetria più recente del dispositivo oppure null se assente
	 */
	@Override
	public Map<String, Object> findLatestTelemetryByDeviceId(String deviceId) {
		String sql = "SELECT * FROM telemetry WHERE device_id = ? ORDER BY id DESC LIMIT 1";
		List<Map<String, Object>> result = jdbcTemplate.queryForList(sql, deviceId);
		return result.isEmpty() ? null : result.get(0);
	}

}