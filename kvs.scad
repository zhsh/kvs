fn = 0;
fs = 0.4;
eps = 0.001;
w = 1.2;
round_corners = true;

nano_length = 44 + 0.5;
nano_width = 17.7 + 0.5;
nano_height = 1.65 + 0.3;

nano_anthena_length = 15.3 + 0.5;
nano_anthena_width = 10 + 0.5;
nano_anthena_height = 4 + 0.3;

nano_usb_length = 5.6 + 0.5;
nano_usb_width = 7.6 + 0.5;
nano_usb_height = 4.3 + 0.3;

nano_middle_width = 13.7;


battery_holder_length = 23.7 + 0.5;
battery_holder_width = 28 + 0.5;
battery_holder_height_nc = 1.6; // 1.6 sharp without clerance
battery_holder_height = battery_holder_height_nc + 0.2;
battery_d = 20 + 0.5;
battery_and_holder_length = 25.15;

battery_and_holder_height_nc = 5.78;  // 5.78 max without clearance
battery_and_holder_height = battery_and_holder_height_nc + 0.4;

battery_upper_length = 15 + 0.5;
battery_upper_width = 21.3 + 0.5;
battery_only_height_nc = battery_and_holder_height_nc - battery_holder_height_nc;

battery_hole_d = 2.5;
battery_hole_y_offset = (20.7 + battery_hole_d) / 2;
battery_hole_x_offset = (16 + battery_hole_d) / 2;

hole_from_edge_x = battery_holder_length / 2 - battery_hole_x_offset;
hole_from_edge_y = battery_holder_width / 2 - battery_hole_y_offset;

module nano() {
  translate([0,0, nano_height/2])
  cube([nano_length,nano_width,nano_height], center = true);

//  translate([5,0, nano_usb_height/2])
//  cube([nano_length - nano_usb_length - nano_anthena_length - 2,
//       nano_middle_width, nano_usb_height], center = true);

  translate([-nano_length/2 + nano_anthena_length / 2,
      0,
      nano_anthena_height / 2])
    cube([nano_anthena_length,
        nano_anthena_width,
        nano_anthena_height], center = true);

  translate([nano_length/2 - nano_usb_length / 2,
      0,
      nano_usb_height / 2])
    cube([nano_usb_length,
        nano_usb_width,
        nano_usb_height], center = true);

  // contacts
  translate([-nano_length/2 + 6.5, - 1.4 + nano_width/2, 0]) cylinder(d=2, h=4.5, $fn=10);
  translate([ nano_length/2 - 6.0, - 1.4 + nano_width/2, 0]) cylinder(d=2, h=4.5, $fn=10);
}

module battery() {
  difference() {
    union() {
      translate([0,0,battery_holder_height/2]) cube([battery_holder_length,
          battery_holder_width,
          battery_holder_height
      ], center = true);
      target_offset = battery_and_holder_length - battery_holder_length;
      edge_offset = battery_holder_length / 2 - battery_d / 2;
      translate([-edge_offset - target_offset, 0, battery_holder_height]) {
        cylinder(d=battery_d, h=battery_only_height_nc - 0.2, $fn=40);
        translate([0,0, (battery_and_holder_height - battery_holder_height)/2])
          cube([battery_upper_length,
              battery_upper_width,
              battery_only_height_nc], center = true);
      }
    };
    union() {
      translate([-battery_hole_x_offset, -battery_hole_y_offset, -eps])
        cylinder(d = battery_hole_d, h=battery_and_holder_height + eps * 2, $fn=10);
      translate([ battery_hole_x_offset, -battery_hole_y_offset, -eps])
        cylinder(d = battery_hole_d, h=battery_and_holder_height + eps * 2, $fn=10);
      translate([-battery_hole_x_offset,  battery_hole_y_offset, -eps])
        cylinder(d = battery_hole_d, h=battery_and_holder_height + eps * 2, $fn=10);
      translate([ battery_hole_x_offset,  battery_hole_y_offset, -eps])
        cylinder(d = battery_hole_d, h=battery_and_holder_height + eps * 2, $fn=10);
    };
  }
}

button_width = 6.18 + 0.5;
button_height = 3.5 + 0.5;
button_d = 3.4 + 0.5;
button_d_h = 4.9 + 0.5;
button_d_h_real = 4.9;

