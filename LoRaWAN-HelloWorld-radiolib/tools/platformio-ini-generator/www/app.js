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
  ph4: $("ph4"),
  ph7: $("ph7"),
  ph10: $("ph10"),
  tdsVcc: $("tdsVcc"),
  tdsAdc: $("tdsAdc"),
  tdsTemp: $("tdsTemp"),
  turbidityVcc: $("turbidityVcc"),
  turbidityAdc: $("turbidityAdc"),
  clearVoltage: $("clearVoltage"),
  clearNtu: $("clearNtu"),
  turbidVoltage: $("turbidVoltage"),
  turbidNtu: $("turbidNtu"),
  output: $("output"),
  generateButton: $("generateButton"),
  downloadButton: $("downloadButton")
};

let generatedIni = "";

function boolFlag(input) {
  return input.checked ? "1" : "0";
}

function normalizeEnvName(value) {
  return value.trim().replace(/[^A-Za-z0-9_\-]/g, "_") || "water_buoy";
}

function onlyHex(value) {
  return value.replace(/[^0-9a-fA-F]/g, "").toUpperCase();
}

function positiveInteger(value, fallback) {
  const parsed = Number.parseInt(value, 10);
  return Number.isFinite(parsed) && parsed > 0 ? parsed : fallback;
}

function pinValue(id, fallback) {
  const element = $(id);
  const parsed = Number.parseInt(element?.value ?? "", 10);
  return Number.isFinite(parsed) ? parsed : fallback;
}

function currentLoRaBitmap() {
  return [
    pinValue("loraPin0", 5),
    pinValue("loraPin1", 2),
    pinValue("loraPin2", 14),
    pinValue("loraPin3", 4)
  ].join(", ");
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
  const bytes = [];
  const matches = value.match(/(?:0x)?[0-9a-fA-F]{1,2}/g) || [];

  for (const match of matches.slice(0, byteCount)) {
    bytes.push(onlyHex(match).padStart(2, "0").slice(-2));
  }

  while (bytes.length < byteCount) {
    bytes.push("00");
  }

  return bytes;
}

function normalizeBytes(value, format, byteCount) {
  switch (format) {
    case "msb-bytes":
      return bytesFromCommaBytes(value, byteCount);
    case "lsb-bytes":
      return bytesFromCommaBytes(value, byteCount).reverse();
    case "direct":
    default:
      return bytesFromContinuousHex(value, byteCount);
  }
}

function normalizeDevEui(value, format) {
  return "0x" + normalizeBytes(value, format, 8).join("");
}

function normalizeKey(value, format) {
  return normalizeBytes(value, format, 16).map((byte) => "0x" + byte).join(", ");
}

function radioFlags(moduleName, bitmap) {
  if (moduleName === "SX1262") {
    return [
      "    -D RADIOLIB_LORA_MODULE=SX1262",
      `    -D RADIOLIB_LORA_MODULE_BITMAP=\"${bitmap}\"`,
      "    -D RADIOLIB_EXCLUDE_CC1101",
      "    -D RADIOLIB_EXCLUDE_LR11X0",
      "    -D RADIOLIB_EXCLUDE_RF69",
      "    -D RADIOLIB_EXCLUDE_RFM2X",
      "    -D RADIOLIB_EXCLUDE_SX1231",
      "    -D RADIOLIB_EXCLUDE_SX127X",
      "    -D RADIOLIB_EXCLUDE_SX128X",
      "    -D RADIOLIB_EXCLUDE_SI443X",
      "    -D RADIOLIB_EXCLUDE_NRF24"
    ];
  }

  return [
    "    -D RADIOLIB_LORA_MODULE=SX1276",
    `    -D RADIOLIB_LORA_MODULE_BITMAP=\"${bitmap}\"`,
    "    -D RADIOLIB_EXCLUDE_CC1101",
    "    -D RADIOLIB_EXCLUDE_SX126X",
    "    -D RADIOLIB_EXCLUDE_LR11X0",
    "    -D RADIOLIB_EXCLUDE_RF69",
    "    -D RADIOLIB_EXCLUDE_RFM2X",
    "    -D RADIOLIB_EXCLUDE_SX1231",
    "    -D RADIOLIB_EXCLUDE_SX128X",
    "    -D RADIOLIB_EXCLUDE_SI443X",
    "    -D RADIOLIB_EXCLUDE_NRF24"
  ];
}

