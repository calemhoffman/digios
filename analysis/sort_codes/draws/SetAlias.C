 {
  
   //Det _22
   psd_tree->SetAlias("xncal_22","xn[22]*1.01974");
   psd_tree->SetAlias("ecalx_22","e[22]*0.907486");
   psd_tree->SetAlias("ecal_22","e[22]*1.0+0.0");
   psd_tree->SetAlias("x1_22","xf[22]/ecalx_22");
   psd_tree->SetAlias("z1_22","(x1_22-0.75)*5.0-29.25");
   psd_tree->SetAlias("ecal1_22","e[22]*0.0037436");
   psd_tree->SetAlias("x2_22","1-xncal_22/ecalx_22");
   psd_tree->SetAlias("z2_22","(x2_22-0.25)*5.0-31.75");
   psd_tree->SetAlias("ecal2_22","e[22]*0.0037554");
   psd_tree->SetAlias("x_22","(xf[22]-xn[22])/(xf[22]+xn[22])");
   psd_tree->SetAlias("xe1_22","(ecal_22-(x1_22*x1_22*x1_22*x1_22*(-2421.59)+x1_22*x1_22*x1_22*8121.18+x1_22*x1_22*(-9878.51)+x1_22*5386.25-257.44))");//
   psd_tree->SetAlias("ex1_22","(xe1_22*(-0.0035193)+7.25776)"); // 
   psd_tree->SetAlias("xe2_22","(ecal_22-(x2_22*x2_22*x2_22*x2_22*(-4202.25)+x2_22*x2_22*x2_22*4619.85+x2_22*x2_22*(-1754.1)+x2_22*484.873+722.953))");//
   psd_tree->SetAlias("ex2_22","(xe2_22*(-0.00354301)+7.29875)"); //     
   psd_tree->SetAlias("tac1_22","(e_t[22]-rdt_t[1])");
   psd_tree->SetAlias("t_22","(t5[0]-(x_22*x_22*x_22*(98.845)+x_22*x_22*(441.423)+x_22*(-122.63)+(2854.75)))");
    psd_tree->SetAlias("tt_22","(t_22-(e[22]*e[22]*e[22]*e[22]*(-0.000000000807765)+e[22]*e[22]*e[22]*(0.0000044382)+e[22]*e[22]*(-0.00858873)+e[22]*(7.01729)+(-2062.26)))");
   psd_tree->SetAlias("ttt_22","tt_22-(ex1_22*ex1_22*ex1_22*(-0.417017)+ex1_22*ex1_22*(-5.32569)+ex1_22*84.4605-85.8557)");

   //Det _21
   psd_tree->SetAlias("xncal_21","xn[21]*0.987393");
   psd_tree->SetAlias("ecalx_21","e[21]*1.11636");
   psd_tree->SetAlias("ecal_21","e[21]*1.0+0.0");
   psd_tree->SetAlias("x1_21","xf[21]/ecalx_21");
   psd_tree->SetAlias("z1_21","(x1_21-0.75)*5-35.15");
   psd_tree->SetAlias("ecal1_21","e[21]*0.0043");
   psd_tree->SetAlias("x2_21","1-xncal_21/ecalx_21");
   psd_tree->SetAlias("z2_21","(x2_21-0.25)*5-37.65");
   psd_tree->SetAlias("ecal2_21","e[21]*0.0043");
   psd_tree->SetAlias("x_21","(xf[21]-xn[21])/(xf[21]+xn[21])");
   psd_tree->SetAlias("xe1_21","(ecal_21-(x1_21*223.942+355.609))");//
   psd_tree->SetAlias("ex1_21","(xe1_21*(-0.00425644)+7.42905)"); // 
   psd_tree->SetAlias("xe2_21","(ecal_21-(x2_21*272.162+445.235))");//
   psd_tree->SetAlias("ex2_21","(xe2_21*(-0.00424458)+7.04477)"); // 
   psd_tree->SetAlias("tac1_21","(e_t[21]-rdt_t[1])");
   psd_tree->SetAlias("t_21","(t5[0]-(x_21*x_21*x_21*x_21*x_21*x_21*(-2373.91)+x_21*x_21*x_21*x_21*x_21*(91.0502)+x_21*x_21*x_21*x_21*(2960.39)+x_21*x_21*x_21*(490.17)+x_21*x_21*(-540.07)+x_21*(-550.42)+2873.8))");
   psd_tree->SetAlias("tt_21","(t_21-(e[20]*e[20]*e[20]*(0.000000532617)+e[20]*e[20]*(-0.00183226)+e[20]*(2.19483)+(-732.702)))");//had wrong e[20] thats why!!
   psd_tree->SetAlias("t_cal_21","tt_21-(ex1_21*ex1_21*ex1_21*(0.125792)+ex1_21*ex1_21*(-11.3004)+ex1_21*(45.1889)+948.22)");
      
   //Det _20
   psd_tree->SetAlias("xncal_20","xn[20]*0.945164");
   psd_tree->SetAlias("ecalx_20","e[20]*0.975706");
   psd_tree->SetAlias("ecal_20","e[20]*1.0+0.0");
   psd_tree->SetAlias("x1_20","xf[20]/ecalx_20");
   psd_tree->SetAlias("z1_20","(x1_20-0.75)*5-41.05");
   psd_tree->SetAlias("ecal1_20","e[20]*0.003970");
   psd_tree->SetAlias("x2_20","1-xncal_20/ecalx_20");
   psd_tree->SetAlias("z2_20","(x2_20-0.25)*5-43.55");
   psd_tree->SetAlias("ecal2_20","e[20]*0.0039595");
   psd_tree->SetAlias("x_20","(xf[20]-xn[20])/(xf[20]+xn[20])");
   psd_tree->SetAlias("xe2_20","(ecal_20-(x2_20*196.431+947.905))");//
   psd_tree->SetAlias("ex2_20","(xe2_20*(-0.00395492)+4.377866)"); // 
   psd_tree->SetAlias("xe1_20","(ecal_20-(x1_20*316.642+371.746))");//
   psd_tree->SetAlias("ex1_20","(xe1_20*(-0.00390155)+6.2854)"); //
   psd_tree->SetAlias("tac1_20","(e_t[20]-rdt_t[1])");
   psd_tree->SetAlias("t_20","(t5[0]-(x_20*x_20*x_20*x_20*(21.8255)+x_20*x_20*x_20*(-68.7306)+x_20*x_20*(129.073)+x_20*(-162.802)+3001.83))");
   psd_tree->SetAlias("tt_20","(t_20-(e[20]*e[20]*e[20]*(0.00000029125)+e[20]*e[20]*(-0.00148629)+e[20]*(2.52475)+(-1361.22)))");
   psd_tree->SetAlias("t_cal_20","tt_20*(-0.07127)+24.33");
   
   //Det _19
   psd_tree->SetAlias("xncal_19","xn[19]*0.975596");
   psd_tree->SetAlias("ecalx_19","e[19]*1.03803");
   psd_tree->SetAlias("ecal_19","e[19]*1.0+0.0");
   psd_tree->SetAlias("x1_19","xf[19]/ecalx_19");
   psd_tree->SetAlias("z1_19","(x1_19-0.75)*5-46.95");
   psd_tree->SetAlias("ecal1_19","e[19]*0.00394");
   psd_tree->SetAlias("x2_19","1-xncal_19/ecalx_19");
   psd_tree->SetAlias("z2_19","(x2_19-0.25)*5-49.45");
   psd_tree->SetAlias("ecal2_19","e[19]*0.00394");
   psd_tree->SetAlias("x_19","(xf[19]-xn[19])/(xf[19]+xn[19])");
   psd_tree->SetAlias("xe2_19","(ecal_19-(x2_19*307.303+748.749))");//3.9 MeV
   psd_tree->SetAlias("ex2_19","(xe2_19*(-0.003962)+4.05575)"); //3.9
   psd_tree->SetAlias("xe1_19","(ecal_19-(x1_19*199.836+796.372))");//lin 3.9?
   psd_tree->SetAlias("ex1_19","(xe1_19*(-0.0039471)+3.936)"); //3.9?
   psd_tree->SetAlias("tac1_19","(e_t[19]-rdt_t[1])");
   psd_tree->SetAlias("t_19","(t5[0]-(x_19*x_19*(546.072)+x_19*(-2.28216)+2927.36))");
  psd_tree->SetAlias("tt_19","(t_19-(e[19]*e[19]*e[19]*e[19]*(0.00000000012279)+e[19]*e[19]*e[19]*(-0.000000774114)+e[19]*e[19]*(0.00160425)+e[19]*(-1.16393)+224.394))");
   psd_tree->SetAlias("t_cal_19","tt_19*1.0+0.0");
 
 }
