/* $Id: 2d_fun.c,v 1.29 2007/06/13 20:21:04 tl Exp $ */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define MAT(n,i,j) *(n + j * nx + i)


/*============================================================*/

int
read_dm(char * fn, int * nx, int * ny, char * machinetype, char * datatype)
{

  /* declarations */

  int             i;
  char            str[132];
  FILE           *fp;

  /* open dm file */

  printf("[%s]\n", fn);
  fflush(stdout);
  i = 0;
  sprintf(str, "%s.dm", fn);
  printf("[%s]\n", str);
  fflush(stdout);
  if ((fp = fopen(str, "r")) == NULL)
  {
    printf("could not open <%s>\n", str);
    printf("\n");
    return (-1);
  } else
    printf("read_dm: .dm file: <%s> is open...\n", str);
  fflush(stdout);

  /* read dm file */

  fscanf(fp, "\n%i", nx);
  fscanf(fp, "\n%i", ny);
  fscanf(fp, "\n%s", machinetype);
  fscanf(fp, "\n%s", datatype);
  fclose(fp);

  printf("x dim.......: %4i; ", *nx);
  printf("y dim.......: %4i\n", *ny);
  printf("machine type: %s; ", machinetype);
  printf("data type...: %s\n", datatype);


#if(0)
  /* trap */

  if (*nx != *ny)
  {
    printf("sorry can only handle square matrices\n");
    exit(0);
  };
#endif 

  /* done */

  return (0);

}

/*============================================================*/

int
wr_dm(char * fn, int nx, int ny, char * machinetype, char * datatype)
{

  /* declarations */

  int             i;
  char            str[132];
  FILE           *fp;

#if(0)
  /* trap */

  if (nx != ny)
  {
    printf("sorry can only handle square matrices\n");
    exit(0);
  };
#endif  

  /* open dm file */

  i = 0;
  sprintf(str, "%s.dm", fn);
  if ((fp = fopen(str, "w")) == NULL)
  {
    printf("could not open <%s>\n", str);
    printf("\n");
    return (0);
  };

  /* write dm file */

  fprintf(fp, "%i\n", nx);
  fprintf(fp, "%i\n", ny);
  fprintf(fp, "%s\n", machinetype);
  fprintf(fp, "%s\n", datatype);
  fclose(fp);

  /* done */

  return (0);

}

/*==============================================================*/

int
rd_mat(char * fn, int nx, int ny, float * mat, char * type)
{

  /* declarations */

  int             i, j, st;
  FILE           *fp;
  float           val;
  int            *imat;
  unsigned short int *usmat;
  unsigned int   *uimat;
  char           *p;

  /* open file */

  if ((fp = fopen(fn, "r")) == NULL)
  {
    printf("rd_mat: could not open <%s>\n", fn);
    return(1);
  } else
    printf("rd_mat: <%s> open...", fn);
  fflush(stdout);

  if ((p = strstr(type, "ascii")) != NULL)
  {
    /* read and transfer */

    st = fscanf(fp, "%i %i %f\n", &i, &j, &val);
    while (st == 3)
    {
      if (j >= 0 && i >= 0 && j < ny && i < nx)
	MAT(mat,j,i) = val;
      st = fscanf(fp, "\n%i %i %f", &i, &j, &val);
    }
    fclose(fp);
    printf(" and read\n");
    fflush(stdout);

  } else if ((p = strstr(type, "int")) != NULL)
  {
    /* read and transfer */

    imat = (int *) malloc(nx * sizeof(int));
    printf("allocated %i bytes of temp read space\n", nx * sizeof(int));
    fflush(stdout);

    /* read and transfer */

    for (j = 0; j < nx; j++)
    {
      st = fread(imat, nx * sizeof(int), 1, fp);
      for (i = 0; i < nx; i++)
	MAT(mat,i,j) = *(imat + i);
    };

    fclose(fp);
    printf(" and read\n");
    fflush(stdout);
    free(imat);

  } else if ((p = strstr(type, "us")) != NULL)
  {
    /* read and transfer */

    usmat = (unsigned short int *) malloc(nx * sizeof(unsigned short int));

    /* read and transfer */

    for (j = 0; j < nx; j++)
    {
      st = fread(usmat, nx * sizeof(unsigned short int), 1, fp);
      for (i = 0; i < nx; i++)
	MAT(mat,i,j) = *(usmat + i);
    };

    fclose(fp);
    printf(" and read\n");
    fflush(stdout);
    free(usmat);

  }else if ((p = strstr(type, "ui")) != NULL)
  {
    /* read and transfer */

    uimat = (unsigned int *) malloc(nx * sizeof(unsigned int));

    /* read and transfer */

    for (j = 0; j < nx; j++)
    {
      st = fread(uimat, nx * sizeof(unsigned int), 1, fp);
      for (i = 0; i < nx; i++)
	MAT(mat,i,j) = *(uimat + i);
    };

    fclose(fp);
    printf(" and read\n");
    fflush(stdout);
    free(uimat);

  } else if ((p = strstr(type, "float")) != NULL)
  {

    /* read */

    st = fread(mat, nx * ny * sizeof(float), 1, fp);

    fclose(fp);
    printf(" and read\n");
    fflush(stdout);

  } else
  {
    printf("2d_fun: don't know how to read data of type <%s> \n", type);
    exit(0);
  }


  /* done */

  return (0);

}

