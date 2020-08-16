// Based on https://www.tutorialspoint.com/webgl/webgl_drawing_a_triangle.htm

const vertices = [-0.5, 0.5, 0.0, -0.5, -0.5, 0.0, 0.5, -0.5, 0.0];

const indices = [0, 1, 2];

const vertexBuffer = null;
const indexBuffer = null;
const shaderProgram = null;

/*
# Functions

- bindBuffer
- bufferData
- createShader
- shaderSource
- compileShader
- createProgram
- attachShader
- linkProgram
- clearColor
- enable
- clear
- viewpoint
- useProgram
- getAttribLocation
- vertexAttribPointer
- enableVertexAttribArray
- drawElements

# Constants

- ARRAY_BUFFER
- ELEMENT_ARRAY_BUFFER
- VERTEX_SHADER
- FRAGMENT_SHADER
- DEPTH_TEST
- COLOR_BUFFER_BIT
- FLOAT
- TRIANGLES
- UNSIGNED_SHORT

# Classes

- WebGLBuffer
- WebGLShader
- WebGLProgram

# Primitives

- Float32Array
- Uint16Array

*/

/**
 *
 * @param {WebGLRenderingContext} gl
 */
function init(gl) {
  vertexBuffer = gl.createBuffer();

  gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices));
  gl.bindBuffer(gl.ARRAY_BUFFER, null);

  indexBuffer = gl.createBuffer();

  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
  gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices));
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);

  const vertSource = `
attribute vec3 coordinates;
				
void main(void) {
  gl_Position = vec4(coordinates, 1.0);
}`;

  const vertShader = gl.createShader(gl.VERTEX_SHADER);
  gl.shaderSource(vertShader, vertSource);
  gl.compileShader(vertShader);

  const fragSource = `
void main(void) {
  gl_FragColor = vec4(0.0, 0.0, 0.0, 0.1);
}`;

  const fragShader = gl.createShader(gl.FRAGMENT_SHADER);
  gl.shaderSource(fragShader, fragSource);
  gl.compileShader(fragShader);

  shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertShader);
  gl.attachShader(shaderProgram, fragShader);
  gl.linkProgram(shaderProgram);
}

/**
 *
 * @param {WebGLRenderingContext} gl
 */
function draw(gl) {
  gl.clearColor(0.5, 0.5, 0.5, 0.9);

  gl.enable(gl.DEPTH_TEST);

  gl.clear(gl.COLOR_BUFFER_BIT);

  gl.viewport(0, 0, 800, 600);

  gl.useProgram(shaderProgram);
  gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);

  const coordinates = gl.getAttribLocation(shaderProgram, "coordinates");
  gl.vertexAttribPointer(coordinates, 3, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(coordinates);

  gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_SHORT, 0);
}
