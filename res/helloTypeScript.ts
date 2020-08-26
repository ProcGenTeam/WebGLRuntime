core.run(
  class extends Application {
    init(gl: WebGL2RenderingContext) {
      console.log('Initializing');
    }

    draw(gl: WebGL2RenderingContext) {
      gl.clear(gl.COLOR_BUFFER_BIT);
    }

    update(gui: ImGUIContext) {
      if (gui.button('Hello, World')) {
        console.log('Hello, World');
      }
    }
  }
);
