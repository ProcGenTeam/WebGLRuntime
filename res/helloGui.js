function draw(gl) {
  gl.clear(gl.COLOR_BUFFER_BIT);
}

function drawGui(gui) {
  gui.beginWindow("TestingWindow", () => {
    if (gui.button("Test")) {
      console.log("Hello, World");
    }
  });
}
