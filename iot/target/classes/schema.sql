CREATE TABLE IF NOT EXISTS devices (
    device_id TEXT PRIMARY KEY,
    status TEXT NOT NULL,
    battery INTEGER,
    last_seen TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS telemetry (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id TEXT NOT NULL,
    timestamp TEXT NOT NULL,
    temperature REAL NOT NULL,
    humidity REAL NOT NULL,
    pressure REAL NOT NULL,
	battery INTEGER,
    orientation TEXT,
    FOREIGN KEY (device_id) REFERENCES devices(device_id)
);

CREATE TABLE IF NOT EXISTS thresholds (
    device_id TEXT PRIMARY KEY,
    temperature_min REAL,
    temperature_max REAL,
    humidity_min REAL,
    humidity_max REAL,
    pressure_min REAL,
    pressure_max REAL,
    FOREIGN KEY (device_id) REFERENCES devices(device_id)
);

CREATE TABLE IF NOT EXISTS alerts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id TEXT NOT NULL,
    timestamp TEXT NOT NULL,
    type TEXT NOT NULL,
    measured_value REAL,
    threshold_value REAL,
    message TEXT NOT NULL,
    FOREIGN KEY (device_id) REFERENCES devices(device_id)
);

CREATE TABLE IF NOT EXISTS commands (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id TEXT NOT NULL,
    type TEXT NOT NULL,
    payload TEXT,
    status TEXT NOT NULL,
    created_at TEXT NOT NULL,
    ack_at TEXT,
    result_message TEXT,
    FOREIGN KEY (device_id) REFERENCES devices(device_id)
);