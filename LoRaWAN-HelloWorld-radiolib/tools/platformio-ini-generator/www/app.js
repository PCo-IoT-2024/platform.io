const $ = (id) => document.getElementById(id);

const fields = {
  environmentName: $("environmentName"),
  radioModule: $("radioModule"),
  lorawanVersion: $("lorawanVersion"),
  region: $("region"),
  devEui: $("devEui"),
  devEuiFormat: $("devEuiFormat"),
  appKey: $("appKey"),
  appKeyFormat: $("appKeyFormat"),
  nwkKey: $("nwkKey"),
  nwkKeyFormat: $("nwkKeyFormat"),
  nwkKeyLabel: $("nwkKeyLabel"),
  hasGps: $("hasGps"),
  hasTemperature: $("hasTemperature"),
  hasPh: $("hasPh"),
  hasTds: $("hasTds"),
  hasTurbidity: $("hasTurbidity"),
  gpsRx: $("gpsRx"),
  gpsTx: $("gpsTx"),
  temperaturePin: $("temperaturePin"),
  phPin: $("phPin"),
  phTemperaturePin: $("phTemperaturePin"),
  tdsPin: $("tdsPin"),
  turbidityPin: $("turbidityPin"),
  loraBitmap: $("loraBitmap"),
  uplinkInterval: $("uplinkInterval"),
  factoryResetPin: $("factoryResetPin"),
  dangerousNonceResetPin: $("dangerousNonceResetPin"),
  phCalibrationPin: $("phCalibrationPin"),
  tdsCalibrationPin: $("tdsCalibrationPin"),
  turbidityCalibrationPin: $("turbidityCalibrationPin"),
  ph4: $("ph4"),
  ph7: $("ph7"),
  ph10: $("ph10"),
  tdsLowAdc: $("tdsLowAdc"),
  tdsLowPpm: $("tdsLowPpm"),
  tdsHighAdc: $("tdsHighAdc"),
  tdsHighPpm: $("tdsHighPpm"),
  tdsDefaultTemperature: $("tdsDefaultTemperature"),
  turbidityClearAdc: $("turbidityClearAdc"),
  turbidityClearNtu: $("turbidityClearNtu"),
  turbidityTurbidAdc: $("turbidityTurbidAdc"),
  turbidityTurbidNtu: $("turbidityTurbidNtu"),
  output: $("output"),
  generateButton: $("generateButton"),
  downloadButton: $("downloadButton")
};

let generatedIni = "";

function boolFlag(input) {
  return input?.checked ? "1" : "0";
}

function valueOrFallback(element, fallback) {
  const value = element?.value?.trim() ?? "";
  return value === "" ? fallback : value;
}

function normalizeEnvName(value) {
  return value.trim().replace(/[^A-Za-z0-9_\-]/g, "_") || "water_buoy";
}

function onlyHex(value) {
  return value.replace(/[^0-9a-fA-F]/g, "").toUpperCase();
}

function bytesFromContinuousHex(value, byteCount) {
  const padded = onlyHex(value).padEnd(byteCount * 2, "0").slice(0, byteCount * 2);
  const bytes = [];
  for (let i = 0; i < byteCount * 2; i += 2) {
    bytes.push(padded.slice(i, i + 2));
  }
  return bytes;
}

function bytesFromCommaBytes(value, byteCount) {
  const matches = value.match(/(?:0x)?[0-9a-fA-F]{1,2}/g) || [];
  const bytes = matches.slice(0, byteCount).map((part) => onlyHex(part).padStart(2, "0").slice(-2));
  while (bytes.length < byteCount) {
    bytes.push("00");
  }
  return bytes;
}

function normalizeBytes(value, format, byteCount) {
  if (format === "msb-bytes") {
    return bytesFromCommaBytes(value, byteCount);
  }
  if (format === "lsb-bytes") {
    return bytesFromCommaBytes(value, byteCount).reverse();
  }
  return bytesFromContinuousHex(value, byteCount);
}

function normalizeDevEui(value, format) {
  return "0x" + normalizeBytes(value, format, 8).join("");
}

function normalizeKey(value, format) {
  return normalizeBytes(value, format, 16).map((byte) => "0x" + byte).join(", ");
}

function hasEnoughBytes(value, format, byteCount) {
  if (format === "direct") {
    return onlyHex(value).length >= byteCount * 2;
  }
  return (value.match(/(?:0x)?[0-9a-fA-F]{1,2}/g) || []).length >= byteCount;
}