module button() {
  translate([0,0,button_height/2])
  cube([button_width, button_width, button_height], center = true);
  cylinder(d = button_d, h = button_d_h, $fn = 20);
}

middle_pad = 0.2;

screw_outer_d = 10;
button_offset_x = nano_length - button_width / 2 - screw_outer_d / 2 - hole_from_edge_y;
button_offset_y = button_width / 2;


module components() {
  nano_offset_y = box_width - nano_width;
  nano_offset_z = battery_and_holder_height + middle_pad;

  color("green")
    translate([button_offset_x, button_offset_y, w])
      button();

  color("blue")
    translate([battery_holder_width / 2,
        battery_holder_length / 2,
        battery_and_holder_height + w])
    rotate(-90, [0,0,1])
    rotate(180, [1,0,0])
    battery();

  color("cyan")
    translate([nano_length/2, nano_width/2 + nano_offset_y, w + nano_offset_z])
    rotate(180, [0,0,1])
    nano();
}

box_length = nano_length;
box_width = battery_and_holder_length;
box_height = nano_usb_height + middle_pad + battery_and_holder_height;

// ================================


module origin(x = 0, y = 0, z = 0) {
  translate([-box_length/2 + x, -box_width/2 + y, -box_height/2 + z]) children();
}

module legacy_origin() {
  origin(0, 0, -w) children();
}

module origin_all(q) {
  origin(q, q, q) children();
}

module mirrorX() { mirror([1,0,0]) children(); }
module mirrorY() { mirror([0,1,0]) children(); }
module mirrorZ() { mirror([0,0,1]) children(); }
module dupX() { children(); mirrorX() children(); }
module dupY() { children(); mirrorY() children(); }
module dupZ() { children(); mirrorZ() children(); }
module dupAll() { dupX() dupY() dupZ() children(); }

module cylinders(values, idx = 0) {
  if (idx < len(values)) {
    cylinder(h = values[idx][0] + eps, d1 = values[idx][1], d2 = values[idx][2], $fs=fs, $fa=1, $fn=fn);
    translate([0, 0, values[idx][0]])
      cylinders(values, idx + 1);
  }
}

// ================================

module simple_box_verbose(offx, offy, offz) {
  cube([box_length + 2 * offx, box_width + 2 * offy, box_height + 2 * offz], center = true);
}
module simple_box(off) {
  simple_box_verbose(off, off, off);
}

// Slower variant
module box1(off) {
  round_corner = 3;
  mul = sqrt(2);
  difference() {
    simple_box(off);

    // Subtract boxes from corners
    dupAll()
      origin_all(-off - eps) 
        cube(round_corner + off - eps);
  }

  // Add sphere to corner
  dupAll()
  intersection() {
    origin_all(-off) cube(round_corner + off);
    origin_all(round_corner)
      sphere(r = (round_corner + off) * mul, $fs=fs, $fa=1, $fn=fn);
  }
}

// Fast variant
module box2(off) {
  round_corner = 3;
  mul = sqrt(2);

  cube([box_length - 2 * round_corner + eps, box_width + 2 * off, box_height + 2 * off], center = true);
  cube([box_length + 2 * off, box_width - 2 * round_corner + eps, box_height + 2 * off], center = true);
  cube([box_length + 2 * off, box_width + 2 * off, box_height - 2 * round_corner + eps], center = true);

  // Add sphere to corner
  intersection() {
    simple_box(off);
    dupAll()
      origin_all(round_corner)
      sphere(r = (round_corner + off) * mul, $fs=fs, $fa=1, $fn=fn);
  }
}

module box(off) { 
  if (round_corners) {
    box2(off);
  } else {
    simple_box(off);
  }
}

module bottom_screw_offset(off = 0) {
  origin(hole_from_edge_y, hole_from_edge_x, -off) children();
}

screw_bottom_height = battery_only_height_nc - 0.2;
screw_narrow_height = 1.5;
screw_cap = 5.5; // with clearance

module screw_add() {
  bottom_screw_offset() {
    cylinders([[screw_bottom_height,screw_outer_d,screw_outer_d]]);
  }
}

