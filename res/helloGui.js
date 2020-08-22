console.log(core.viewportWidth, core.viewportHeight);

const params = {
  text: "",
  float: 0.0,
  slider: 0.0,
  check: false,
};

function draw(gl) {
  gl.clear(gl.COLOR_BUFFER_BIT);
}

function drawGui(gui) {
  gui.beginWindow("TestingWindow", () => {
    gui.text("Hello, World");

    gui.text(`Current Size: ${core.viewportWidth}x${core.viewportHeight}`);

    params.text = gui.inputText("Name", params.text);
    params.float = gui.inputNumber("Value", params.float);
    params.slider = gui.inputSlider("Value2", 0, 100, params.slider);
    params.check = gui.checkbox("Check", params.check);

    if (gui.button("Test")) {
      console.log("Hello, World");
    }
  });
}
