function expect(message: string): never {
  throw new Error(message);
}

class ShaderProgram {
  private _program: WebGLProgram | undefined = undefined;

  get program() {
    return this._program || expect('Program has not been created.');
  }

  constructor(readonly gl: WebGL2RenderingContext) {}

  compileAndLink(vertexSource: string, fragmentSource: string) {
    const vertexShader = this.compileShader(
      this.gl.VERTEX_SHADER,
      vertexSource
    );
    const fragmentShader = this.compileShader(
      this.gl.FRAGMENT_SHADER,
      fragmentSource
    );

    this._program =
      this.gl.createProgram() || expect('Failed to create Program');

    this.gl.attachShader(this._program, vertexShader);
    this.gl.attachShader(this._program, fragmentShader);

    this.gl.linkProgram(this._program);

    if (!this.gl.getProgramParameter(this._program, this.gl.LINK_STATUS)) {
      throw new Error(
        'Linking shader failed: ' + this.gl.getProgramInfoLog(this._program)
      );
    }
  }

  setUniform2(uniformName: string, x: number, y: number) {
    this.gl.useProgram(this.program);

    const location = this.gl.getUniformLocation(this.program, uniformName);

    this.gl.uniform2f(location, x, y);
  }

  setUniform1(uniformName: string, x: number) {
    this.gl.useProgram(this.program);

    const location = this.gl.getUniformLocation(this.program, uniformName);

    this.gl.uniform1f(location, x);
  }

  private compileShader(type: number, source: string): WebGLShader {
    const shader =
      this.gl.createShader(type) || expect('Failed to create Shader');

    this.gl.shaderSource(shader, source);

    this.gl.compileShader(shader);

    if (!this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS)) {
      const shaderInfoLog = this.gl.getShaderInfoLog(shader);
      throw new Error('Compiling shader failed: ' + shaderInfoLog);
    }

    return shader;
  }
}

class Mesh {
  private buffer: WebGLBuffer | undefined = undefined;

  private vertexCount = 0;

  constructor(readonly gl: WebGL2RenderingContext) {}

  uploadVertexes(vertexList: Float32Array, vertexCount: number) {
    this.buffer = this.gl.createBuffer() || expect('Failed to create buffer');

    this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffer);
    this.gl.bufferData(this.gl.ARRAY_BUFFER, vertexList, this.gl.STATIC_DRAW);
    this.gl.bindBuffer(this.gl.ARRAY_BUFFER, null);

    this.vertexCount = vertexCount;
  }

  draw(shader: ShaderProgram) {
    if (this.buffer === undefined) {
      throw new Error('Buffer not Initialized');
    }

    this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffer);
    this.gl.useProgram(shader.program);

    const coordinates = this.gl.getAttribLocation(
      shader.program,
      'coordinates'
    );
    this.gl.vertexAttribPointer(coordinates, 3, this.gl.FLOAT, false, 0, 0);
    this.gl.enableVertexAttribArray(coordinates);

    this.gl.drawArrays(this.gl.TRIANGLES, 0, this.vertexCount);

    // this.gl.useProgram(null);
    // this.gl.bindBuffer(this.gl.ARRAY_BUFFER, null);
  }
}

const VERTEX_SOURCE = `#version 330

in vec3 coordinates;

out vec3 position;

void main(void) {
  gl_Position = vec4(coordinates, 1.0);

  position = coordinates;
}`;

