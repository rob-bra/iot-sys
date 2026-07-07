package com.example.iot.dto;

// DTO = Data Transfer Object
/* Oggetto Java che rappresenta il JSON in ingresso.
 * 
 * esempio: dal client mando un file JSON:
 * {
 * 	 "message" : "ciao",
 *   "deviceId": "STEVAL V1"
 * }
 *
 * spring lo andrà a mappare in un oggetto EchoRequest.
 */

/**
 * Data Transfer Object utilizzato per le richieste di test verso l'endpoint di
 * echo.
 * <p>
 * Contiene un messaggio e l'identificativo del dispositivo associato alla
 * richiesta.
 */
public class EchoRequest {
	private String message;
	private String deviceId;

	/**
	 * Costruttore di default richiesto dal framework per la deserializzazione JSON.
	 */
	public EchoRequest() {
	}

	/**
	 * Restituisce il messaggio contenuto nella richiesta.
	 *
	 * @return il messaggio inviato dal client
	 */
	public String getMessage() {
		return message;
	}

	/**
	 * Imposta il messaggio contenuto nella richiesta.
	 *
	 * @param message il messaggio inviato dal client
	 */
	public void setMessage(String message) {
		this.message = message;
	}

	/**
	 * Restituisce l'identificativo del dispositivo associato alla richiesta.
	 *
	 * @return l'identificativo del dispositivo
	 */
	public String getDeviceId() {
		return deviceId;
	}

	/**
	 * Imposta l'identificativo del dispositivo associato alla richiesta.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 */
	public void setDeviceId(String deviceId) {
		this.deviceId = deviceId;
	}
}
