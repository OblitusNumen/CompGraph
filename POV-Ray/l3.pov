#include "colors.inc"
#include "textures.inc"

background {
  color rgb <0.5, 0.7, 1>
}
camera {                  
    location <3.5, 6, 20> 
    location <20, 6, 20> 
    location <-20, 6, -20>  
    //look_at <3.5, 2.9, 3.3>
    look_at <3.5, 5.5, 2.5>   
    location <20,5,-5>
    look_at <0,1,4>  
    //location <6,20,4>
    //look_at <6,1.5,4>
}
light_source {
  <0, 10, 20>
  color rgb <1, 1, 1>
}         
//surface
box { <-10.00, 0.00, -10.00>,< 20.00, -2.00, 20.00>   

      texture { pigment{ color rgbf <.9,.9,.9,.5>}  
                finish { phong 1 reflection{ .500 metallic 1.00} } 
              } // end of texture

      scale <1,1,1> rotate<0,0,0> translate<0,0,0> 
} // end of box --------------------------------------                                                                  
                                                     
#macro bottle1(r,g,b,l)     
    object {                     
        isosurface{ // ------------------------------------------------------------
        
          function {  -(z* z)+pow(z+.1, 4)+(x*x+y*y-.03)
                   }
        
          threshold 0
          accuracy 0.0001
          max_gradient 5
          contained_by {box {<-2,-2,-2>, < 2, 2, l>}}
          //open
        
          texture{ pigment{ color rgbf <r,g,b,.9>}
                   finish { phong .7}
        	  }
          scale 2.00
          rotate <-90,0,0>
          translate <0, 0, 0>   
        } // end of isosurface -----------------------------------------------------   
        scale .99
    }
                                                         
                         
    isosurface{ // ------------------------------------------------------------
    
      function {  -(z* z)+pow(z+.1, 4)+(x*x+y*y-.03)
               }
    
      threshold 0
      accuracy 0.0001
      max_gradient 5
      contained_by {box {<-2,-2,-2>, < 2, 2, .15>}}
      //open
    
      texture{ pigment{ color rgbf .9}
               finish { phong 1}
    	  }
      scale 2.00
      rotate <-90,0,0>
      translate <0, 0, 0>
    } // end of isosurface ----------------------------------------------------- 
#end       

#macro bottle2(r,g,b,l)    
    difference {
        sphere_sweep{  // rotates a 2-D outline of points around the Y axis to create a 3-D shape
          linear_spline //  linear_spline | quadratic_spline | cubic_spline
          2,      // number of points,
          <0.0, 1.0, >, .1,
          <0.0, -1.0, >, .1    
          rotate<0,0,0> 
          translate<0,0.0,0>
        } // ----------------------------------------------- end of lathe object
        box {
            <1,1,1>, <-1,2,-1>
        }    
        texture{ pigment{ color rgbf .9}
            finish { phong 1}
    	}
    }
    difference {
        sphere_sweep{  // rotates a 2-D outline of points around the Y axis to create a 3-D shape
          linear_spline //  linear_spline | quadratic_spline | cubic_spline
          2,      // number of points,
          <0.0, 1.0, >, .1,
          <0.0, -1.0, >, .1    
          rotate<0,0,0> 
          translate<0,0.0,0>
        } // ----------------------------------------------- end of lathe object
        box {
            <1,-1+l*2,1>, <-1,2,-1>
        }
        scale .99  
        texture{ pigment{ color rgbf <r,g,b,.9>}
            finish { phong 3}
        }
    }
#end
                         
#declare petrih=.15;
#declare petrir=.8;
#macro petri(r,g,b,l)
    difference {
        cylinder {
            <0,0,0>,<0,petrih,0>,petrir
        }
        cylinder {
            <0,.01,0>,<0,petrih*2,0>,petrir-.02
        }
        texture{ pigment{ color rgbf .9}
            finish { phong 1}
    	}
    }
    difference {
        cylinder {
            <0,.02,0>,<0,petrih*l,0>,petrir-.02
        }
        texture{ pigment{ color rgbf <r,g,b,.9>}
            finish { phong .5}
        }
    }
#end

                     
//bottle1(1,.1,.1,-1)  
//bottle2(.5,.5,.8,.3)

#declare col=<89/255, 61/255, 38/255>;
#declare cols=array[10][3]{
    {.7,.2,.2},
    {.3,.3,.5},
    {.6,.6,.6},
    {.5,.7,.6},
    {.9,.6,.6},
    {.7, .9, .4},
    {.2, .6, .9},
    {.7, .8, .2},
    {.5, .1, .8},
    {.7, 0,.7}
}
#declare full=array[10]{
    .6, .3, .9, .75, .4, .1, .8, .4, .3, .7
}

