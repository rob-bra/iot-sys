package com.example.iot.controller;

import java.util.Map;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController					// dice che la classe gestisce risposte web/REST
public class HelloController {

    @GetMapping("/hello")		// creazione endpoint GET /hello
    public Map<String, String> hello() {
        return Map.of("message", "Hello from Spring Boot");		// Restituisce una mappa Java che Spring converte automaticamente in JSON
        
        // Testo su browser: http://localhost:8080/hello
    }	
}
