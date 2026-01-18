
{

  double w = 0.05;
  double xRangeMin = -2;
  
  double xCenter = 4.44;

  double xMin = xCenter -0.2;
  double xMax = xCenter + 0.2;

  int binMin = (xMin - xRangeMin) / w + 1;
  int binMax = (xMax - xRangeMin) / w + 1;

  for (int i = 0; i < 24; i++){
    double value = hExi[i]->Integral(binMin, binMax);
    printf("DetID %2d: Count = %.0f\n", i, value);
    if( i % 6 == 5 ) printf("\n");
  }

}