difference {
    union {
        box {
            <1,3,1>
            <4,2.9,7>
        }
        box {
            <1.25,1,1.25>
            <3.75,.9,6.75>
        }
        cylinder {
            <1.25,0, 1.25>, <1.25,3,1.25>, .25
        }
        cylinder {
            <1.25,0, 6.75>, <1.25,3,6.75>, .25
        }
        cylinder {
            <3.75,0, 1.25>, <3.75,3,1.25>, .25
        }
        cylinder {
            <3.75,0, 6.75>, <3.75,3,6.75>, .25
        }
    }        
    #for (i, 2, 6, 1)
    object {
        union { bottle2(.5,.5,.8,.3) }
        scale 1.5
        translate <2,2.65,i>
    }
    #end
    #for (i, 2, 6, 1)
    object {
        union { bottle2(.5,.5,.8,.3) }
        scale 1.5
        translate <3,2.65,i>
    }
    #end                    
    texture { pigment{ color rgb col } }
}
//счетчик, начальное значение, конечное, шаг    
#for (i, 2, 6, 1)
object {
    union { bottle2(cols[i*2-4][0],cols[i*2-4][1],cols[i*2-4][2],full[i*2-4]) }
    scale 1.5
    translate <2,2.65,i>
}
#end
#for (i, 2, 6, 1)
object {
    union { bottle2(cols[i*2-3][0],cols[i*2-3][1],cols[i*2-3][2],full[i*2-3]) }
    scale 1.5
    translate <3,2.65,i>
}
#end
       
object {
    union { bottle1(0,1,1,-1) }
    translate <5,2.4,8.5>
}  
       
object {
    union { bottle1(0.9,1,0.1,-1) }
    translate <7.5,2.4,6.5>
}
       
object {
    union { bottle1(0.9,.5,0.1,-1) }
    translate <7,2.4,0.5>
}

object {
    union { petri(1,1,0,.5) }
    translate <9.5,0,3.5>
}

#macro bottle3(r,g,b,l)
    union {
        difference {
            sphere {
                0,1.5
            }
            sphere {
                0,1.5
                scale .99
            }
            cylinder {
                0,2,.4
            }
            cylinder {
                0,<-2,2,-2>,.4
            }
            cylinder {
                0,<0, 5, 0>,.4
            }    
            texture{ pigment{ color rgbf .9}
                finish { phong 1}
    	    }
        }
        difference {     
            cylinder {
                0,2,.4
            }
            cylinder {
                0,3,.35
            }     
            sphere {
                0,1.5
            }   
            texture{ pigment{ color rgbf .9}
                finish { phong 1}
    	    }
        }
        difference {      
            cylinder {
                0,<-2,2,-2>,.4
            }
            cylinder {
                0,<-3,3,-3>,.35
            }
            sphere {
                0,1.5
            }  
            texture{ pigment{ color rgbf .9}
                finish { phong 1}
    	    }
        }
        difference {
            cylinder {
                0,<0, 5, 0>,.4
            }
            cylinder {
                0,<0, 5, 0>,.4
                scale <.99, 1.1, .99>
            }     
            sphere {
                0,1.5
            }  
            texture{ pigment{ color rgbf .9}
                finish { phong 1}
    	    }
        }
	}     
    difference {
        sphere {
            0,1.5
            scale .99
        }
        box {
            <-2,-1.5+3*l,-2>,
            <2,2,2>
        }
        texture{ pigment{ color rgbf <r,g,b,.9>}
            finish { phong 2}
        }
    }
#end

object {
    union { bottle3(.0,.95,0,.25) }
    translate <6,1.5,4>
}         

isosurface{ // ------------------------------------------------------------

  function { .5*pow(sin(x*2),2)* x* y + .5*pow(cos(2* y),2)* y* z-5+x* x+y* y+z* z
           }

  threshold 0
  accuracy 0.0001
  max_gradient 5
  contained_by {box {-3, 3}}
  //open
             
  texture { pigment{ color rgb<1,0.75,0.55>}
            normal { bumps 0.5 scale 0.05}
            finish { phong 0.3}
	  }
  scale .2
  rotate <0,0,0>
  translate <7,.6,0.5>   
} // end of isosurface ----------------------------------------------------- 










