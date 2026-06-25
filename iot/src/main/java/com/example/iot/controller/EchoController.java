package com.example.iot.controller;

import com.example.iot.dto.EchoRequest;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
public class EchoController {

    @PostMapping("/echo")													// creazione endpoint POST /echo
    public Map<String, String> echo(@RequestBody EchoRequest request) {		// @RequestBody dice a Spring: prendi il JSON del body HTTP e trasformalo in oggetto Java EchoRequest
        return Map.of(
                "receivedMessage", request.getMessage(),
                "receivedDeviceId", request.getDeviceId()
        );
    }
}