module screw_sub() {
  cap_height = w + screw_bottom_height - screw_narrow_height;
  bottom_screw_offset(w + eps) {
    cylinders([[cap_height, screw_cap + 1, screw_cap], [screw_narrow_height + 3 * eps, 3.5, 3.5]]);
  }
}

module screw_support() {
  cap_height = w + screw_bottom_height - screw_narrow_height;
  support_z_clearance = 0.35;
  bottom_screw_offset(w + eps) {
    cylinders([[cap_height - support_z_clearance, screw_cap - 0.5, screw_cap - 0.5]]);
  }
}


module screw_top(len) {
  screw_thread = 2.9;
  mirrorZ() bottom_screw_offset() difference() {
    cylinders([[len, 6, 6]]);
    cylinders([[len + 2 * eps, screw_thread-0.2, screw_thread]]);
  }
}

module top_plain() {
  translate([-30,-20,0])
  cube([60,40,20]);
}

// Divisor slope calculation
z0 = box_height / 2 - 1.3 * w;
z1 = -box_height / 2 + battery_only_height_nc / 2;

module plain_transform(off = 0) {
  dz = z0 - z1;
  angle = atan2(dz, box_width);
  move = (z0 + z1) / 2 + off;
  translate([0,0,move])
  rotate(angle, [1,0,0])
  children();
}

module battery_bracket() {
  color("red")
  translate([-w, 0, 0])
    cube([w, hole_from_edge_x, battery_and_holder_height_nc]);
  cube([hole_from_edge_y, hole_from_edge_x, screw_bottom_height]);
}

module cord1() {
  mirrorX() mirrorY()
  origin()
  rotate(45, [0,0,1])
  rotate(90, [0,1,0])
  cylinder(d=2, h=10, center = true, $fs=fs, $fa=1, $fn=fn);
}

module cord() {
  translate([0,-3, 4.7]) cord1();
  translate([-3, 0, 1.7]) cord1();
}

// ziplock triangle
module lock_transform() {
  mirrorX() mirrorY() mirrorZ() origin() children();
}

triangle_sz = 2.6;

pin_length = 3;
pin_triangle_length = 5;
pin_stride = pin_length + pin_triangle_length;
base_clearance = 0.2;
padding_in = 5;
num_pins = floor((box_length - pin_triangle_length - 2 * padding_in) / pin_stride);
padding = (box_length - (pin_stride * num_pins + pin_triangle_length)) / 2;
clearance_y = base_clearance;

module triangle(len) {
  rotate(90, [0,1,0])
    linear_extrude(len)
    polygon([[0,0],[-triangle_sz, 0],[0, triangle_sz]]);
}

module triangle_base() {
  translate([padding, clearance_y, 0]) triangle(box_length - padding * 2);
}

module zipper(clearance) {
  translate([padding, clearance_y, -w])
  cube([box_length - padding * 2, triangle_sz, w]);

  for (x = [0 : num_pins - 1]) {
    translate([padding + pin_triangle_length + pin_stride * x - clearance / 2,
               -w - eps - clearance_y,
               -w - eps - clearance / 2])
      cube([pin_length + clearance,
            w + 2 * clearance_y + 2 * eps,
            w + clearance + eps]);
  }
}

module zipper2(clearance) {
  color("green")
    for (x = [0 : num_pins]) {
      translate([padding + pin_stride * x + base_clearance / 2 - clearance / 2,
          -w - eps + clearance,
          -w - eps - clearance / 2])
        triangle(pin_stride - pin_length - base_clearance + clearance);
    }
}

// lock_transform()
//{
//  difference() {
//    union() {
//      triangle_base();
//      color("red") zipper(0);
//    };
//    zipper2(0.2);
//  }
//
//  color("blue")
//  difference() {
//    translate([0,-w,-w]) cube([box_length, w, w + triangle_sz]);
//    zipper(0.2);
//  }
//
//  color("green") zipper2(0);
//}


// Case clearance. Extra clearance for the case double wall inner (thiner) part.
cc = 0.1;

module bottom() {
  difference() {
    // Shell
    box(0.4 + cc + eps);

    // Shell subtractions
    box(eps - cc);
    plain_transform(-0.1 - cc) top_plain();


    cord();
    dupX() screw_sub();
    lock_transform() zipper(base_clearance);
  }

