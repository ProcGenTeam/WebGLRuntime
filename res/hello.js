let frame = 0;

function draw(ctx) {
  for (let i = 0; i < 255; i++) {
    ctx.setDrawColor(i + 100, i, i);
    ctx.fillRect(Math.sin(frame / i) * 200 + 200, i * 4, 100, 3);
  }

  frame++;
}

console.log(
  "call",
  callMe(() => testProp)
);