function credentialsReady() {
  const devEuiReady = hasEnoughBytes(fields.devEui.value, fields.devEuiFormat.value, 8);
  const appKeyReady = hasEnoughBytes(fields.appKey.value, fields.appKeyFormat.value, 16);
  const nwkKeyReady = fields.lorawanVersion.value !== "1.1.0" || hasEnoughBytes(fields.nwkKey.value, fields.nwkKeyFormat.value, 16);
  return devEuiReady && appKeyReady && nwkKeyReady;
}

function updateIniActionState() {
  const ready = credentialsReady();
  fields.generateButton.disabled = !ready;
  if (!ready) {
    generatedIni = "";
    fields.downloadButton.disabled = true;
  }
}

function positiveInteger(value, fallback) {
  const parsed = Number.parseInt(value, 10);
  return Number.isFinite(parsed) && parsed > 0 ? parsed : fallback;
}

function pinValue(id, fallback) {
  const parsed = Number.parseInt($(id)?.value ?? "", 10);
  return Number.isFinite(parsed) ? parsed : fallback;
}

function currentLoRaBitmap() {
  return [pinValue("loraPin0", 5), pinValue("loraPin1", 2), pinValue("loraPin2", 14), pinValue("loraPin3", 4)].join(", ");
}

function radioFlags(moduleName, bitmap) {
  const common = [
    `    -D RADIOLIB_LORA_MODULE=${moduleName}`,
    `    -D RADIOLIB_LORA_MODULE_BITMAP=\"${bitmap}\"`,
    "    -D RADIOLIB_EXCLUDE_CC1101",
    "    -D RADIOLIB_EXCLUDE_LR11X0",
    "    -D RADIOLIB_EXCLUDE_RF69",
    "    -D RADIOLIB_EXCLUDE_RFM2X",
    "    -D RADIOLIB_EXCLUDE_SX1231",
    "    -D RADIOLIB_EXCLUDE_SX128X",
    "    -D RADIOLIB_EXCLUDE_SI443X",
    "    -D RADIOLIB_EXCLUDE_NRF24"
  ];

  if (moduleName === "SX1262") {
    return [...common, "    -D RADIOLIB_EXCLUDE_SX127X"];
  }

  return [...common, "    -D RADIOLIB_EXCLUDE_SX126X"];
}

function installLoRaPinFields() {
  const legacyInput = fields.loraBitmap;
  const legacyLabel = legacyInput?.closest("label");
  if (!legacyLabel || $("loraPin0")) {
    return;
  }

  const wrapper = document.createElement("div");
  wrapper.className = "lora-pin-map";
  wrapper.innerHTML = `
    <div class="lora-pin-map-header"><strong>LoRa module pins</strong></div>
    <div class="grid four compact-grid lora-pin-grid">
      <label><span class="lora-pin-label" data-pin-label="0">NSS / CS</span><input id="loraPin0" type="number" value="5"><span class="field-help" data-pin-help="0">SPI select.</span></label>
      <label><span class="lora-pin-label" data-pin-label="1">DIO1 / IRQ</span><input id="loraPin1" type="number" value="2"><span class="field-help" data-pin-help="1">Interrupt.</span></label>
      <label><span class="lora-pin-label" data-pin-label="2">RESET</span><input id="loraPin2" type="number" value="14"><span class="field-help" data-pin-help="2">Radio reset.</span></label>
      <label><span class="lora-pin-label" data-pin-label="3">BUSY</span><input id="loraPin3" type="number" value="4"><span class="field-help" data-pin-help="3">Ready/busy.</span></label>
    </div>
    <input id="loraBitmap" type="hidden" value="5, 2, 14, 4">
  `;

  legacyLabel.replaceWith(wrapper);
  fields.loraBitmap = $("loraBitmap");
  updateLoRaPinLabels();
}

function updateLoRaPinLabels() {
  const labels = fields.radioModule.value === "SX1276"
    ? [["NSS / CS", "SPI select."], ["DIO0 / IRQ", "Interrupt."], ["RESET", "Radio reset."], ["DIO1", "Second IRQ."]]
    : [["NSS / CS", "SPI select."], ["DIO1 / IRQ", "Interrupt."], ["RESET", "Radio reset."], ["BUSY", "Ready/busy."]];

  labels.forEach(([label, help], index) => {
    const labelElement = document.querySelector(`[data-pin-label="${index}"]`);
    const helpElement = document.querySelector(`[data-pin-help="${index}"]`);
    if (labelElement) {
      labelElement.textContent = label;
    }
    if (helpElement) {
      helpElement.textContent = help;
    }
  });
}

