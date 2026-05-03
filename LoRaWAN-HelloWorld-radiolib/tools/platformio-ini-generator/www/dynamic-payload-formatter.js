(() => {
  const $ = (id) => document.getElementById(id);
  const prompt = "Press “Generate payload formatter”.";
  let generated = "";
  let hasGenerated = false;

  const out = $("payloadFormatterOutput");
  if (out) {
    out.textContent = prompt;
    new MutationObserver(() => {
      if (!hasGenerated && out.textContent !== prompt) {
        out.textContent = prompt;
      }
    }).observe(out, { childList: true, characterData: true, subtree: true });
  }

  function checked(id) {
    return !!$(id)?.checked;
  }

  function add(lines, enabled, block) {
    if (enabled) {
      lines.push(block.trimEnd(), "");
    }
  }

  function pin(id, fallback) {
    const parsed = Number.parseInt($(id)?.value ?? "", 10);
    return Number.isFinite(parsed) ? parsed : fallback;
  }

  function enhanceHero() {
    const lead = document.querySelector(".lead");
    if (!lead) {
      return;
    }

    lead.innerHTML = `Generate a tailored <code>platformio.ini</code> and matching <code>payload-formatter.js</code> for the selected radio module, LoRaWAN version, <a href="https://www.thethingsindustries.com/docs/concepts/ttn/" target="_blank" rel="noopener noreferrer">The Things Networks Sandbox (TTN)</a> device credentials, sensor set, and pin mapping.`;
  }

  function populateLorawanVersionOptions() {
    const version = $("lorawanVersion");
    if (!version) {
      return;
    }

    const selected = version.value || "1.1.0";
    const versions = ["1.1.0", "1.0.4", "1.0.3", "1.0.2", "1.0.1", "1.0.0"];

    version.replaceChildren();

    for (const value of versions) {
      const option = document.createElement("option");
      option.value = value;
      option.textContent = value;
      version.appendChild(option);
    }

    version.value = versions.includes(selected) ? selected : "1.1.0";
  }

  function populateRegionOptions() {
    const region = $("region");
    if (!region) {
      return;
    }

    const selected = region.value || "EU868";
    const regions = [
      ["EU868", "EU868 — Europe 863–870 MHz"],
      ["US915", "US915 — North America 902–928 MHz"],
      ["EU433", "EU433 — Europe 433 MHz"],
      ["AU915", "AU915 — Australia 915–928 MHz"],
      ["CN470", "CN470 — China 470–510 MHz"],
      ["AS923", "AS923-1 — Asia-Pacific 923 MHz"],
      ["AS923_2", "AS923-2 — Asia-Pacific variant"],
      ["AS923_3", "AS923-3 — Asia-Pacific variant"],
      ["AS923_4", "AS923-4 — Asia-Pacific variant"],
      ["KR920", "KR920 — South Korea 920–923 MHz"],
      ["IN865", "IN865 — India 865–867 MHz"]
    ];

    region.replaceChildren();

    for (const [value, label] of regions) {
      const option = document.createElement("option");
      option.value = value;
      option.textContent = label;
      region.appendChild(option);
    }

    region.value = regions.some(([value]) => value === selected) ? selected : "EU868";
  }

  function appendFieldHelp(controlId, text) {
    const control = $(controlId);
    const label = control?.closest("label");
    if (!control || !label || label.querySelector(".field-help")) {
      return;
    }

    const help = document.createElement("span");
    help.className = "field-help";
    help.textContent = text;
    label.appendChild(help);
  }

  function enhanceLorawanProfileFields() {
    appendFieldHelp("radioModule", "Transceiver chip mounted on the ESP32 radio board.");
    appendFieldHelp("lorawanVersion", "Must match the LoRaWAN MAC version configured in TTN.");
    appendFieldHelp("region", "Frequency plan. Use EU868 for this course in Europe.");
  }

  function moveUplinkIntervalToLorawanProfile() {
    const uplinkInput = $("uplinkInterval");
    const uplinkLabel = uplinkInput?.closest("label");
    const lorawanGrid = document.querySelector(".device-row-lorawan .compact-grid");
    const hardwareTitle = document.querySelector(".device-row-hardware .device-copy h4");
    const hardwareText = document.querySelector(".device-row-hardware .device-copy p");

    if (!uplinkLabel || !lorawanGrid || uplinkLabel.parentElement === lorawanGrid) {
      return;
    }

    lorawanGrid.classList.remove("three");
    lorawanGrid.classList.add("four");
    lorawanGrid.appendChild(uplinkLabel);

    if (hardwareTitle) {
      hardwareTitle.textContent = "Radio wiring";
    }

    if (hardwareText) {
      hardwareText.textContent = "The pin map connects the ESP32 to the LoRa module. These pins describe the physical SPI and interrupt/reset wiring used by RadioLib.";
    }
  }

  function injectMaintenanceAndCalibrationCard() {
    if ($("factoryResetPin")) {
      return;
    }

    const sensorHeading = [...document.querySelectorAll("h2")]
      .find((heading) => heading.textContent.trim() === "3. Sensor modules");

    if (!sensorHeading) {
      return;
    }

    sensorHeading.textContent = "4. Sensor modules";
    sensorHeading.insertAdjacentHTML("beforebegin", `
      <h2>3. Maintenance and calibration pins</h2>
      <article class="device-card maintenance-card">
        <header class="device-header">
          <div>
            <h3>Startup buttons and analog calibration mode</h3>
            <p class="hint">These ESP32 pins use internal pull-ups. Connect a button from the pin to GND. When the button is held low during power-on or wake-up, the firmware enters the selected maintenance or calibration mode.</p>
          </div>
          <span class="device-badge">Service pins</span>
        </header>

        <div class="device-row device-row-project">
          <div class="device-copy">
            <h4>Reset buttons</h4>
            <p>Factory reset clears the LoRaWAN session while preserving nonces. The nonce reset also clears nonces and should only be used together with a TTN nonce reset or a new DevEUI.</p>
          </div>
          <div class="grid two compact-grid">
            <label>Factory reset pin
              <input id="factoryResetPin" type="number" value="0">
              <span class="field-help">Hold low during boot to clear the saved session.</span>
            </label>
            <label>Nonce reset pin
              <input id="dangerousNonceResetPin" type="number" value="15">
              <span class="field-help">Hold low only when nonce reset is really intended.</span>
            </label>
          </div>
        </div>

        <div class="device-row device-row-hardware">
          <div class="device-copy">
            <h4>Analog sensor calibration buttons</h4>
            <p>Holding one of these pins low starts a serial calibration loop for the selected analog sensor. Releasing the button restarts the ESP32 and normal LoRaWAN operation continues.</p>
          </div>
          <div class="grid three compact-grid">
            <label>pH calibration pin
              <input id="phCalibrationPin" type="number" value="25">
              <span class="field-help">Streams pH ADC values and calculated pH.</span>
            </label>
            <label>TDS calibration pin
              <input id="tdsCalibrationPin" type="number" value="26">
              <span class="field-help">Streams raw ADC and calculated ppm.</span>
            </label>
            <label>Turbidity calibration pin
              <input id="turbidityCalibrationPin" type="number" value="13">
              <span class="field-help">Streams voltage and calculated NTU.</span>
            </label>
          </div>
        </div>
      </article>
    `);
  }

  function patchGeneratedIni() {
    const output = $("output");
    if (!output || !output.textContent.includes("[env:")) {
      return;
    }

    let text = output.textContent;

    text = text.replace(/\n\s*-D APP_FACTORY_RESET_PIN=.*(?=\n)/, `\n    -D APP_FACTORY_RESET_PIN=${pin("factoryResetPin", 0)}`);
    text = text.replace(/\n\s*-D APP_DANGEROUS_NONCE_RESET_PIN=.*(?=\n)/, `\n    -D APP_DANGEROUS_NONCE_RESET_PIN=${pin("dangerousNonceResetPin", 15)}`);
    text = text.replace(/\n\s*-D APP_PH_CALIBRATION_PIN=.*(?=\n)/g, "");
    text = text.replace(/\n\s*-D APP_TDS_CALIBRATION_PIN=.*(?=\n)/g, "");
    text = text.replace(/\n\s*-D APP_TURBIDITY_CALIBRATION_PIN=.*(?=\n)/g, "");

    const calibrationFlags = [
      `    -D APP_PH_CALIBRATION_PIN=${pin("phCalibrationPin", 25)}`,
      `    -D APP_TDS_CALIBRATION_PIN=${pin("tdsCalibrationPin", 26)}`,
      `    -D APP_TURBIDITY_CALIBRATION_PIN=${pin("turbidityCalibrationPin", 13)}`
    ].join("\n");

    text = text.replace(
      /(\n\s*-D APP_DANGEROUS_NONCE_RESET_PIN=.*\n)/,
      `$1${calibrationFlags}\n`
    );

    output.textContent = text;
  }

  function installPlatformioPatchHooks() {
    const generateButton = $("generateButton");
    const downloadButton = $("downloadButton");

    generateButton?.addEventListener("click", () => {
      setTimeout(patchGeneratedIni, 0);
    });

    downloadButton?.addEventListener("click", (event) => {
      const output = $("output");
      if (!output || !output.textContent.includes("[env:")) {
        return;
      }

      patchGeneratedIni();
      event.preventDefault();
      event.stopImmediatePropagation();
      downloadText("platformio.ini", $("output").textContent);
    }, true);

    [
      "factoryResetPin",
      "dangerousNonceResetPin",
      "phCalibrationPin",
      "tdsCalibrationPin",
      "turbidityCalibrationPin"
    ].forEach((id) => {
      $(id)?.addEventListener("input", patchGeneratedIni);
      $(id)?.addEventListener("change", patchGeneratedIni);
    });
  }

  function enhanceUsageDocs() {
    const docs = document.querySelector(".docs-card");
    if (!docs) {
      return;
    }

    const heading = docs.querySelector("h2");
    if (heading) {
      heading.textContent = "How to use the generated files";
    }

    const monitorHeading = [...docs.querySelectorAll("h3")]
      .find((h) => h.textContent.trim() === "5. Monitor and TTN setup");

    if (!monitorHeading || docs.querySelector(".payload-formatter-docs")) {
      return;
    }

    const formatterSection = document.createElement("section");
    formatterSection.className = "payload-formatter-docs";
    formatterSection.innerHTML = `
      <h3>5. Use the generated TTN payload formatter</h3>
      <p>The firmware sends compact uplinks as byte payloads. TTN does not automatically know whether fPort 1 contains GPS data, fPort 2 contains temperature, or whether a disabled sensor should be ignored. The generated payload formatter is the translation layer between the device firmware and readable application data in The Things Stack.</p>
      <p>Generate the formatter in the dedicated card above after selecting the sensors. Then copy or download the JavaScript and paste it into the TTN uplink payload formatter for the application or for the specific end device. Regenerate it whenever the selected sensor combination changes.</p>
      <p>After installation in TTN, uplinks are shown as decoded JSON fields such as <code>temperature_c</code>, <code>ph_level</code>, <code>tds_ppm</code>, <code>turbidity_ntu</code>, or GPS coordinates. Diagnostic messages on fPorts 221, 222 and 223 are decoded as information, warnings, or errors.</p>
      <h3>6. Calibrate analog sensors</h3>
      <p>For pH, TDS, or turbidity calibration, hold the corresponding calibration button low during power-on or wake-up. The ESP32 then prints live sensor readings to the serial monitor. Release the button to restart into normal LoRaWAN operation.</p>`;

    monitorHeading.parentNode.insertBefore(formatterSection, monitorHeading);
    monitorHeading.textContent = "7. Monitor and test";

    const monitorList = monitorHeading.nextElementSibling;
    if (monitorList?.tagName === "UL") {
      monitorList.innerHTML = `
        <li>Open the serial monitor with <code>pio device monitor</code>.</li>
        <li>Check the TTN live data view to verify that uplinks arrive and that the decoded payload contains the expected fields for the selected sensors.</li>`;
    }
  }

  function commonFormatterHelpers() {
    return `function bytesToString(bytes) {
  return String.fromCharCode.apply(null, bytes || []);
}

function toNumber(value) {
  const number = Number(value);
  return Number.isFinite(number) ? number : null;
}

function parseCsvNumbers(decodedString) {
  if (!decodedString || decodedString.length === 0) {
    return [];
  }

  return decodedString.split(",").map(toNumber);
}

function numberOrDefault(value, fallback) {
  return value === null || value === undefined ? fallback : value;
}

function makeInfoResult(input, decodedString) {
  return {
    data: {
      f_port: input.fPort,
      payload_raw: decodedString,
      message: decodedString,
      level: "info"
    }
  };
}

function makeWarningResult(input, decodedString) {
  return {
    data: {
      f_port: input.fPort,
      payload_raw: decodedString,
      message: decodedString,
      level: "warning"
    },
    warnings: ["Warning: " + decodedString]
  };
}

function makeErrorResult(input, decodedString) {
  return {
    data: {
      f_port: input.fPort,
      payload_raw: decodedString,
      message: decodedString,
      level: "error"
    },
    errors: ["Error: " + decodedString]
  };
}`;
  }

  function generatePayloadFormatter() {
    hasGenerated = true;

    const lines = [];
    lines.push(`// TTN / The Things Stack uplink payload formatter.
// Generated by tools/platformio-ini-generator.
// Includes only the selected sensor fPorts plus common diagnostics.

${commonFormatterHelpers()}

function decodeUplink(input) {
  const decodedString = bytesToString(input.bytes || []);
  const variables = parseCsvNumbers(decodedString);

  switch (input.fPort) {`);

    add(lines, checked("hasGps"), `    case 1: { // GPS location
      const latitude = numberOrDefault(variables[0], 0);
      const longitude = numberOrDefault(variables[1], 0);
      const altitude = numberOrDefault(variables[2], 0);
      const hdop = numberOrDefault(variables[3], 0);

      return {
        latitude: latitude,
        longitude: longitude,
        altitude: altitude,
        hdop: hdop,
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          latitude: latitude,
          longitude: longitude,
          altitude: altitude,
          hdop: hdop
        }
      };
    }`);

    add(lines, checked("hasTemperature"), `    case 2: // DS18B20 temperature
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          temperature_c: numberOrDefault(variables[0], 0)
        }
      };`);

    add(lines, checked("hasPh"), `    case 3: // PH4502C pH level
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          ph_level: numberOrDefault(variables[0], 0)
        }
      };`);

    add(lines, checked("hasTds"), `    case 4: // Gravity TDS ppm + compensation temperature
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          tds_ppm: numberOrDefault(variables[0], 0),
          temperature_c: numberOrDefault(variables[1], 0)
        }
      };`);

    add(lines, checked("hasTurbidity"), `    case 5: // Turbidity NTU
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          turbidity_ntu: numberOrDefault(variables[0], 0)
        }
      };`);

    lines.push(`    case 220:
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          message: "Request further downlinks"
        }
      };

    case 221:
      return makeInfoResult(input, decodedString);

    case 222:
      return makeWarningResult(input, decodedString);

    case 223:
      return makeErrorResult(input, decodedString);

    default:
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          message: decodedString,
          level: "error"
        },
        errors: [
          "Error (fatal): fPort " + input.fPort + " in uplink not enabled by this formatter"
        ]
      };
  }
}
`);

    generated = lines.join("\n");

    if (out) {
      out.textContent = generated;
    }

    const copyButton = $("copyFormatterButton");
    const downloadButton = $("downloadFormatterButton");

    if (copyButton) {
      copyButton.disabled = false;
    }

    if (downloadButton) {
      downloadButton.disabled = false;
    }
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

  async function copyPayloadFormatter(event) {
    event?.stopImmediatePropagation();
    if (!generated) {
      return;
    }

    await navigator.clipboard.writeText(generated);

    const button = $("copyFormatterButton");
    if (!button) {
      return;
    }

    const oldText = button.textContent;
    button.textContent = "Copied";
    setTimeout(() => {
      button.textContent = oldText;
    }, 1200);
  }

  function downloadPayloadFormatter(event) {
    event?.stopImmediatePropagation();
    if (generated) {
      downloadText("payload-formatter.js", generated);
    }
  }

  enhanceHero();
  populateLorawanVersionOptions();
  populateRegionOptions();
  moveUplinkIntervalToLorawanProfile();
  injectMaintenanceAndCalibrationCard();
  enhanceLorawanProfileFields();
  enhanceUsageDocs();
  installPlatformioPatchHooks();
  $("generateFormatterButton")?.addEventListener("click", generatePayloadFormatter, true);
  $("copyFormatterButton")?.addEventListener("click", copyPayloadFormatter, true);
  $("downloadFormatterButton")?.addEventListener("click", downloadPayloadFormatter, true);
})();