function installLoRaPinFields() {
  const legacyInput = fields.loraBitmap;
  if (!legacyInput || $("loraPin0")) {
    return;
  }

  const legacyLabel = legacyInput.closest("label");
  if (!legacyLabel) {
    return;
  }

  const wrapper = document.createElement("div");
  wrapper.className = "lora-pin-map";
  wrapper.innerHTML = `
    <div class="lora-pin-map-header">
      <strong>LoRa module pins</strong>
      <span class="field-help">Order used for <code>RADIOLIB_LORA_MODULE_BITMAP</code>.</span>
    </div>
    <div class="grid four compact-grid lora-pin-grid">
      <label><span class="lora-pin-label" data-pin-label="0">NSS / CS</span>
        <input id="loraPin0" type="number" value="5">
        <span class="field-help" data-pin-help="0">SPI select.</span>
      </label>
      <label><span class="lora-pin-label" data-pin-label="1">DIO1 / IRQ</span>
        <input id="loraPin1" type="number" value="2">
        <span class="field-help" data-pin-help="1">Interrupt.</span>
      </label>
      <label><span class="lora-pin-label" data-pin-label="2">RESET</span>
        <input id="loraPin2" type="number" value="14">
        <span class="field-help" data-pin-help="2">Radio reset.</span>
      </label>
      <label><span class="lora-pin-label" data-pin-label="3">BUSY</span>
        <input id="loraPin3" type="number" value="4">
        <span class="field-help" data-pin-help="3">Ready/busy.</span>
      </label>
    </div>
    <input id="loraBitmap" type="hidden" value="5, 2, 14, 4">
  `;

  legacyLabel.replaceWith(wrapper);
  fields.loraBitmap = $("loraBitmap");
  updateLoRaPinLabels();
}