  difference() {
    // Shell
    box(w);

    // Shell subtractions
    box(0.4 + cc);
    plain_transform(w -0.1) top_plain();


    cord();
    dupX() screw_sub();
    lock_transform() zipper(base_clearance);
  }

  lock_transform() {
    difference() {
      translate([padding + base_clearance,-w,-w])
        cube([box_length - padding * 2 - 2 * base_clearance, w, w]);
      zipper(base_clearance);
    }
    zipper2(0);
  }

  dupX() screw_support();

  intersection() {
    box(-cc);

    // Additions restricted to internal space
    union() {
      dupX() {
        difference() { screw_add(); screw_sub(); }
      }
      mirrorY() origin() battery_bracket();
    }
  }

  // nano right support
  difference() {
    union() {
      mirrorX() mirrorY() origin() cube([w, nano_width, battery_and_holder_height_nc]);
      mirrorX() mirrorY() origin() cube([box_length - battery_holder_width, w, battery_and_holder_height_nc]);
    }
    cord();
  }

  // battery right support
  dupY() translate([battery_holder_width, 0, 0]) origin() mirrorX() battery_bracket();


  // Button brackets
  {
    sep_w = w;
    origin() translate([button_offset_x - button_width/2 - sep_w, 0, 0])
      cube([sep_w, button_width, button_height]);

    origin() translate([button_offset_x - w/2, button_width, 0])
      cube([w, 3, button_height]);
  }


}

shaft_d = 11;
module button_shaft() {
  cylinders([[nano_usb_height - nano_height + w, shaft_d, shaft_d]]);
}

module button_cap(off, top_height = 1.6, button_stick_out=0, eps = 0) {
  top_outer = shaft_d - 2 * 0.6;
  narrow_top_outer = top_outer - 0.8;
  narrow_height = 0.8;
  button_axe_d = 6;
  
  cylinders([[button_stick_out, narrow_top_outer + 2 * off, narrow_top_outer + 2 * off],
             [narrow_height + eps, narrow_top_outer + 2 * off, narrow_top_outer + 0.2 + 2 * off],
             [top_height - narrow_height, top_outer + 2 * off, top_outer + 2 * off],
             [box_height + w - button_d_h_real - top_height, button_axe_d, button_axe_d]]);

}

module button_transform(off = 0) {
  mirrorZ()
    translate([button_offset_x, button_offset_y, -w + off])
    origin() children();
}

module button_cap_final() {
  clearance = 0.1; // base_clearance / 2;
      intersection() {
        button_transform(-eps) button_cap(-clearance, 1.6, 0.5, 0);
        simple_box_verbose(w, -clearance, w);
      }
}

module top() {
  difference() {
    // Shell
    union() {
      intersection() {
        box(0.4);
        plain_transform(0.1) top_plain();
      }
      intersection() {
        box(w);
        plain_transform(w + 0.1) top_plain();
      }
    }

    // Shell subtractions
    box(eps);
    lock_transform() translate([padding,-w - eps,-w - eps])
      cube([box_length - padding * 2,
          w + clearance_y + 2 * eps,
          w + triangle_sz + 2 * eps]);

    intersection() {
      button_transform(-eps) button_cap(0, nano_usb_height - nano_height + w, 0);
      simple_box_verbose(w+eps, 0, w+eps);
    }

    d = 2.5;
    mirrorY() origin() translate([d/2 + 1.6, 3.05, 0]) cylinder(d=d, h=20, $fn=30);
    mirrorY() origin() translate([d/2 + 1, nano_width - 4.5, 0]) cylinder(d=d, h=20, $fn=30);
    lock_transform() zipper2(base_clearance);
  }

  screw_top(nano_usb_height);
  mirrorX() screw_top(box_height - screw_bottom_height - 0.2);

  lock_transform() { 
    difference() {
      union() {
        triangle_base();
        color("red")
          zipper(0);
      }
     zipper2(base_clearance);
    }
  }

  intersection() {
    box(0);
    button_transform(-eps)
      difference() {
        button_shaft();
        button_cap(0, nano_usb_height - nano_height + w, eps);
      }
  }
}

bottom();
// top();
// button_cap_final();

// legacy_origin() components();

module test_components() {
  difference() {
    legacy_origin() components();
    children();
  }
}

//test_components()
//  box(eps);

