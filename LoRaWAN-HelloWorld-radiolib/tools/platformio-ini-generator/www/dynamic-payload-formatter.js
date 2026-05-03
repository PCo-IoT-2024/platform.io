(() => {
  const $ = (id) => document.getElementById(id);
  let generatedFormatter = "";

  function enhanceHero() {
    const lead = document.querySelector(".lead");
    if (lead) {
      lead.innerHTML = 'Generate a tailored <code>platformio.ini</code> and matching <code>payload-formatter.js</code> for the selected radio module, LoRaWAN key model, <a href="https://www.thethingsindustries.com/docs/concepts/ttn/" target="_blank" rel="noopener noreferrer">The Things Networks Sandbox (TTN)</a> device credentials, sensor set, and pin mapping.';
    }
  }

  function populateLorawanKeyModelOptions() {
    const version = $("lorawanVersion");
    if (!version) return;

    const selected = version.value === "1.1.0" ? "1.1.0" : "1.0.x";
    version.replaceChildren();

    [["1.1.0", "LoRaWAN 1.1.0 key model"], ["1.0.x", "LoRaWAN 1.0.x key model"]].forEach(([value, label]) => {
      const option = document.createElement("option");
      option.value = value;
      option.textContent = label;
      version.appendChild(option);
    });

    version.value = selected;
    version.dispatchEvent(new Event("change"));
  }

  function populateRegionOptions() {
    const region = $("region");
    if (!region) return;

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
    regions.forEach(([value, label]) => {
      const option = document.createElement("option");
      option.value = value;
      option.textContent = label;
      region.appendChild(option);
    });
    region.value = regions.some(([value]) => value === selected) ? selected : "EU868";
  }

  function appendFieldHelp(controlId, text) {
    const label = $(controlId)?.closest("label");
    if (!label || label.querySelector(".field-help")) return;

    const help = document.createElement("span");
    help.className = "field-help";
    help.textContent = text;
    label.appendChild(help);
  }

  function enhanceLorawanProfileFields() {
    appendFieldHelp("radioModule", "Transceiver chip mounted on the ESP32 radio board.");
    appendFieldHelp("lorawanVersion", "Select AppKey + NwkKey for 1.1.0, or AppKey-only for 1.0.x devices.");
    appendFieldHelp("region", "Frequency plan. Use EU868 for this course in Europe.");
  }

  function moveUplinkIntervalToLorawanProfile() {
    const uplinkLabel = $("uplinkInterval")?.closest("label");
    const lorawanGrid = document.querySelector(".device-row-lorawan .compact-grid");
    if (!uplinkLabel || !lorawanGrid || uplinkLabel.parentElement === lorawanGrid) return;

    lorawanGrid.classList.remove("three");
    lorawanGrid.classList.add("four");
    lorawanGrid.appendChild(uplinkLabel);
  }

  function injectPhBoardTemperatureUi() {
    if ($("phBoardTemperatureCalibrationPin")) return;

    const calibrationGrid = document.querySelector(".maintenance-card .device-row-hardware .compact-grid");
    if (calibrationGrid) {
      calibrationGrid.classList.remove("three");
      calibrationGrid.classList.add("four");
      calibrationGrid.insertAdjacentHTML("beforeend", `<label>pH board temp. calibration pin
        <input id="phBoardTemperatureCalibrationPin" type="number" value="12">
        <span class="field-help">Streams raw ADC and °C.</span>
      </label>`);
    }

    const phCard = $("hasPh")?.closest("article");
    const phCalibrationGroup = phCard?.querySelector(".calibration-group");
    if (!phCard || !phCalibrationGroup) return;

    phCard.querySelector(".sensor-header")?.insertAdjacentHTML("beforeend", `<span class="sensor-port">fPort 6</span>`);

    phCalibrationGroup.insertAdjacentHTML("afterend", `<div class="calibration-group ph-board-temperature-calibration">
      <h4>PH4502C board temperature raw ADC calibration line</h4>
      <p>The firmware maps raw ADC readings from the PH4502C temperature pin to °C using two reference points.</p>
      <div class="grid three compact-grid">
        <label>Low temp ADC
          <input id="phBoardTemperatureLowAdc" type="number" value="1200">
          <span class="field-help">Raw ADC at the low temperature reference.</span>
        </label>
        <label>Low temp °C
          <input id="phBoardTemperatureLowC" value="20.0">
          <span class="field-help">Known low reference temperature.</span>
        </label>
        <label>High temp ADC
          <input id="phBoardTemperatureHighAdc" type="number" value="1800">
          <span class="field-help">Raw ADC at the high temperature reference.</span>
        </label>
        <label>High temp °C
          <input id="phBoardTemperatureHighC" value="40.0">
          <span class="field-help">Known high reference temperature.</span>
        </label>
      </div>
    </div>`);
  }

  function val(id, fallback) {
    const value = $(id)?.value?.trim() ?? "";
    return value === "" ? fallback : value;
  }

  function removeFlag(text, name) {
    return text.replace(new RegExp(`\\n\\s*-D ${name}=.*(?=\\n)`, "g"), "");
  }

  function insertAfter(text, anchorName, lines) {
    const re = new RegExp(`(\\n\\s*-D ${anchorName}=.*\\n)`);
    return text.replace(re, `$1${lines.join("\n")}\n`);
  }

  function patchGeneratedIni() {
    const output = $("output");
    if (!output || !output.textContent.includes("[env:")) return;

    let text = output.textContent;
    [
      "APP_HAS_PH_BOARD_TEMPERATURE",
      "APP_PH_BOARD_TEMPERATURE_CALIBRATION_PIN",
      "PH_BOARD_TEMPERATURE_LOW_ADC_VALUE",
      "PH_BOARD_TEMPERATURE_LOW_C_VALUE",
      "PH_BOARD_TEMPERATURE_HIGH_ADC_VALUE",
      "PH_BOARD_TEMPERATURE_HIGH_C_VALUE"
    ].forEach((name) => {
      text = removeFlag(text, name);
    });

    text = insertAfter(text, "APP_PH_CALIBRATION_PIN", [
      `    -D APP_PH_BOARD_TEMPERATURE_CALIBRATION_PIN=${val("phBoardTemperatureCalibrationPin", "12")}`
    ]);
    text = insertAfter(text, "PH10_ADC_VALUE", [
      `    -D PH_BOARD_TEMPERATURE_LOW_ADC_VALUE=${val("phBoardTemperatureLowAdc", "1200")}`,
      `    -D PH_BOARD_TEMPERATURE_LOW_C_VALUE=${val("phBoardTemperatureLowC", "20.0")}`,
      `    -D PH_BOARD_TEMPERATURE_HIGH_ADC_VALUE=${val("phBoardTemperatureHighAdc", "1800")}`,
      `    -D PH_BOARD_TEMPERATURE_HIGH_C_VALUE=${val("phBoardTemperatureHighC", "40.0")}`
    ]);
    text = insertAfter(text, "APP_HAS_PH", ["    -D APP_HAS_PH_BOARD_TEMPERATURE=1"]);

    output.textContent = text;
  }

  function installIniPatchHooks() {
    $("generateButton")?.addEventListener("click", () => setTimeout(patchGeneratedIni, 0));
    $("downloadButton")?.addEventListener("click", (event) => {
      const output = $("output");
      if (!output || !output.textContent.includes("[env:")) return;
      patchGeneratedIni();
      event.preventDefault();
      event.stopImmediatePropagation();
      downloadText("platformio.ini", output.textContent);
    }, true);

    [
      "phBoardTemperatureCalibrationPin",
      "phBoardTemperatureLowAdc",
      "phBoardTemperatureLowC",
      "phBoardTemperatureHighAdc",
      "phBoardTemperatureHighC"
    ].forEach((id) => {
      $(id)?.addEventListener("input", patchGeneratedIni);
      $(id)?.addEventListener("change", patchGeneratedIni);
    });
  }

  function enabled(id) {
    return !!$(id)?.checked;
  }

  function formatterCase(fPort, comment, fields) {
    const dataLines = fields.map(([name, index], fieldIndex) => {
      const comma = fieldIndex + 1 < fields.length ? "," : "";
      return `          ${name}: numberOrDefault(values[${index}], 0)${comma}`;
    });

    return [
      `    case ${fPort}: { // ${comment}`,
      "      var values = parseCsvNumbers(text);",
      "",
      "      return {",
      "        data: {",
      dataLines.join("\n"),
      "        }",
      "      };",
      "    }"
    ].join("\n");
  }

  function generatePayloadFormatter() {
    const cases = [];

    if (enabled("hasGps")) {
      cases.push(formatterCase(1, "GPS location", [["latitude", 0], ["longitude", 1], ["altitude", 2], ["hdop", 3]]));
    }
    if (enabled("hasTemperature")) {
      cases.push(formatterCase(2, "DS18B20 water temperature", [["temperature_c", 0]]));
    }
    if (enabled("hasPh")) {
      cases.push(formatterCase(3, "PH4502C pH level", [["ph_level", 0]]));
    }
    if (enabled("hasTds")) {
      cases.push(formatterCase(4, "Gravity TDS ppm", [["tds_ppm", 0]]));
    }
    if (enabled("hasTurbidity")) {
      cases.push(formatterCase(5, "Turbidity NTU", [["turbidity_ntu", 0]]));
    }

    cases.push(formatterCase(6, "PH4502C board temperature", [["ph_board_temperature_c", 0]]));

    generatedFormatter = `// TTN / The Things Stack uplink payload formatter.
// Generated by tools/platformio-ini-generator.

function bytesToString(bytes) {
  var text = "";

  for (var i = 0; i < bytes.length; ++i) {
    text += String.fromCharCode(bytes[i]);
  }

  return text;
}

function toNumber(value) {
  var number = Number(value);
  return isFinite(number) ? number : null;
}

function parseCsvNumbers(text) {
  if (!text) {
    return [];
  }

  return text.split(",").map(toNumber);
}

function numberOrDefault(value, fallback) {
  return value === null || value === undefined ? fallback : value;
}

function decodeUplink(input) {
  var text = bytesToString(input.bytes || []);

  switch (input.fPort) {
${cases.join("\n\n")}

    case 220:
    case 221:
      return {
        data: {
          message: text,
          level: "info"
        }
      };

    case 222:
      return {
        data: {
          message: text,
          level: "warning"
        },
        warnings: [text]
      };

    case 223:
      return {
        data: {
          message: text,
          level: "error"
        },
        errors: [text]
      };

    default:
      return {
        warnings: ["Unsupported fPort " + input.fPort]
      };
  }
}
`;

    const output = $("payloadFormatterOutput");
    if (output) output.textContent = generatedFormatter;
    if ($("copyFormatterButton")) $("copyFormatterButton").disabled = false;
    if ($("downloadFormatterButton")) $("downloadFormatterButton").disabled = false;
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

  async function copyPayloadFormatter() {
    if (generatedFormatter) {
      await navigator.clipboard.writeText(generatedFormatter);
    }
  }

  function downloadPayloadFormatter() {
    if (generatedFormatter) {
      downloadText("payload-formatter.js", generatedFormatter);
    }
  }

  function initializePayloadFormatterCard() {
    const output = $("payloadFormatterOutput");
    if (output) output.textContent = "Press “Generate payload formatter”.";
    if ($("copyFormatterButton")) $("copyFormatterButton").disabled = true;
    if ($("downloadFormatterButton")) $("downloadFormatterButton").disabled = true;
  }

  enhanceHero();
  populateLorawanKeyModelOptions();
  populateRegionOptions();
  moveUplinkIntervalToLorawanProfile();
  enhanceLorawanProfileFields();
  injectPhBoardTemperatureUi();
  installIniPatchHooks();
  initializePayloadFormatterCard();

  $("generateFormatterButton")?.addEventListener("click", generatePayloadFormatter);
  $("copyFormatterButton")?.addEventListener("click", copyPayloadFormatter);
  $("downloadFormatterButton")?.addEventListener("click", downloadPayloadFormatter);
})();