function updateLoRaPinLabels() {
  const moduleName = fields.radioModule.value;

  const sx1262 = [
    ["NSS / CS", "SPI select."],
    ["DIO1 / IRQ", "Interrupt."],
    ["RESET", "Radio reset."],
    ["BUSY", "Ready/busy."]
  ];

  const sx1276 = [
    ["NSS / CS", "SPI select."],
    ["DIO0 / IRQ", "Interrupt."],
    ["RESET", "Radio reset."],
    ["DIO1", "Second IRQ."]
  ];

  const labels = moduleName === "SX1276" ? sx1276 : sx1262;

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

function syncLoRaBitmapField() {
  const bitmap = currentLoRaBitmap();
  if (fields.loraBitmap) {
    fields.loraBitmap.value = bitmap;
  }
  return bitmap;
}

function generateIni() {
  const envName = normalizeEnvName(fields.environmentName.value);
  const moduleName = fields.radioModule.value;
  const lorawanVersion = fields.lorawanVersion.value;
  const devEui = normalizeDevEui(fields.devEui.value, fields.devEuiFormat.value);
  const appKey = normalizeKey(fields.appKey.value, fields.appKeyFormat.value);
  const nwkKey = normalizeKey(fields.nwkKey.value, fields.nwkKeyFormat.value);
  const uplinkIntervalSeconds = positiveInteger(fields.uplinkInterval.value, 60);
  const loraBitmap = syncLoRaBitmapField();

  const lines = [];
  lines.push("; Generated PlatformIO configuration for the LoRaWAN water buoy lego firmware");
  lines.push("; Generated by tools/platformio-ini-generator");
  lines.push(";");
  lines.push(`; Radio module: ${moduleName}`);
  lines.push(`; LoRaWAN version: ${lorawanVersion}`);
  lines.push(";");
  lines.push("; The environment below is listed in [platformio] default_envs.");
  lines.push("; Therefore -e is not needed for the usual commands.");
  lines.push(";");
  lines.push("; Build:");
  lines.push(";   pio run");
  lines.push("; Upload:");
  lines.push(";   pio run -t upload");
  lines.push("; Monitor:");
  lines.push(";   pio device monitor");
  lines.push("");
  lines.push("[platformio]");
  lines.push(`default_envs = ${envName}`);
  lines.push("");
  lines.push(`[env:${envName}]`);
  lines.push("platform = espressif32");
  lines.push("board = esp32dev");
  lines.push("framework = arduino");
  lines.push("build_unflags =");
  lines.push("    -std=gnu++11");
  lines.push("    -std=c++11");
  lines.push("    -std=gnu++14");
  lines.push("    -std=c++14");
  lines.push("lib_deps =");
  lines.push("    https://github.com/jgromes/RadioLib.git");
  lines.push("    mikalhart/TinyGPSPlus");
  lines.push("    PaulStoffregen/OneWire");
  lines.push("    milesburton/DallasTemperature");
  lines.push("    https://github.com/PCo-IoT-2024/GravityTDS.git");
  lines.push("    eeprom");
  lines.push("build_flags =");
  lines.push("    -std=gnu++17");
  lines.push("    -D RADIOLIB_LORAWAN_JOIN_EUI=\"0x0000000000000000\"");
  lines.push(`    -D RADIOLIB_LORAWAN_DEV_EUI=\"${devEui}\"`);
  lines.push(`    -D RADIOLIB_LORAWAN_APP_KEY=\"${appKey}\"`);
  if (lorawanVersion === "1.1.0") {
    lines.push(`    -D RADIOLIB_LORAWAN_NWK_KEY=\"${nwkKey}\"`);
  }
  lines.push("    -D RADIOLIB_DEBUG_LOG=1");
  lines.push("    -D APP_DEBUG_SERIAL=1");
  lines.push("    -D APP_FACTORY_RESET_PIN=0");
  lines.push("    -D APP_DANGEROUS_NONCE_RESET_PIN=15");
  lines.push("    -D RADIOLIB_JOIN_RETRY_MIN_SECONDS=60UL");
  lines.push("    -D RADIOLIB_JOIN_RETRY_MAX_SECONDS=3600UL");
  lines.push(`    -D RADIOLIB_LORA_UPLINK_INTERVAL_SECONDS=${uplinkIntervalSeconds}UL`);
  lines.push(`    -D RADIOLIB_LORA_REGION=${fields.region.value}`);
  lines.push("    -D RADIOLIB_LORA_SUBBANDS=0");
  lines.push("    -D RADIOLIB_LORAWAN_PAYLOAD_SIZE=115");
  lines.push("    -D RADIOLIB_EXCLUDE_AFSK");
  lines.push("    -D RADIOLIB_EXCLUDE_APRS");
  lines.push("    -D RADIOLIB_EXCLUDE_AX25");
  lines.push("    -D RADIOLIB_EXCLUDE_BELL");
  lines.push("    -D RADIOLIB_EXCLUDE_FSK4");
  lines.push("    -D RADIOLIB_EXCLUDE_HELLSCHREIBER");
  lines.push("    -D RADIOLIB_EXCLUDE_MORSE");
  lines.push("    -D RADIOLIB_EXCLUDE_PAGER");
  lines.push("    -D RADIOLIB_EXCLUDE_RTTY");
  lines.push("    -D RADIOLIB_EXCLUDE_SSTV");
  lines.push(...radioFlags(moduleName, loraBitmap));
  lines.push("    -D GPS_SERIAL_PORT=2");
  lines.push("    -D GPS_SERIAL_BAUD_RATE=9600");
  lines.push("    -D GPS_SERIAL_CONFIG=SERIAL_8N1");
  lines.push(`    -D GPS_SERIAL_RX_PIN=${fields.gpsRx.value}`);
  lines.push(`    -D GPS_SERIAL_TX_PIN=${fields.gpsTx.value}`);
  lines.push(`    -D DALLAS_TEMPERATURE_PIN=${fields.temperaturePin.value}`);
  lines.push(`    -D PH4502C_PH_PIN=${fields.phPin.value}`);
  lines.push(`    -D PH4502C_TEMPERATURE_PIN=${fields.phTemperaturePin.value}`);
  lines.push(`    -D PH4_ADC_VALUE=${fields.ph4.value}`);
  lines.push(`    -D PH7_ADC_VALUE=${fields.ph7.value}`);
  lines.push(`    -D PH10_ADC_VALUE=${fields.ph10.value}`);
  lines.push(`    -D TDS_SENSOR_PIN=${fields.tdsPin.value}`);
  lines.push(`    -D TDS_SENSOR_VCC=${fields.tdsVcc.value}`);
  lines.push(`    -D TDS_SENSOR_ADC_RESOLUTION=${fields.tdsAdc.value}`);
  lines.push(`    -D TDS_DEFAULT_TEMPERATURE_C=${fields.tdsTemp.value}`);
  lines.push("    -D A1=TDS_SENSOR_PIN");
  lines.push(`    -D TURBIDITY_PIN=${fields.turbidityPin.value}`);
  lines.push(`    -D TURBIDITY_VCC=${fields.turbidityVcc.value}`);
  lines.push(`    -D TURBIDITY_ADC_MAX=${fields.turbidityAdc.value}`);
  lines.push(`    -D TURBIDITY_CLEAR_WATER_VOLTAGE=${fields.clearVoltage.value}`);
  lines.push(`    -D TURBIDITY_CLEAR_WATER_NTU=${fields.clearNtu.value}`);
  lines.push(`    -D TURBIDITY_TURBID_WATER_VOLTAGE=${fields.turbidVoltage.value}`);
  lines.push(`    -D TURBIDITY_TURBID_WATER_NTU=${fields.turbidNtu.value}`);
  lines.push(`    -D APP_HAS_GPS=${boolFlag(fields.hasGps)}`);
  lines.push(`    -D APP_HAS_TEMPERATURE=${boolFlag(fields.hasTemperature)}`);
  lines.push(`    -D APP_HAS_PH=${boolFlag(fields.hasPh)}`);
  lines.push(`    -D APP_HAS_TDS=${boolFlag(fields.hasTds)}`);
  lines.push(`    -D APP_HAS_TURBIDITY=${boolFlag(fields.hasTurbidity)}`);
  lines.push("    -Wno-cpp");

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
  if (!generatedIni) {
    return;
  }

  downloadText("platformio.ini", generatedIni);
}

function updateVersionUi() {
  const isV110 = fields.lorawanVersion.value === "1.1.0";
  fields.nwkKey.disabled = !isV110;
  fields.nwkKeyFormat.disabled = !isV110;
  fields.nwkKeyLabel.classList.toggle("disabled", !isV110);
}

installLoRaPinFields();
fields.generateButton.addEventListener("click", generateIni);
fields.downloadButton.addEventListener("click", downloadIni);
fields.lorawanVersion.addEventListener("change", updateVersionUi);
fields.radioModule.addEventListener("change", updateLoRaPinLabels);
updateVersionUi();
updateLoRaPinLabels();
