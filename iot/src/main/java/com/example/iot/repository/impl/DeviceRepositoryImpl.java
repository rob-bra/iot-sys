package com.example.iot.repository.impl;

import com.example.iot.repository.DeviceRepository;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

/*
 * REPOSITORY per la tabella devices
 */

/**
 * Implementazione concreta del repository dedicato alla gestione persistente
 * dei dispositivi.
 * <p>
 * Utilizza JDBC e {@code JdbcTemplate} per memorizzare e aggiornare lo stato
 * dei device nel database SQLite.
 */
@Repository
public class DeviceRepositoryImpl implements DeviceRepository {

	private final JdbcTemplate jdbcTemplate;

	/**
	 * Costruisce l'implementazione del repository dei dispositivi inizializzando il
	 * supporto JDBC per l'accesso al database.
	 *
	 * @param jdbcTemplate il componente Spring utilizzato per eseguire query SQL
	 */
	public DeviceRepositoryImpl(JdbcTemplate jdbcTemplate) {
		this.jdbcTemplate = jdbcTemplate;
	}

	/**
	 * Restituisce tutti i dispositivi presenti nel database. Tabella in output.
	 *
	 * @return la lista dei dispositivi registrati
	 */
	@Override
	public List<Map<String, Object>> findAllDevices() {
		String sql = "SELECT * FROM devices ORDER BY device_id";
		return jdbcTemplate.queryForList(sql);
	}

	/**
	 * Inserisce o aggiorna il record del dispositivo nel database.
	 * 
	 * Se il device NON esiste, viene aggiunto. Se il device esiste, vengono
	 * aggiornati i parametri di stato/batteria/last_seen
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param battery  il livello di batteria corrente
	 * @param lastSeen il timestamp dell'ultimo contatto
	 */
	@Override
	public void upsertDevice(String deviceId, Integer battery, String lastSeen) {
		String sql = """
				INSERT INTO devices (device_id, status, battery, last_seen)
				VALUES (?, 'ONLINE', ?, ?)
				ON CONFLICT(device_id) DO UPDATE SET
				    status = 'ONLINE',
				    battery = excluded.battery,
				    last_seen = excluded.last_seen
				""";

		jdbcTemplate.update(sql, deviceId, battery, lastSeen);
	}
}
