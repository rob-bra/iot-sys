package com.example.iot.service.impl;

import com.example.iot.dto.ThresholdRequest;
import com.example.iot.repository.ThresholdRepository;
import com.example.iot.service.ThresholdService;

import org.springframework.stereotype.Service;

import java.util.Map;

/**
 * Implementazione concreta del service dedicato alle soglie di allarme.
 * <p>
 * Gestisce il salvataggio e il recupero della configurazione delle soglie
 * associate ai dispositivi monitorati.
 */
@Service
public class ThresholdServiceImpl implements ThresholdService {

	private final ThresholdRepository thresholdRepository;

	/**
	 * Costruisce l'implementazione del servizio dedicato alle soglie di allarme.
	 *
	 * @param thresholdRepository il repository delle soglie
	 */
	public ThresholdServiceImpl(ThresholdRepository thresholdRepository) {
		this.thresholdRepository = thresholdRepository;
	}

	/**
	 * Salva o aggiorna le soglie del dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @param request  il payload contenente le soglie da memorizzare
	 */
	@Override
	public void saveThresholds(String deviceId, ThresholdRequest request) {
		thresholdRepository.upsertThresholds(deviceId, request);
	}

	/**
	 * Restituisce le soglie configurate per il dispositivo specificato.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 * @return le soglie del dispositivo oppure null se assenti
	 */
	@Override
	public Map<String, Object> getThresholds(String deviceId) {
		return thresholdRepository.findThresholdsByDeviceId(deviceId);
	}
}