/*==============================================================*/

int
wr_mat(char * fn, int nx, int ny, float * mat, char * type)
{

  /* declarations */

  int             i, j, st;
  FILE           *fp;
  float           val;
  int            *imat;
  unsigned short int *usmat;
  unsigned int *uimat;
  char           *p;

  /* open file */

  if ((fp = fopen(fn, "w")) == NULL)
  {
    printf("wr_mat: could not open <%s>\n", fn);
    exit(0);
  };

  /* write data acording to type */

  if ((p = strstr(type, "ascii")) != NULL)
  {

    for (i = 0; i < nx; i++)
      for (j = 0; j < ny; j++)
      {
	val = MAT(mat,j,i) ;
	if (val != 0)
	  fprintf(fp, "%i %i %f\n", i, j, val);
      };

  } else if ((p = strstr(type, "int")) != NULL)
  {

    imat = (int *) malloc(nx * sizeof(int));

    for (j = 0; j < nx; j++)
    {
      for (i = 1; i < nx; i++)
	*(imat + i) = (int) MAT(mat,i,j);
      st = fwrite(imat, nx * sizeof(int), 1, fp);
    };

    free(imat);

  } else if ((p = strstr(type, "us")) != NULL)
  {

    usmat = (unsigned short int *) malloc(nx * sizeof(unsigned short int));

    for (j = 0; j < nx; j++)
    { 
      for (i = 1; i < nx; i++)
	*(usmat + i) = (unsigned short int) MAT(mat,i,j);
      st = fwrite(usmat, nx * sizeof(unsigned short int), 1, fp);
    };

    free(usmat);

  }else if ((p = strstr(type, "ui")) != NULL)
  {

    uimat = (unsigned int *) malloc(nx * sizeof(unsigned int));

    for (j = 0; j < nx; j++)
    { 
      for (i = 1; i < nx; i++)
	*(uimat + i) = (unsigned int) MAT(mat,i,j);
      st = fwrite(uimat, nx * sizeof(unsigned int), 1, fp);
    };

    free(uimat);

  } else if ((p = strstr(type, "float")) != NULL)
  {
    st = fwrite(mat, nx * ny * sizeof(float), 1, fp);

  } else
  {
    printf("wr_mat: don't know how to write data of type <%s> \n", type);
    exit(0);
  }

  /* close output file */

  st = fclose(fp);

  /* done */

  return (0);

}


/*=====================================================*/

