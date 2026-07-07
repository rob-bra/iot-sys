package com.example.iot.dto;

/**
 * Data Transfer Object che rappresenta l'acknowledgement inviato dal
 * dispositivo al backend dopo l'esecuzione di un comando remoto.
 * <p>
 * Contiene lo stato finale del comando, il timestamp dell'ack e un eventuale
 * messaggio di risultato.
 */
public class CommandAckRequest {

	private String status;
	private String ackAt;
	private String resultMessage;

	/**
	 * Costruttore di default richiesto dal framework per la deserializzazione JSON.
	 */
	public CommandAckRequest() {
	}

	/**
	 * Restituisce lo stato finale del comando remoto.
	 *
	 * @return lo stato del comando
	 */
	public String getStatus() {
		return status;
	}

	/**
	 * Imposta lo stato finale del comando remoto.
	 *
	 * @param status lo stato del comando
	 */
	public void setStatus(String status) {
		this.status = status;
	}

	/**
	 * Restituisce il timestamp dell'acknowledgement del comando.
	 *
	 * @return il timestamp di conferma
	 */
	public String getAckAt() {
		return ackAt;
	}

	/**
	 * Imposta il timestamp dell'acknowledgement del comando.
	 *
	 * @param ackAt il timestamp di conferma
	 */
	public void setAckAt(String ackAt) {
		this.ackAt = ackAt;
	}

	/**
	 * Restituisce il messaggio descrittivo del risultato del comando.
	 *
	 * @return il messaggio di risultato
	 */
	public String getResultMessage() {
		return resultMessage;
	}

	/**
	 * Imposta il messaggio descrittivo del risultato del comando.
	 *
	 * @param resultMessage il messaggio di risultato
	 */
	public void setResultMessage(String resultMessage) {
		this.resultMessage = resultMessage;
	}
}