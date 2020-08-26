import * as THREE from 'three';

console.error = function (...args) {
  console.log('error', ...args);
};

class ThreeTestApplication extends Application {
  scene = new THREE.Scene();
  camera?: THREE.PerspectiveCamera = undefined;
  renderer?: THREE.WebGLRenderer = undefined;
  cube?: THREE.Mesh = undefined;

  init(gl: WebGL2RenderingContext) {
    const fakeCanvas = {
      get width() {
        return core.viewportWidth;
      },
      set width(v) {},
      get height() {
        return core.viewportHeight;
      },
      set height(v) {},
      getContext() {
        return gl;
      },
      addEventListener() {},
      style: {},
    };

    this.camera = new THREE.PerspectiveCamera(
      75,
      fakeCanvas.width / fakeCanvas.height,
      0.1,
      1000
    );

    this.renderer = new THREE.WebGLRenderer({
      canvas: (fakeCanvas as unknown) as HTMLCanvasElement,
    });
    this.renderer.setSize(fakeCanvas.width, fakeCanvas.height);

    var geometry = new THREE.BoxGeometry();
    var material = new THREE.MeshBasicMaterial({color: 0x00ff00});

    this.cube = new THREE.Mesh(geometry, material);
    this.scene.add(this.cube);

    this.camera.position.z = 5;
  }

  draw(gl: WebGL2RenderingContext) {
    if (
      this.cube === undefined ||
      this.renderer === undefined ||
      this.camera === undefined
    ) {
      throw new Error('Not Implemented');
    }

    this.cube.rotation.x += 0.01;
    this.cube.rotation.y += 0.01;
    this.renderer.render(this.scene, this.camera);
  }

  update(gui: ImGUIContext) {}
}

core.run(ThreeTestApplication);
