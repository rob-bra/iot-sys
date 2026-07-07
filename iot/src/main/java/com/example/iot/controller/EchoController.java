package com.example.iot.controller;

import com.example.iot.dto.EchoRequest;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

/**
 * Controller REST di test utilizzato per verificare il corretto mapping dei
 * payload JSON verso oggetti Java e la serializzazione della risposta dal
 * backend verso il client.
 * <p>
 * Espone un endpoint di echo utile nelle prime fasi di sviluppo.
 */
@RestController
public class EchoController {

	/**
	 * POST method. Riceve un payload JSON di test e restituisce i valori ricevuti.
	 * Verifica del mapping tra richiesta HTTP e DTO.
	 *
	 * @param request il payload contenente il messaggio e l'identificativo del
	 *                device (deviceId)
	 * @return una mappa contenente i dati ricevuti nella richiesta
	 */
	@PostMapping("/echo") // creazione endpoint POST /echo
	public Map<String, String> echo(@RequestBody EchoRequest request) { // @RequestBody dice a Spring: prendi il JSON
																		// del body HTTP e trasformalo in oggetto Java
																		// EchoRequest
		return Map.of("receivedMessage", request.getMessage(), "receivedDeviceId", request.getDeviceId());
	}
}
