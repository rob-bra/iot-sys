package com.example.iot.controller;

import java.util.Map;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

/**
 * Controller REST di supporto utilizzato per verificare che l'applicazione
 * Spring Boot sia correttamente avviata e raggiungibile.
 * <p>
 * Espone un endpoint di test che restituisce un semplice messaggio JSON.
 */
@RestController // dice che la classe gestisce risposte web/REST
public class HelloController {

	/**
	 * Restituisce un semplice messaggio di test per verificare che l'applicazione
	 * Spring Boot sia correttamente avviata e raggiungibile.
	 *
	 * @return una mappa contenente il messaggio di test
	 */
	@GetMapping("/hello") // creazione endpoint GET /hello
	public Map<String, String> hello() {
		return Map.of("message", "Hello from Spring Boot"); // Restituisce una mappa Java che Spring converte
															// automaticamente in JSON

		// Testo su browser: http://localhost:8080/hello
	}
}