int
mat_mc(float * mat, int nx, int ny, float mc, float off, int way)
{
  /* multiply matrix */

  /* decalarations */

  int             i, j, k, i1, i2;
  float          *mt, lo, hi, val, r1;

  /* temp matrix */

  k = nx * ny * sizeof(float);
  mt = (float *) malloc(k);

  /* transfer */

  k = nx * ny;
  for (i = 0; i < k; i++)
  {
    *(mt + i) = *(mat + i);
    *(mat + i) = 0;
  };

  /* modify */

  if (way == 1)
  {

    printf("multiply matrix by factor of %f in y direction \n", mc);

    /* multiply in y direction */

    for (i = 0; i < nx; i++)
      for (j = 0; j < ny; j++)
      {

	val = *(mt + i * nx + j);

	if (val != 0.0)
	{

          /*printf("processing (%i,%i)-->%f\n",i,j,val);*/

	  /* find the actual channel coverage */

	  lo = j - 0.5;
	  hi = j + 0.5;

	  /* find new channel coverage */

	  lo = mc * lo + off;
	  hi = mc * hi + off;

	  /* evaluate limits */

	  i1 = (int) (lo + 0.5);
	  i2 = (int) (hi + 0.499999999999);

	  if (i2 > i1)
	  {

	    i1++;
	    i2--;

	    val /= mc;

	    for (k = i1; k <= i2; k++)
	      if (k < ny)
		*(mat + i * nx + k) += val;

	    /* then the snips at the ends */

	    r1 = (i1 - 0.5 - lo) * val;
	    *(mat + i * nx + i1 - 1) += r1;
	    r1 = (hi - (i2 + 0.5)) * val;
	    *(mat + i * nx + i2 + 1) += r1;

	  } else
	  {

	    *(mat + i * nx + i1) += val;

	  };

	};

      }

  } else if (way == 0)
  {

    printf("multiply matrix by factor of %f in x direction \n", mc);

    /* multiply in x direction */

    for (j = 0; j < (ny - 1); j++)
      for (i = 0; i < (nx - 1); i++)
      {

	val = *(mt + i * nx + j);

	if (val != 0)
	{

	  /* find the actual channel coverage */

	  lo = i - 0.5;
	  hi = i + 0.5;

	  /* find new channel coverage */

	  lo = mc * lo + off;
	  hi = mc * hi + off;

	  /* evaluate limits */

	  i1 = (int) (lo + 0.5);
	  i2 = (int) (hi + 0.499999999999);

	  if (i2 > i1)
	  {

	    i1++;
	    i2--;

	    val /= mc;

	    for (k = i1; k <= i2; k++)
	      if (k < ny)
		*(mat + k * nx + j) += val;

	    /* then the snips at the ends */

	    r1 = (i1 - 0.5 - lo) * val;
	    *(mat + (i1-1) * nx + j) += r1;
	    r1 = (hi - (i2 + 0.5)) * val;
	    *(mat + (i2+1) * nx + j) += r1;

	  } else
	  {

	    *(mat + i1 * nx + j) += val;

	  };

	};
      };

  };

  /* done */

  free(mt);
  fflush(stdout);
  return (0);

}

/*=====================================================*/

int
sm_mat(float *mat, int nx, int ny)
{
  /* 2D smooth function */

  /* declarations */

  int             i, j, k, l, nn, ii, jj;
  float          *mt;

  /* temp matrix */

  k = nx * ny * sizeof(float);
  mt = (float *) malloc(k);

  /* transfer */

  k = nx * ny;
  for (i = 0; i < k; i++)
  {
    *(mt + i) = *(mat + i);
    *(mat + i) = 0;
  };
  
#if(0)  
  /* debug print */
  
  for (i=0;i<nx;i++)
    for (j=0;j<ny;j++)
      printf("%2i,%2i: %f --> %f\n",i,j,*(mt + i * nx + j),*(mat + i * nx + j));  
#endif
  

  /* smooth with nearest neighbors */

  for (i = 1; i < nx; i++)
    for (j = 1; j < ny; j++)
    {
    
    /* nearest neighbors */
    
#if(0)        
    printf("i,j=%i,%i\n",i,j);
#endif    
    nn=0;
    for (k=-1;k<=1;k++)
      for (l=-1;l<=1; l++)
        if (!(k==0 && l==0))
        {
        ii=i+k;
        jj=j+l;
#if(0)        
        printf("ii,jj=%i,%i\n",ii,jj);
#endif   
        if( ii>=0 && ii< nx)
          if(jj>=0 && jj< ny)
            {
            MAT(mat,i,j)+=MAT(mt,ii,jj);
            nn++;
#if(0)        
            if (MAT(mt,ii,jj)>0)
              {
              printf("MAT(mt,%i,%i)=%f; ",ii,jj,MAT(mt,ii,jj));
              printf("MAT(mat,%i,%i)=%f\n",i,j,MAT(mat,i,j));
              };
#endif   
            };
        };
        
    /* add original data point */    
        
    if (nn>0)
      MAT(mat,i,j)+=MAT(mt,i,j)*2*nn;
    else
      MAT(mat,i,j)+=MAT(mt,i,j);
         
    /* normalize */
    
    if (nn>0)
      MAT(mat,i,j) /= (3*nn);
    else
      MAT(mat,i,j) =0;
      
    if ( std::isnan((double)MAT(mat,i,j)) )
      {
      printf("%i, %i -> %f\n", i,j,MAT(mat,i,j));
      MAT(mat,i,j)=0;
      };  
    
    
#if(0)    
    printf("%2i,%2i: %f --> %f\n",i,j,MAT(mat,i,j),MAT(mt,i,j));
#endif    
    
    };

  /* done */

  free(mt);
  fflush(stdout);
  return (0);

}

