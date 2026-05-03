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
    const regions = [["EU868", "EU868 — Europe 863–870 MHz"], ["US915", "US915 — North America 902–928 MHz"], ["EU433", "EU433 — Europe 433 MHz"], ["AU915", "AU915 — Australia 915–928 MHz"], ["CN470", "CN470 — China 470–510 MHz"], ["AS923", "AS923-1 — Asia-Pacific 923 MHz"], ["AS923_2", "AS923-2 — Asia-Pacific variant"], ["AS923_3", "AS923-3 — Asia-Pacific variant"], ["AS923_4", "AS923-4 — Asia-Pacific variant"], ["KR920", "KR920 — South Korea 920–923 MHz"], ["IN865", "IN865 — India 865–867 MHz"]];
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
    if ($("hasPhBoardTemperature")) return;

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

    phCard.querySelector(".sensor-header")?.insertAdjacentHTML("beforeend", `<span class="sensor-port">fPort 6 temp</span>`);
    phCard.querySelector(".sensor-description")?.insertAdjacentHTML("afterend", `<p class="calibration-note ph-board-temperature-note"><label class="sensor-toggle"><input type="checkbox" id="hasPhBoardTemperature" checked> PH4502C board temperature</label><br>This uses the analog temperature output on the PH4502C board as a separate air/board temperature value.</p>`);

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

    ["APP_HAS_PH_BOARD_TEMPERATURE", "APP_PH_BOARD_TEMPERATURE_CALIBRATION_PIN", "PH_BOARD_TEMPERATURE_LOW_ADC_VALUE", "PH_BOARD_TEMPERATURE_LOW_C_VALUE", "PH_BOARD_TEMPERATURE_HIGH_ADC_VALUE", "PH_BOARD_TEMPERATURE_HIGH_C_VALUE"].forEach((name) => {
      text = removeFlag(text, name);
    });

    text = insertAfter(text, "APP_PH_CALIBRATION_PIN", [`    -D APP_PH_BOARD_TEMPERATURE_CALIBRATION_PIN=${val("phBoardTemperatureCalibrationPin", "12")}`]);
    text = insertAfter(text, "PH10_ADC_VALUE", [
      `    -D PH_BOARD_TEMPERATURE_LOW_ADC_VALUE=${val("phBoardTemperatureLowAdc", "1200")}`,
      `    -D PH_BOARD_TEMPERATURE_LOW_C_VALUE=${val("phBoardTemperatureLowC", "20.0")}`,
      `    -D PH_BOARD_TEMPERATURE_HIGH_ADC_VALUE=${val("phBoardTemperatureHighAdc", "1800")}`,
      `    -D PH_BOARD_TEMPERATURE_HIGH_C_VALUE=${val("phBoardTemperatureHighC", "40.0")}`
    ]);
    text = insertAfter(text, "APP_HAS_PH", [`    -D APP_HAS_PH_BOARD_TEMPERATURE=${$("hasPhBoardTemperature")?.checked ? "1" : "0"}`]);

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
    ["hasPhBoardTemperature", "phBoardTemperatureCalibrationPin", "phBoardTemperatureLowAdc", "phBoardTemperatureLowC", "phBoardTemperatureHighAdc", "phBoardTemperatureHighC"].forEach((id) => {
      $(id)?.addEventListener("input", patchGeneratedIni);
      $(id)?.addEventListener("change", patchGeneratedIni);
    });
  }

  function enabled(id) { return !!$(id)?.checked; }
  function addCase(lines, isEnabled, caseText) { if (isEnabled) lines.push(caseText); }

  function generatePayloadFormatter() {
    const lines = [];
    lines.push("// TTN / The Things Stack uplink payload formatter.");
    lines.push("// Generated by tools/platformio-ini-generator.");
    lines.push("");
    lines.push("function ascii(bytes) {");
    lines.push("  var s = \"\";");
    lines.push("  for (var i = 0; i < bytes.length; ++i) s += String.fromCharCode(bytes[i]);");
    lines.push("  return s;");
    lines.push("}");
    lines.push("");
    lines.push("function nums(text) {");
    lines.push("  if (!text) return [];");
    lines.push("  return text.split(\",\").map(function (v) { var n = Number(v); return isFinite(n) ? n : 0; });");
    lines.push("}");
    lines.push("");
    lines.push("function decodeUplink(input) {");
    lines.push("  var text = ascii(input.bytes || []);");
    lines.push("  var v = nums(text);");
    lines.push("  switch (input.fPort) {");
    addCase(lines, enabled("hasGps"), "    case 1:\n      return { data: { f_port: input.fPort, payload_raw: text, latitude: v[0] || 0, longitude: v[1] || 0, altitude: v[2] || 0, hdop: v[3] || 0 } };" );
    addCase(lines, enabled("hasTemperature"), "    case 2:\n      return { data: { f_port: input.fPort, payload_raw: text, temperature_c: v[0] || 0 } };" );
    addCase(lines, enabled("hasPh"), "    case 3:\n      return { data: { f_port: input.fPort, payload_raw: text, ph_level: v[0] || 0 } };" );
    addCase(lines, enabled("hasTds"), "    case 4:\n      return { data: { f_port: input.fPort, payload_raw: text, tds_ppm: v[0] || 0, temperature_c: v[1] || 0 } };" );
    addCase(lines, enabled("hasTurbidity"), "    case 5:\n      return { data: { f_port: input.fPort, payload_raw: text, turbidity_ntu: v[0] || 0 } };" );
    addCase(lines, enabled("hasPhBoardTemperature"), "    case 6:\n      return { data: { f_port: input.fPort, payload_raw: text, ph_board_temperature_c: v[0] || 0 } };" );
    lines.push("    case 220:");
    lines.push("    case 221:");
    lines.push("      return { data: { f_port: input.fPort, payload_raw: text, message: text, level: \"info\" } };");
    lines.push("    case 222:");
    lines.push("      return { data: { f_port: input.fPort, payload_raw: text, message: text, level: \"warning\" }, warnings: [text] };");
    lines.push("    case 223:");
    lines.push("      return { data: { f_port: input.fPort, payload_raw: text, message: text, level: \"error\" }, errors: [text] };");
    lines.push("    default:");
    lines.push("      return { data: { f_port: input.fPort, payload_raw: text }, warnings: [\"Unsupported fPort \" + input.fPort] };");
    lines.push("  }");
    lines.push("}");
    generatedFormatter = lines.join("\n") + "\n";
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

  async function copyPayloadFormatter() { if (generatedFormatter) await navigator.clipboard.writeText(generatedFormatter); }
  function downloadPayloadFormatter() { if (generatedFormatter) downloadText("payload-formatter.js", generatedFormatter); }
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
