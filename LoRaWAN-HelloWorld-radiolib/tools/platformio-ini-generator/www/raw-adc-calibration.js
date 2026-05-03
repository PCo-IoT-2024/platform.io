(() => {
  const $ = (id) => document.getElementById(id);

  function numberValue(id, fallback) {
    const element = $(id);
    const value = element?.value ?? "";
    return value.trim() === "" ? fallback : value.trim();
  }

  function setLabel(label, text) {
    if (!label) {
      return;
    }

    for (const node of label.childNodes) {
      if (node.nodeType === Node.TEXT_NODE) {
        node.textContent = text + "\n              ";
        return;
      }
    }
  }

  function setHelp(input, text) {
    const help = input?.closest("label")?.querySelector(".field-help");
    if (help) {
      help.textContent = text;
    }
  }

  function ensureInputAfter(referenceInput, id, labelText, value, helpText) {
    if ($(id) || !referenceInput) {
      return;
    }

    const referenceLabel = referenceInput.closest("label");
    if (!referenceLabel) {
      return;
    }

    const label = document.createElement("label");
    label.textContent = labelText;

    const input = document.createElement("input");
    input.id = id;
    input.value = value;

    const help = document.createElement("span");
    help.className = "field-help";
    help.textContent = helpText;

    label.appendChild(input);
    label.appendChild(help);
    referenceLabel.insertAdjacentElement("afterend", label);
  }

  function rewriteTdsCalibrationUi() {
    const lowAdc = $("tdsVcc");
    const lowPpm = $("tdsAdc");
    const highAdc = $("tdsTemp");

    if (!lowAdc || !lowPpm || !highAdc) {
      return;
    }

    const group = lowAdc.closest(".calibration-group");
    const title = group?.querySelector("h4");
    const text = group?.querySelector("p");

    if (title) {
      title.textContent = "TDS raw ADC calibration line";
    }

    if (text) {
      text.textContent = "The firmware maps raw ADC readings to ppm using two reference points. Measure the raw ADC values in known low and high TDS reference solutions.";
    }

    setLabel(lowAdc.closest("label"), "Low TDS ADC");
    lowAdc.value = "0";
    setHelp(lowAdc, "Raw ADC value measured in the low/reference solution.");

    setLabel(lowPpm.closest("label"), "Low TDS ppm");
    lowPpm.value = "0.0";
    setHelp(lowPpm, "Known ppm value of the low/reference solution.");

    setLabel(highAdc.closest("label"), "High TDS ADC");
    highAdc.value = "1800";
    setHelp(highAdc, "Raw ADC value measured in the high TDS solution.");

    ensureInputAfter(highAdc, "tdsHighPpm", "High TDS ppm", "1000.0", "Known ppm value of the high TDS solution.");
  }

  function rewriteTurbidityCalibrationUi() {
    const clearAdc = $("clearVoltage");
    const clearNtu = $("clearNtu");
    const turbidAdc = $("turbidVoltage");
    const turbidNtu = $("turbidNtu");

    if (!clearAdc || !clearNtu || !turbidAdc || !turbidNtu) {
      return;
    }

    const group = clearAdc.closest(".calibration-group");
    const title = group?.querySelector("h4");
    const text = group?.querySelector("p");

    if (title) {
      title.textContent = "Turbidity raw ADC calibration line";
    }

    if (text) {
      text.textContent = "The firmware maps raw ADC readings to NTU using two reference points. Measure one point in clear water and one point in a known turbid reference sample.";
    }

    $("turbidityVcc")?.closest("label")?.remove();
    $("turbidityAdc")?.closest("label")?.remove();

    setLabel(clearAdc.closest("label"), "Clear water ADC");
    clearAdc.value = "2500";
    setHelp(clearAdc, "Raw ADC value measured in clear water.");

    setLabel(clearNtu.closest("label"), "Clear water NTU");
    clearNtu.value = "0.0";
    setHelp(clearNtu, "Reference NTU assigned to clear water.");

    setLabel(turbidAdc.closest("label"), "Turbid water ADC");
    turbidAdc.value = "1200";
    setHelp(turbidAdc, "Raw ADC value measured in the turbid reference sample.");

    setLabel(turbidNtu.closest("label"), "Turbid water NTU");
    turbidNtu.value = "600.0";
    setHelp(turbidNtu, "Reference NTU assigned to the turbid sample.");
  }

  function removeFlag(text, name) {
    return text.replace(new RegExp(`\\n\\s*-D ${name}=.*(?=\\n)`, "g"), "");
  }

  function replaceFlag(text, name, value) {
    const re = new RegExp(`\\n\\s*-D ${name}=.*(?=\\n)`);
    if (re.test(text)) {
      return text.replace(re, `\n    -D ${name}=${value}`);
    }
    return text;
  }

  function insertAfterFlag(text, anchorName, lines) {
    const re = new RegExp(`(\\n\\s*-D ${anchorName}=.*\\n)`);
    return text.replace(re, `$1${lines.join("\n")}\n`);
  }

  function patchGeneratedIni() {
    const output = $("output");
    if (!output || !output.textContent.includes("[env:")) {
      return;
    }

    let text = output.textContent;

    text = removeFlag(text, "TDS_SENSOR_VCC");
    text = removeFlag(text, "TDS_SENSOR_ADC_RESOLUTION");
    text = removeFlag(text, "TDS_LOW_ADC_VALUE");
    text = removeFlag(text, "TDS_LOW_PPM_VALUE");
    text = removeFlag(text, "TDS_HIGH_ADC_VALUE");
    text = removeFlag(text, "TDS_HIGH_PPM_VALUE");

    text = insertAfterFlag(text, "TDS_SENSOR_PIN", [
      `    -D TDS_LOW_ADC_VALUE=${numberValue("tdsVcc", "0")}`,
      `    -D TDS_LOW_PPM_VALUE=${numberValue("tdsAdc", "0.0")}`,
      `    -D TDS_HIGH_ADC_VALUE=${numberValue("tdsTemp", "1800")}`,
      `    -D TDS_HIGH_PPM_VALUE=${numberValue("tdsHighPpm", "1000.0")}`
    ]);

    text = removeFlag(text, "TURBIDITY_VCC");
    text = removeFlag(text, "TURBIDITY_ADC_MAX");
    text = removeFlag(text, "TURBIDITY_CLEAR_WATER_VOLTAGE");
    text = removeFlag(text, "TURBIDITY_TURBID_WATER_VOLTAGE");
    text = removeFlag(text, "TURBIDITY_CLEAR_WATER_ADC_VALUE");
    text = removeFlag(text, "TURBIDITY_TURBID_WATER_ADC_VALUE");

    text = insertAfterFlag(text, "TURBIDITY_PIN", [
      `    -D TURBIDITY_CLEAR_WATER_ADC_VALUE=${numberValue("clearVoltage", "2500")}`,
      `    -D TURBIDITY_CLEAR_WATER_NTU=${numberValue("clearNtu", "0.0")}`,
      `    -D TURBIDITY_TURBID_WATER_ADC_VALUE=${numberValue("turbidVoltage", "1200")}`,
      `    -D TURBIDITY_TURBID_WATER_NTU=${numberValue("turbidNtu", "600.0")}`
    ]);

    text = replaceFlag(text, "PH4_ADC_VALUE", numberValue("ph4", "2503"));
    text = replaceFlag(text, "PH7_ADC_VALUE", numberValue("ph7", "2080"));
    text = replaceFlag(text, "PH10_ADC_VALUE", numberValue("ph10", "1615"));

    output.textContent = text;
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

  function installHooks() {
    $("generateButton")?.addEventListener("click", () => {
      setTimeout(patchGeneratedIni, 0);
    });

    $("downloadButton")?.addEventListener("click", (event) => {
      const output = $("output");
      if (!output || !output.textContent.includes("[env:")) {
        return;
      }

      patchGeneratedIni();
      event.preventDefault();
      event.stopImmediatePropagation();
      downloadText("platformio.ini", output.textContent);
    }, true);

    [
      "tdsVcc", "tdsAdc", "tdsTemp", "tdsHighPpm",
      "clearVoltage", "clearNtu", "turbidVoltage", "turbidNtu",
      "ph4", "ph7", "ph10"
    ].forEach((id) => {
      $(id)?.addEventListener("input", patchGeneratedIni);
      $(id)?.addEventListener("change", patchGeneratedIni);
    });
  }

  function run() {
    rewriteTdsCalibrationUi();
    rewriteTurbidityCalibrationUi();
    installHooks();
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", run);
  } else {
    run();
  }
})();
