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
}
light_source {
  <0, 10, 20>
  color rgb <1, 1, 1>
}      

//surface
box { <-10.00, 0.00, -10.00>,< 10.00, -2.00, 10.00>   

      texture { pigment{ color rgbf <.9,.9,.9,.5>}  
                finish { phong 1 reflection{ .500 metallic 1.00} } 
              } // end of texture

      scale <1,1,1> rotate<0,0,0> translate<0,0,0> 
} // end of box --------------------------------------


#macro Head() 
    sphere {
        <0, 0, 0>, 1
        scale <2.5, 2.25, 1.5> 
        translate <3.5, 5.5, 2.5>
    }
#end

#macro eyeL()
    intersection {
        cylinder {
            <2.5, 0, 2.5>, <2.5, 0, 4>, .5
            scale <1, 2, 1>
            translate <0, 5.5, 0>
        }
        box {
            <-5, 5.5, -5>
            <5, 4, 5>    
        }
    }
#end       
         
#macro eyeR()
intersection {
    cylinder {
        <4.5, 0, 2.5>, <4.5, 0, 4>, .5
        scale <1, 2, 1>     
        translate <0, 5.5, 0>
    }
    box {
        <-5, 5.5, -5>
        <5, 4, 5>  
    }
}
#end    

#macro highlight()
    union {
        cylinder {
            <2.1, 5.4, 2.5>, <2.1, 5.4, 4>, .14
        }
        cylinder {
            <4.1, 5.4, 2.5>, <4.1, 5.4, 4>, .14
        }
    }
#end   


#macro face()
    union {
        box {
            <3.375, 4.115, 2.5>, <3.625, 4.135, 4>
        }
        box {
            <2.5, 5.76, 2.5>, <3, 5.74, 4>
        }
        box {
            <4, 5.76, 2.5>, <4.5, 5.74, 4>
        }
        box {
            <2, 6.26, 2.5>, <3, 6.24, 4>
        }
        box {
            <4, 6.26, 2.5>, <5, 6.24, 4>
        }
    }
#end
          
         
//head
difference {
    Head() 
    eyeL()
    eyeR()
    face()     
    texture {
        pigment { color rgb <1, 219/255, 172/255> }//
    }
}

intersection {
    face()
    Head()
}
difference {
    intersection {
        eyeR()
        Head()   
        texture {
            pigment { color rgbf <.6, .1, 0, .5> }//
        }
    }
    highlight()
}
difference {
    intersection {
        eyeL()
        Head()   
        texture {
            pigment { color rgbf <.6, .1, 0, .3> }//
        }
    }
    highlight()    
}
intersection {
    highlight()
    Head()      
    union { 
        eyeL()
        eyeR()
    }
    texture {
        pigment { color rgbf <1,1,1, .3> }//
    }
}

//hair
#macro hair()
    difference {
        union {
            sphere {
                <0, 0, 0>, 1                      
                scale <2.6, 2.35, 1.6> 
                translate <3.5, 5.5, 2.5>
            }
            cylinder {
                <0, 0, 0>,<0,-3>, 1                  
                scale <2.6, 1, 1.6> 
                translate <3.5, 5.5, 2.5>
            }
        }
        union {
            sphere {
                <0, 0, 0>, 1                   
                scale <2.51, 2.26, 1.51> 
                translate <3.5, 5.5, 2.5>
            }
            cylinder {
                <0, 0, 0>,<0,-3>, 1           
                scale <2.51, 1.01, 1.51> 
                translate <3.5, 5.5, 2.5>
            }
        }
    }
#end      

#macro backHair()
     union { 
        intersection {
            sphere {
                <0,2.25,1>,4.5
            }
            sphere {
                <8,2.25,1>,4.5
            }
        }
        intersection {
            sphere {
                <-2,2.45,1>,4.5
            }
            sphere {
                <6,2.45,1>,4.5
            }
        }/*  
        intersection {
            sphere {
                <-.5,2.5,1>,4
            }
            sphere {
                <6.1,2.5,1>,4
            }
        }*/         
        intersection {
            sphere {
                <-1,3,3.55>,3.5
            }
            sphere {
                <4,3,0>,3.5
            }
        }
        intersection {
            sphere {
                <5,2.5,2>,1.8
            }
            sphere {
                <8,2.5,3>,3.8
            }
        }
        intersection {
            difference {
                sphere {
                    <6,4,1>,3
                }
                sphere {
                    <6,8,0>,4
                }
            }
            sphere {
                <6,4,3>,1
            }
        }
        intersection {
            sphere {
                <3,3,1>,3
            }
            sphere {
                <-1,3,5>,4
            }
        }
        translate <0,-.75,0>      
    }
#end

difference {
    hair()  /*
    box {
        <0, 6.5, 3.5>
        <7, 2, 5>  
    }*/             
    backHair()
    difference {
        sphere {
            <3.5,5.5,4>,1.5
        }
        sphere {
            <2,6.5,4>,2.1
        }
    }
    difference {
        sphere {
            <5,5,3.5>,3.1
        }
        sphere {
            <5,7.5,3.5>,2.8
        }
        box {
            <4,7,5>
            <8,0,0>
        }
        box {
            <0,0,0>
            <10,10,2.5>
        }
    }
    intersection {
        sphere {
            <5.75, 5.5-.2-.2-.2, 3.2>,1.1
        }
        sphere {
            <4.75,5.5,4.2>,1.1
        }
    }
    intersection {
        sphere {
            <6.2, 5.2-.2-.2-.2, 2.8>,1
        }
        sphere {
            <5.2,5,3.8>,1
        }
    }
    intersection {
        sphere {
            <3.5*2-5.75, 5.2-.2-.2-.2-.2, 3.2>,1
        }
        sphere {
            <3.5*2-4.75,5.2-.2,4.2>,1
        }
    }
    intersection {
        sphere {
            <3.5*2-6.2, 4.8-.2-.2-.2-.2, 2.8>,1
        }
        sphere {
            <3.5*2-5.2,4.6-.2,3.8>,1
        }
    }
    difference {
        union { 
            sphere {
                <3.5,0,4.5>,5
            }
            sphere {
                <5,4.5,3.8>,.5
            }
        }
        box {
            <-1,-1,2.5>,<10,10,-10>
        }
        /*
        sphere {
            <3.5,10,-3>,7.5    
            translate <0,.5,0> 
        }*/                 
    }
    sphere {
        <5,3,4>,2
    }
    texture {
        pigment { color rgb <89/255, 61/255, 38/255> }//
    }
}

