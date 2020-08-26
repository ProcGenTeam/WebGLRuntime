core.run(
  class extends Application {
    init(gl) {
      console.log("Initializing");
    }

    draw(gl) {
      gl.clear(gl.COLOR_BUFFER_BIT);
    }

    update(gui) {
      if (gui.button("Hello, World")) {
        console.log("Hello, World");
      }
    }
  }
);
