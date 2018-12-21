width: 1200px;
height: 480px;

plot {
  title: "To the Moon";

  x: csv('examples/data/log_example.csv', x);
  y: csv('examples/data/log_example.csv', y);

  axis-x: log;
  axis-x-min: 0;
  axis-x-max: 5;

  axis-y: log;
  axis-y-min: 0;
  axis-y-max: 10000;

  layer {
    type: lines;
  }
}
