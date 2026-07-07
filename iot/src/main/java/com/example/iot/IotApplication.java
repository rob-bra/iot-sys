package com.example.iot;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

/**
 * Classe principale di avvio dell'applicazione Spring Boot.
 * <p>
 * Inizializza il contesto applicativo, i componenti del framework e i moduli
 * del backend del sistema IoT di monitoraggio e controllo remoto.
 */
@SpringBootApplication
public class IotApplication {

	public static void main(String[] args) {
		SpringApplication.run(IotApplication.class, args);
	}

}