const FRAGMENT_SOURCE = `#version 330

in vec3 position;

uniform vec2 viewportSize;

uniform float persistence;

// From: https://www.shadertoy.com/view/4lB3zz

uniform float firstOctave;
uniform float octaves;
uniform float octaveStep;

float noise(int x,int y)
{   
    float fx = float(x);
    float fy = float(y);
    
    return 2.0 * fract(sin(dot(vec2(fx, fy) ,vec2(12.9898,78.233))) * 43758.5453) - 1.0;
}

float smoothNoise(int x,int y)
{
    return noise(x,y)/4.0+(noise(x+1,y)+noise(x-1,y)+noise(x,y+1)+noise(x,y-1))/8.0+(noise(x+1,y+1)+noise(x+1,y-1)+noise(x-1,y+1)+noise(x-1,y-1))/16.0;
}

float COSInterpolation(float x, float y, float n) {
    float r = n * 3.1415926;
    float f = (1.0 - sin(r)) * 0.5;
    return x * (1.0-f) + y * f;
}

float InterpolationNoise(float x, float y)
{
    int ix = int(x);
    int iy = int(y);
    float fracx = x-float(int(x));
    float fracy = y-float(int(y));
    
    float v1 = smoothNoise(ix,iy);
    float v2 = smoothNoise(ix+1,iy);
    float v3 = smoothNoise(ix,iy+1);
    float v4 = smoothNoise(ix+1,iy+1);
    
   	float i1 = COSInterpolation(v1,v2,fracx);
    float i2 = COSInterpolation(v3,v4,fracx);
    
    return COSInterpolation(i1,i2,fracy);
    
}

float PerlinNoise2D(float x, float y)
{
    float sum = 0.0;
    float frequency = 0.0;
    float amplitude = 0.0;
    for(float i=firstOctave;i<octaves + firstOctave;i += octaveStep)
    {
        frequency = pow(2.0,float(i));
        amplitude = pow(persistence,float(i));
        sum = sum + InterpolationNoise(x*frequency,y*frequency)*amplitude;
    }
    
    return sum;
}

uniform float factor;

void main(void) {
  vec2 screenSpaceCoords = position.xy;

  vec2 uv = viewportSize.xy * screenSpaceCoords.xy * factor;
  float x = uv.x;
  float y = uv.y;
  float noiseR = 0.3 + 0.7 * PerlinNoise2D(-x, y);
  float noiseG = 0.3 + 0.7 * PerlinNoise2D(x, -y);
  float noiseB = 0.3 + 0.7 * PerlinNoise2D(x, y);

  gl_FragColor = vec4(noiseR, noiseG, noiseB, 1.0);
}`;

class TerrainApplication extends Application {
  private simpleProgram: ShaderProgram | undefined = undefined;
  private fullScreenRect: Mesh | undefined = undefined;

  private float1s: Record<string, number> = {};

  init(gl: WebGL2RenderingContext) {
    console.log('Initializing');

    this.simpleProgram = new ShaderProgram(gl);

    this.simpleProgram.compileAndLink(VERTEX_SOURCE, FRAGMENT_SOURCE);

    this.fullScreenRect = new Mesh(gl);

    this.fullScreenRect.uploadVertexes(
      new Float32Array([
        -1,
        -1,
        0,

        1,
        -1,
        0,

        -1,
        1,
        0,

        1,
        -1,
        0,

        -1,
        1,
        0,

        1,
        1,
        0,
      ]),
      6
    );
  }

  draw(gl: WebGL2RenderingContext) {
    gl.clear(gl.COLOR_BUFFER_BIT);

    const mesh = this.fullScreenRect || expect('Failed to get mesh');
    const program = this.simpleProgram || expect('Failed to get program');

    program.setUniform2(
      'viewportSize',
      core.viewportWidth,
      core.viewportHeight
    );

    for (const [key, value] of Object.entries(this.float1s)) {
      program.setUniform1(key, value);
    }

    mesh.draw(program);
  }

  update(gui: ImGUIContext) {
    this.float1(gui, 'persistence', 0.0, 1.0, 0.5);
    this.float1(gui, 'firstOctave', -10.0, 10.0, 3.0);
    this.float1(gui, 'octaves', 0.0, 20.0, 8.0);
    this.float1(gui, 'octaveStep', 0.2, 2, 1.0);
    this.float1(gui, 'factor', -10.0, 10.0, 1.0);

    gui.text('Current Fragment Source:\n' + FRAGMENT_SOURCE);
  }

  private float1(
    gui: ImGUIContext,
    name: string,
    min: number,
    max: number,
    defaultValue: number
  ) {
    if (this.float1s[name] === undefined) {
      this.float1s[name] = defaultValue;
    }

    this.float1s[name] = gui.inputSlider(name, min, max, this.float1s[name]);
  }
}

core.run(TerrainApplication);
