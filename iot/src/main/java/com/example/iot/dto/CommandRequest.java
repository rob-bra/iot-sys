package com.example.iot.dto;

/**
 * Data Transfer Object che rappresenta la richiesta di creazione di un comando
 * remoto destinato a un dispositivo.
 * <p>
 * Contiene il tipo di comando e l'eventuale payload associato.
 */
public class CommandRequest {

	private String type;
	private String payload;

	/**
	 * Costruttore di default richiesto dal framework per la deserializzazione JSON.
	 */
	public CommandRequest() {
	}

	/**
	 * Restituisce il tipo di comando remoto da eseguire.
	 *
	 * @return il tipo di comando
	 */
	public String getType() {
		return type;
	}

	/**
	 * Imposta il tipo di comando remoto da eseguire.
	 *
	 * @param type il tipo di comando
	 */
	public void setType(String type) {
		this.type = type;
	}

	/**
	 * Restituisce il payload associato al comando remoto.
	 *
	 * @return il payload del comando
	 */
	public String getPayload() {
		return payload;
	}

	/**
	 * Imposta il payload associato al comando remoto.
	 *
	 * @param payload il payload del comando
	 */
	public void setPayload(String payload) {
		this.payload = payload;
	}
}
