function draw(ctx) {
  ctx.setDrawColor(255, 0, 0);
  ctx.fillRect(10, 10, 100, 100);

  ctx.setDrawColor(0, 255, 0);
  ctx.fillRect(120, 10, 100, 100);

  ctx.setDrawColor(0, 0, 255);
  ctx.fillRect(10, 120, 100, 100);

  ctx.setDrawColor(255, 255, 255);
  ctx.fillRect(120, 120, 100, 100);
}