function generateIni() {
  if (!credentialsReady()) {
    updateIniActionState();
    return;
  }

  const envName = normalizeEnvName(fields.environmentName.value);
  const moduleName = fields.radioModule.value;
  const keyModel = fields.lorawanVersion.value;
  const devEui = normalizeDevEui(fields.devEui.value, fields.devEuiFormat.value);
  const appKey = normalizeKey(fields.appKey.value, fields.appKeyFormat.value);
  const nwkKey = normalizeKey(fields.nwkKey.value, fields.nwkKeyFormat.value);
  const uplinkIntervalSeconds = positiveInteger(fields.uplinkInterval.value, 60);
  const loraBitmap = currentLoRaBitmap();

  const lines = [
    "; Generated PlatformIO configuration for the LoRaWAN water buoy lego firmware",
    "; Generated by tools/platformio-ini-generator",
    ";",
    `; Radio module: ${moduleName}`,
    `; LoRaWAN key model: ${keyModel}`,
    ";",
    "; The environment below is listed in [platformio] default_envs.",
    "; Therefore -e is not needed for the usual commands.",
    ";",
    "; Build:",
    ";   pio run",
    "; Upload:",
    ";   pio run -t upload",
    "; Monitor:",
    ";   pio device monitor",
    "",
    "[platformio]",
    `default_envs = ${envName}`,
    "",
    `[env:${envName}]`,
    "platform = espressif32",
    "board = esp32dev",
    "framework = arduino",
    "build_unflags =",
    "    -std=gnu++11",
    "    -std=c++11",
    "    -std=gnu++14",
    "    -std=c++14",
    "lib_deps =",
    "    https://github.com/jgromes/RadioLib.git",
    "    mikalhart/TinyGPSPlus",
    "    PaulStoffregen/OneWire",
    "    milesburton/DallasTemperature",
    "    eeprom",
    "build_flags =",
    "    -std=gnu++17",
    "    -D RADIOLIB_LORAWAN_JOIN_EUI=\"0x0000000000000000\"",
    `    -D RADIOLIB_LORAWAN_DEV_EUI=\"${devEui}\"`,
    `    -D RADIOLIB_LORAWAN_APP_KEY=\"${appKey}\"`
  ];

  if (keyModel === "1.1.0") {
    lines.push(`    -D RADIOLIB_LORAWAN_NWK_KEY=\"${nwkKey}\"`);
  }

  lines.push(
    "    -D RADIOLIB_DEBUG_LOG=1",
    "    -D APP_DEBUG_SERIAL=1",
    `    -D APP_FACTORY_RESET_PIN=${valueOrFallback(fields.factoryResetPin, "0")}`,
    `    -D APP_DANGEROUS_NONCE_RESET_PIN=${valueOrFallback(fields.dangerousNonceResetPin, "15")}`,
    `    -D APP_PH_CALIBRATION_PIN=${valueOrFallback(fields.phCalibrationPin, "25")}`,
    `    -D APP_TDS_CALIBRATION_PIN=${valueOrFallback(fields.tdsCalibrationPin, "26")}`,
    `    -D APP_TURBIDITY_CALIBRATION_PIN=${valueOrFallback(fields.turbidityCalibrationPin, "13")}`,
    "    -D RADIOLIB_JOIN_RETRY_MIN_SECONDS=60UL",
    "    -D RADIOLIB_JOIN_RETRY_MAX_SECONDS=3600UL",
    `    -D RADIOLIB_LORA_UPLINK_INTERVAL_SECONDS=${uplinkIntervalSeconds}UL`,
    `    -D RADIOLIB_LORA_REGION=${fields.region.value}`,
    "    -D RADIOLIB_LORA_SUBBANDS=0",
    "    -D RADIOLIB_LORAWAN_PAYLOAD_SIZE=115",
    "    -D RADIOLIB_EXCLUDE_AFSK",
    "    -D RADIOLIB_EXCLUDE_APRS",
    "    -D RADIOLIB_EXCLUDE_AX25",
    "    -D RADIOLIB_EXCLUDE_BELL",
    "    -D RADIOLIB_EXCLUDE_FSK4",
    "    -D RADIOLIB_EXCLUDE_HELLSCHREIBER",
    "    -D RADIOLIB_EXCLUDE_MORSE",
    "    -D RADIOLIB_EXCLUDE_PAGER",
    "    -D RADIOLIB_EXCLUDE_RTTY",
    "    -D RADIOLIB_EXCLUDE_SSTV",
    ...radioFlags(moduleName, loraBitmap),
    "    -D GPS_SERIAL_PORT=2",
    "    -D GPS_SERIAL_BAUD_RATE=9600",
    "    -D GPS_SERIAL_CONFIG=SERIAL_8N1",
    `    -D GPS_SERIAL_RX_PIN=${valueOrFallback(fields.gpsRx, "16")}`,
    `    -D GPS_SERIAL_TX_PIN=${valueOrFallback(fields.gpsTx, "17")}`,
    `    -D DALLAS_TEMPERATURE_PIN=${valueOrFallback(fields.temperaturePin, "27")}`,
    `    -D PH4502C_PH_PIN=${valueOrFallback(fields.phPin, "34")}`,
    `    -D PH4502C_TEMPERATURE_PIN=${valueOrFallback(fields.phTemperaturePin, "35")}`,
    `    -D PH4_ADC_VALUE=${valueOrFallback(fields.ph4, "2503")}`,
    `    -D PH7_ADC_VALUE=${valueOrFallback(fields.ph7, "2080")}`,
    `    -D PH10_ADC_VALUE=${valueOrFallback(fields.ph10, "1615")}`,
    `    -D TDS_SENSOR_PIN=${valueOrFallback(fields.tdsPin, "32")}`,
    `    -D TDS_LOW_ADC_VALUE=${valueOrFallback(fields.tdsLowAdc, "0")}`,
    `    -D TDS_LOW_PPM_VALUE=${valueOrFallback(fields.tdsLowPpm, "0.0")}`,
    `    -D TDS_HIGH_ADC_VALUE=${valueOrFallback(fields.tdsHighAdc, "1800")}`,
    `    -D TDS_HIGH_PPM_VALUE=${valueOrFallback(fields.tdsHighPpm, "1000.0")}`,
    `    -D TDS_DEFAULT_TEMPERATURE_C=${valueOrFallback(fields.tdsDefaultTemperature, "22.0f")}`,
    "    -D A1=TDS_SENSOR_PIN",
    `    -D TURBIDITY_PIN=${valueOrFallback(fields.turbidityPin, "33")}`,
    `    -D TURBIDITY_CLEAR_WATER_ADC_VALUE=${valueOrFallback(fields.turbidityClearAdc, "2500")}`,
    `    -D TURBIDITY_CLEAR_WATER_NTU=${valueOrFallback(fields.turbidityClearNtu, "0.0")}`,
    `    -D TURBIDITY_TURBID_WATER_ADC_VALUE=${valueOrFallback(fields.turbidityTurbidAdc, "1200")}`,
    `    -D TURBIDITY_TURBID_WATER_NTU=${valueOrFallback(fields.turbidityTurbidNtu, "600.0")}`,
    `    -D APP_HAS_GPS=${boolFlag(fields.hasGps)}`,
    `    -D APP_HAS_TEMPERATURE=${boolFlag(fields.hasTemperature)}`,
    `    -D APP_HAS_PH=${boolFlag(fields.hasPh)}`,
    `    -D APP_HAS_TDS=${boolFlag(fields.hasTds)}`,
    `    -D APP_HAS_TURBIDITY=${boolFlag(fields.hasTurbidity)}`,
    "    -Wno-cpp"
  );

  generatedIni = lines.join("\n") + "\n";
  fields.output.textContent = generatedIni;
  fields.downloadButton.disabled = false;
}

