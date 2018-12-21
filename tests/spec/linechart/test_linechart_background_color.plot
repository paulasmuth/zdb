width: 1200px;
height: 600px;

background-color: #000;
foreground-color: #fff;

plot {
  axis-x-format: datetime("%H:%M:%S");

  layer {
    type: lines;
    x: csv('tests/testdata/measurement.csv', time);
    y: csv('tests/testdata/measurement.csv', value2);
  }
}
