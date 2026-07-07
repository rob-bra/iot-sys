package com.example.iot.repository.impl;

import com.example.iot.dto.ThresholdRequest;
import com.example.iot.repository.ThresholdRepository;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

/**
 * Implementazione concreta del repository dedicato alla gestione persistente
 * delle soglie di allarme.
 * <p>
 * Utilizza JDBC e {@code JdbcTemplate} per inserire, aggiornare e leggere la
 * configurazione delle soglie dal database SQLite.
 */
@Repository
public class ThresholdRepositoryImpl implements ThresholdRepository {

	private final JdbcTemplate jdbcTemplate;

	/**
	 * Costruisce l'implementazione del repository delle soglie inizializzando il
	 * supporto JDBC per l'accesso al database.
	 *
	 * @param jdbcTemplate il componente Spring utilizzato per eseguire query SQL
	 */
	public ThresholdRepositoryImpl(JdbcTemplate jdbcTemplate) {
		this.jdbcTemplate = jdbcTemplate;
	}

	/**
	 * Inserisce o aggiorna le soglie associate al dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param request  il payload contenente le soglie da memorizzare
	 */
	@Override
	public void upsertThresholds(String deviceId, ThresholdRequest request) {
		String sql = """
				INSERT INTO thresholds (
				    device_id,
				    temperature_min,
				    temperature_max,
				    humidity_min,
				    humidity_max,
				    pressure_min,
				    pressure_max
				)
				VALUES (?, ?, ?, ?, ?, ?, ?)
				ON CONFLICT(device_id) DO UPDATE SET
				    temperature_min = excluded.temperature_min,
				    temperature_max = excluded.temperature_max,
				    humidity_min = excluded.humidity_min,
				    humidity_max = excluded.humidity_max,
				    pressure_min = excluded.pressure_min,
				    pressure_max = excluded.pressure_max
				""";

		jdbcTemplate.update(sql, deviceId, request.getTemperatureMin(), request.getTemperatureMax(),
				request.getHumidityMin(), request.getHumidityMax(), request.getPressureMin(), request.getPressureMax());
	}

	/**
	 * Restituisce le soglie configurate per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return la configurazione delle soglie oppure null se non presente
	 */
	@Override
	public Map<String, Object> findThresholdsByDeviceId(String deviceId) {
		String sql = "SELECT * FROM thresholds WHERE device_id = ?";
		List<Map<String, Object>> result = jdbcTemplate.queryForList(sql, deviceId);
		return result.isEmpty() ? null : result.get(0);
	}
}