function downloadText(filename, content) {
  const blob = new Blob([content], { type: "text/plain;charset=utf-8" });
  const link = document.createElement("a");
  link.href = URL.createObjectURL(blob);
  link.download = filename;
  document.body.appendChild(link);
  link.click();
  link.remove();
  URL.revokeObjectURL(link.href);
}

function downloadIni() {
  if (generatedIni) {
    downloadText("platformio.ini", generatedIni);
  }
}

function updateVersionUi() {
  const isV110 = fields.lorawanVersion.value === "1.1.0";
  fields.nwkKey.disabled = !isV110;
  fields.nwkKeyFormat.disabled = !isV110;
  fields.nwkKeyLabel.classList.toggle("disabled", !isV110);
  updateIniActionState();
}

function installCredentialValidation() {
  [fields.devEui, fields.devEuiFormat, fields.appKey, fields.appKeyFormat, fields.nwkKey, fields.nwkKeyFormat, fields.lorawanVersion]
    .forEach((element) => {
      element?.addEventListener("input", updateIniActionState);
      element?.addEventListener("change", updateIniActionState);
    });
}

installLoRaPinFields();
installCredentialValidation();
fields.generateButton.addEventListener("click", generateIni);
fields.downloadButton.addEventListener("click", downloadIni);
fields.lorawanVersion.addEventListener("change", updateVersionUi);
fields.radioModule.addEventListener("change", updateLoRaPinLabels);
updateVersionUi();
updateLoRaPinLabels();
updateIniActionState();
