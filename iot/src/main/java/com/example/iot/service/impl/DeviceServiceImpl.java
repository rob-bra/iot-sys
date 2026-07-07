package com.example.iot.service.impl;

import com.example.iot.repository.DeviceRepository;
import com.example.iot.service.DeviceService;

import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Map;

/**
 * Implementazione concreta del service dedicato ai dispositivi.
 * <p>
 * Si occupa di fornire al livello superiore le informazioni di stato dei
 * dispositivi registrati nel sistema.
 */
@Service
public class DeviceServiceImpl implements DeviceService {

	private final DeviceRepository deviceRepository;

	/**
	 * Costruisce l'implementazione del servizio dedicato ai dispositivi.
	 *
	 * @param deviceRepository il repository dei dispositivi
	 */
	public DeviceServiceImpl(DeviceRepository deviceRepository) {
		this.deviceRepository = deviceRepository;
	}

	/**
	 * Restituisce l'elenco completo dei dispositivi registrati.
	 *
	 * @return la lista dei dispositivi presenti nel sistema
	 */
	@Override
	public List<Map<String, Object>> getAllDevices() {
		return deviceRepository.findAllDevices();
	}
}