#macro band()
    difference {
        intersection {
            sphere {
                <3.5,8.5,2.5>,3.5
            }
            box {
                <-1,6,2.45>,<8,12,2.55>
            }
        }
        intersection {
            sphere {
                <6,15,2.5>,6.5
            }
            sphere {
                <1,15,2.5>,6.5
            }
        }
        intersection {
            sphere {
                <5,2,2.5>,6.5
            }
            sphere {
                <2,2,2.5>,6.5
            }
        }
    }
#end
       
union {
    band()
    cylinder {
        <3,8.7,2.5>
        <4,8.7,2.5>
        .5
    }
    translate <0,-.5,0>
    texture {
        pigment { color rgbf <.6, .1, 0, .5> }//
    }
}
        
//strands        
sphere_sweep {
    b_spline
    4,                      
    <2,8,.8>, 0.1,
    <2,7,3.85>, 0.12,
    <2,4,3.85>, 0.12,
    <2,-5,3.65>, 0.12
    tolerance 0.9                                            
    texture {
        pigment { color rgb <89/255, 61/255, 38/255> }//
    }
}
sphere_sweep {
    b_spline
    4,                      
    <5,8,.8>, 0.1,
    <5,7,3.85>, 0.12,
    <5,4,3.85>, 0.12,
    <5,-5,3.65>, 0.12
    tolerance 0.9                                            
    texture {
        pigment { color rgb <89/255, 61/255, 38/255> }//
    }
}
cylinder {
    <5,5,3.85>
    <5,4,3.85>
    .135       
    texture {
        pigment { color rgb <.6, .1, 0> }//
    }
}
cylinder {
    <2,5,3.85>
    <2,4,3.85>
    .135       
    texture {
        pigment { color rgb <.6, .1, 0> }//
    }
}
                 
//body
union {
    cone {
        <3.5,4.5,2.5>,.5
        <3.5,0.3,2.5>,1     
    }
    sphere {
        <0,0,0>,1
        scale <1,.3,1>
        translate <3.5,0.3,2.5>
    }
    sphere_sweep {
        linear_spline
        2,                      
        <1.4,2.8,2.5>, 0.35,
        <5.6,2.8,2.5>, 0.35
        tolerance 0.9   
    }
    cylinder {
        <3,.5,3.1>
        <2.5,.5,4.5>
        .5
    }
    sphere {
        <3,.5,3.1>
        .5
    }
    cylinder {
        <4,.5,3.1>
        <4.5,.5,4.5>
        .5
    }
    sphere {
        <4,.5,3.1>
        .5
    }
    texture {
        pigment { color rgb <1, 219/255, 172/255> }//
    }
}
sphere {
    <2.5,.5,4.5>
    .5
    texture {
        pigment { color rgb .1 }//
    }
}
sphere {
    <4.5,.5,4.5>
    .5
    texture {
        pigment { color rgb .1 }//
    }
}

//dress
cone {
    <3.5,2.1,2.5>,.8
    <3.5,.5,2.5>,2
    open
    texture {
        pigment { color rgb <.6, .1, 0> }//
    }
}    
cone {
    <3.5,2.09,2.5>,.79  
    <3.5,3.1,2.5>,.679  
    texture {
        pigment { color rgb <.6, .1, 0> }//
    }  
}    
  
       // <1.4,2.8,2.5>, 0.35,
       // <5.6,2.8,2.5>, 0.35
//sleeve   
union {
    cone {
        <2.6,2.8,2.5>,.4
        <1.4,2.4,2.5>,.8
        open
        texture {
            pigment { color rgb .9 }//
        }
    }
    cone {
        <4.4,2.8,2.5>,.4
        <5.6,2.4,2.5>,.8
        open
        texture {
            pigment { color rgb .9 }//
        }
    }
    scale <1,1,.7>    
    translate<0,0,.7*2.5/2>     
}

//tie   
cylinder {
    <3.5,4,2.5>
    <3.5,3.1,2.5>
    .68
    open 
    texture {
        pigment { color rgb .9 }//
    }
} 
difference {
    cylinder {
        <3.5,4-.55,2.5>
        <3.5,3.8-.7,2.5>
        .8
        open
    }
    intersection {
        sphere {
            <2.5,3,3>,1.25
        }
        sphere {
            <4.5,3,3>,1.25
        }
    }
    texture {
        pigment { color rgb .9 }//
    }
}

// linear prism in x-direction: from ... to ..., number of points (first = last)
prism { -.25 ,.25 , 5
        <-1, 3.5>, // first point
        < 1, 3.5>, 
        < 2, -3.5>,
        < -2, -3.5>,
        <-1, 3.5>  // last point = first point!!!
        rotate<-100,0,0> //turns prism in x direction! Don't change this line!  

      texture { pigment{ color Yellow } } // end of texture

       scale .15
       translate<3.5, 2.9, 3.3> 
} // end of prism --------------------------------------------------------   
                     
