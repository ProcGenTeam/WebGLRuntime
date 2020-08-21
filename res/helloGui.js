const params = {
  text: "",
  float: 0.0,
  slider: 0.0,
};

function draw(gl) {
  gl.clear(gl.COLOR_BUFFER_BIT);
}

function drawGui(gui) {
  gui.beginWindow("TestingWindow", () => {
    gui.text("Hello, World");
    params.text = gui.inputText("Name", params.text);
    params.float = gui.inputNumber("Value", params.float);
    params.slider = gui.inputSlider("Value2", 0, 100, params.slider);
    if (gui.button("Test")) {
      console.log("Hello, World");
    }
  });
}
