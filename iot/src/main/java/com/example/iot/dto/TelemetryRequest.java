package com.example.iot.dto;

/**
 * Data Transfer Object che rappresenta una richiesta di telemetria inviata da
 * un dispositivo al backend.
 * <p>
 * Contiene i parametri ambientali rilevati, informazioni di stato del
 * dispositivo e metadati come identificativo del device e timestamp.
 */
public class TelemetryRequest {
	private String deviceId;
	private String timestamp;
	private double temperature;
	private double humidity;
	private double pressure;
	private Integer battery;
	private String orientation;

	/**
	 * Costruttore di default richiesto dal framework per la deserializzazione JSON.
	 */
	public TelemetryRequest() {
	}

	/**
	 * Restituisce l'identificativo del dispositivo che ha generato la telemetria.
	 *
	 * @return l'identificativo del dispositivo
	 */
	public String getDeviceId() {
		return deviceId;
	}

	/**
	 * Imposta l'identificativo del dispositivo che ha generato la telemetria.
	 *
	 * @param deviceId l'identificativo del dispositivo
	 */
	public void setDeviceId(String deviceId) {
		this.deviceId = deviceId;
	}

	/**
	 * Restituisce il timestamp associato alla telemetria.
	 *
	 * @return il timestamp della misura
	 */
	public String getTimestamp() {
		return timestamp;
	}

	/**
	 * Imposta il timestamp associato alla telemetria.
	 *
	 * @param timestamp il timestamp della misura
	 */
	public void setTimestamp(String timestamp) {
		this.timestamp = timestamp;
	}

	/**
	 * Restituisce il valore di temperatura rilevato.
	 *
	 * @return la temperatura misurata
	 */
	public double getTemperature() {
		return temperature;
	}

	/**
	 * Imposta il valore di temperatura rilevato.
	 *
	 * @param temperature la temperatura misurata
	 */
	public void setTemperature(double temperature) {
		this.temperature = temperature;
	}

	/**
	 * Restituisce il valore di umidità rilevato.
	 *
	 * @return l'umidità misurata
	 */
	public double getHumidity() {
		return humidity;
	}

	/**
	 * Imposta il valore di umidità rilevato.
	 *
	 * @param humidity l'umidità misurata
	 */
	public void setHumidity(double humidity) {
		this.humidity = humidity;
	}

	/**
	 * Restituisce il valore di pressione rilevato.
	 *
	 * @return la pressione misurata
	 */
	public double getPressure() {
		return pressure;
	}

	/**
	 * Imposta il valore di pressione rilevato.
	 *
	 * @param pressure la pressione misurata
	 */
	public void setPressure(double pressure) {
		this.pressure = pressure;
	}

	/**
	 * Restituisce il livello di batteria del dispositivo.
	 *
	 * @return la percentuale di batteria disponibile
	 */
	public Integer getBattery() {
		return battery;
	}

	/**
	 * Imposta il livello di batteria del dispositivo.
	 *
	 * @param battery la percentuale di batteria disponibile
	 */
	public void setBattery(Integer battery) {
		this.battery = battery;
	}

	/**
	 * Restituisce l'informazione di orientamento o stato spaziale del dispositivo.
	 *
	 * @return lo stato di orientamento del dispositivo
	 */
	public String getOrientation() {
		return orientation;
	}

	/**
	 * Imposta l'informazione di orientamento o stato spaziale del dispositivo.
	 *
	 * @param orientation lo stato di orientamento del dispositivo
	 */
	public void setOrientation(String orientation) {
		this.orientation = orientation;
	}
}
